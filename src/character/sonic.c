/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "sonic.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../random.h"
#include "../main.h"

//Sonic player types
enum
{
	Sonic_ArcMain_Sonic0,
	Sonic_ArcMain_Sonic1,
	Sonic_ArcMain_Sonic2,
	Sonic_ArcMain_Sonic3,
	Sonic_ArcMain_Sonic4,
	Sonic_ArcMain_Sonic5,
	Sonic_ArcMain_Sonic6,
	Fleetway_ArcMain_Fleetway0,
	Fleetway_ArcMain_Fleetway1,
	Fleetway_ArcMain_Fleetway2,
	Fleetway_ArcMain_Fleetway3,
	Fleetway_ArcMain_Fleetway4,
	Fleetway_ArcMain_Fleetway5,
	Fleetway_ArcMain_Fleetway6,
	Fleetway_ArcMain_Fleetway7,
	Fleetway_ArcMain_Fleetway8,
	Fleetway_ArcMain_Fleetway9,
	Fleetway_ArcMain_Fleetway10,
	Sonic_ArcMain_Death, //BREAK
	
	Sonic_ArcMain_Max,
};

#define Sonic_Arc_Max Sonic_ArcMain_Max

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Sonic_Arc_Max];
	
	Gfx_Tex tex, tex_retry;
	u8 frame, tex_id;
	
} Char_Sonic;

//Sonic player definitions
static const CharFrame char_sonic_frame[] = {
	{Sonic_ArcMain_Sonic0, {  2,   2,  88, 121}, { 76, 112}}, //0 idle 1
	{Sonic_ArcMain_Sonic0, {116,   2,  88, 120}, { 75, 112}}, //1 idle 2
	{Sonic_ArcMain_Sonic0, {  3, 131,  88, 121}, { 74, 113}}, //2 idle 3
	{Sonic_ArcMain_Sonic0, {116, 130,  89, 122}, { 74, 114}}, //3 idle 4
	{Sonic_ArcMain_Sonic1, {  1,   1,  89, 122}, { 73, 114}}, //4 idle 5
	{Sonic_ArcMain_Sonic1, {114,   1,  89, 122}, { 73, 114}}, //5 idle 6
	
	{Sonic_ArcMain_Sonic1, {   3, 129,  88, 123}, { 83, 116}}, //6 left 1
	{Sonic_ArcMain_Sonic1, { 104, 129,  89, 124}, { 82, 117}}, //7 left 2
	{Sonic_ArcMain_Sonic2, {   3,   1,  88, 124}, { 81, 117}}, //8 left 3
	{Sonic_ArcMain_Sonic2, { 104,   1,  88, 124}, { 81, 117}}, //9 left 4
	
	{Sonic_ArcMain_Sonic2, {  0, 133,  89, 122}, { 74, 110}}, //10 down 1
	{Sonic_ArcMain_Sonic2, {111, 132,  88, 122}, { 73, 111}}, //11 down 2
	
	{Sonic_ArcMain_Sonic3, { 10,   3,  96, 119}, { 74, 113}}, //12 up 1
	{Sonic_ArcMain_Sonic3, {119,   3,  96, 118}, { 74, 112}}, //13 up 2
	{Sonic_ArcMain_Sonic3, {  2, 130,  95, 117}, { 74, 111}}, //14 up 3
	{Sonic_ArcMain_Sonic3, {113, 130,  95, 117}, { 74, 110}}, //15 up 2
	
	{Sonic_ArcMain_Sonic4, {  9,   4, 103, 123}, { 74, 115}}, //16 right 1
	{Sonic_ArcMain_Sonic4, {124,   5, 103, 122}, { 74, 114}}, //17 right 2
	{Sonic_ArcMain_Sonic4, {  2, 133, 102, 123}, { 74, 115}}, //18 right 3
	{Sonic_ArcMain_Sonic4, {117, 133, 102, 122}, { 74, 116}}, //19 right 4
	
	{Sonic_ArcMain_Sonic5, {  4,   2,  88, 123}, { 83, 115}}, //20 left miss 1
	{Sonic_ArcMain_Sonic5, {122,   2,  89, 123}, { 86, 115}}, //21 left miss 2
	
	{Sonic_ArcMain_Sonic5, {  7, 129,  90, 118}, { 76, 105}}, //22 down miss 1
	{Sonic_ArcMain_Sonic5, {136, 130,  90, 117}, { 76, 104}}, //23 down miss 2
	
	{Sonic_ArcMain_Sonic6, { 19,   2,  96, 119}, { 74, 111}}, //24 up miss 1
	{Sonic_ArcMain_Sonic6, {128,   0,  93, 123}, { 71, 114}}, //25 up miss 2
	
	{Sonic_ArcMain_Sonic6, {  6, 130, 104, 123}, { 75, 115}}, //26 right miss 1
	{Sonic_ArcMain_Sonic6, {114, 132, 106, 123}, { 76, 114}}, //27 right miss 2

    {Fleetway_ArcMain_Fleetway0, {  3,   2,  90, 136}, { 87, 128}}, //28 idle and left 1
	{Fleetway_ArcMain_Fleetway0, {105,   2,  90, 136}, { 86, 129}}, //29 idle and left 2
	{Fleetway_ArcMain_Fleetway1, {  3,   2,  90, 136}, { 85, 129}}, //30 idle and left 3
	{Fleetway_ArcMain_Fleetway1, {105,   2,  90, 137}, { 85, 129}}, //31 idle and left 4
	
	{Fleetway_ArcMain_Fleetway2, {  0,   2,  90, 133}, { 79, 121}}, //32 down 1
	{Fleetway_ArcMain_Fleetway2, {103,   2,  90, 133}, { 78, 122}}, //33 down 2
	{Fleetway_ArcMain_Fleetway3, {  3,   3,  91, 134}, { 78, 123}}, //34 down 1
	{Fleetway_ArcMain_Fleetway3, {106,   3,  91, 134}, { 78, 123}}, //35 down 2
	
	{Fleetway_ArcMain_Fleetway4, {  3,   4, 104, 123}, { 74, 117}}, //36 up 1
	{Fleetway_ArcMain_Fleetway4, {120,   4, 103, 123}, { 74, 116}}, //37 up 2
	{Fleetway_ArcMain_Fleetway5, {  7,   2, 101, 123}, { 74, 116}}, //38 up 3
	{Fleetway_ArcMain_Fleetway5, {121,   2, 101, 123}, { 74, 115}}, //39 up 2
	
	{Fleetway_ArcMain_Fleetway10, {  0,  15, 104, 134}, { 75, 126}}, //40 right 1
	{Fleetway_ArcMain_Fleetway10, {117,  16, 103, 133}, { 74, 125}}, //41 right 2
	
	{Fleetway_ArcMain_Fleetway6, {  1,   1,  90, 136}, { 87, 128}}, //42 left miss 1
	{Fleetway_ArcMain_Fleetway6, {103,   1,  91, 136}, { 90, 128}}, //43 left miss 2
	
	{Fleetway_ArcMain_Fleetway7, {  0,   6,  94, 133}, { 80, 121}}, //44 down miss 1
	{Fleetway_ArcMain_Fleetway7, {107,   6,  97, 130}, { 82, 118}}, //45 down miss 2
	
	{Fleetway_ArcMain_Fleetway8, {  7,   1, 106, 128}, { 74, 121}}, //46 up miss 1
	{Fleetway_ArcMain_Fleetway8, {126,   1, 101, 131}, { 70, 123}}, //47 up miss 2
	
	{Fleetway_ArcMain_Fleetway9, {  3,   1, 107, 135}, { 75, 127}}, //48 right miss 1
	{Fleetway_ArcMain_Fleetway9, {123,   2, 107, 134}, { 75, 126}}, //49 right miss 2

	{Sonic_ArcMain_Death, {  2,   2,  87, 120}, { 76, 112}}, //0 idle 1
	{Sonic_ArcMain_Death, {114,   3,  87, 120}, { 77, 112}}, //1 idle 2
	{Sonic_ArcMain_Death, { 12, 133,  87, 120}, { 77, 111}}, //2 idle 3
};

static const Animation char_sonic_anim[PlayerAnim_Max] = {
	{2, (const u8[]){ 0,  1,  2,  3,  4,  5, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 6,  7,  8,  9, ASCR_BACK, 1}},             //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_LeftAlt
	{2, (const u8[]){10, 11, ASCR_BACK, 1}},             //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_DownAlt
	{2, (const u8[]){12, 13, 14, 15, ASCR_BACK, 1}},             //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_UpAlt
	{2, (const u8[]){16, 17, 18, 19, ASCR_BACK, 1}},             //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_RightAlt
	
	{1, (const u8[]){ 6, 20, 20, 21, ASCR_BACK, 1}},     //PlayerAnim_LeftMiss
	{1, (const u8[]){10, 22, 22, 23, ASCR_BACK, 1}},     //PlayerAnim_DownMiss
	{1, (const u8[]){12, 24, 24, 25, ASCR_BACK, 1}},     //PlayerAnim_UpMiss
	{1, (const u8[]){16, 26, 26, 27, ASCR_BACK, 1}},     //PlayerAnim_RightMiss
	
	{2, (const u8[]){13, 14, 15, ASCR_BACK, 1}},         //PlayerAnim_Peace
	{2, (const u8[]){16, 17, 18, 19, ASCR_REPEAT}},      //PlayerAnim_Sweat
	
	{2, (const u8[]){10, 11, ASCR_CHGANI, PlayerAnim_Dead1}}, //PlayerAnim_Dead0
	{5, (const u8[]){50, ASCR_REPEAT}},                                                       //PlayerAnim_Dead1
	{6, (const u8[]){52, 51, 50,ASCR_CHGANI, PlayerAnim_Dead3}}, //PlayerAnim_Dead2
	{6, (const u8[]){52, 51, 50, ASCR_CHGANI, PlayerAnim_Dead3}},                                                       //PlayerAnim_Dead3
	{3, (const u8[]){50, 51, 52, ASCR_CHGANI, PlayerAnim_Dead3}},             //PlayerAnim_Dead4
	{3, (const u8[]){50, 51, 52, ASCR_CHGANI, PlayerAnim_Dead3}},             //PlayerAnim_Dead5
	
	{10, (const u8[]){30, 30, 30, ASCR_BACK, 1}}, //PlayerAnim_Dead4
	{ 3, (const u8[]){33, 34, 30, ASCR_REPEAT}},  //PlayerAnim_Dead5
};
static const Animation char_fleetway_anim[PlayerAnim_Max] = {
	{2, (const u8[]){28, 29, 30, 31, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){28, 29, 30, 31, ASCR_BACK, 1}},             //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_LeftAlt
	{2, (const u8[]){32, 33, 34, 35, ASCR_BACK, 1}},             //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_DownAlt
	{2, (const u8[]){36, 37, 38, 39, ASCR_BACK, 1}},             //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_UpAlt
	{2, (const u8[]){40, 41, ASCR_BACK, 1}},             //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_RightAlt
	
	{1, (const u8[]){28, 42, 42, 43, ASCR_BACK, 1}},     //PlayerAnim_LeftMiss
	{1, (const u8[]){32, 44, 44, 45, ASCR_BACK, 1}},     //PlayerAnim_DownMiss
	{1, (const u8[]){36, 46, 46, 47, ASCR_BACK, 1}},     //PlayerAnim_UpMiss
	{1, (const u8[]){40, 48, 48, 49, ASCR_BACK, 1}},     //PlayerAnim_RightMiss
	
	{2, (const u8[]){13, 14, 15, ASCR_BACK, 1}},         //PlayerAnim_Peace
	{2, (const u8[]){16, 17, 18, 19, ASCR_REPEAT}},      //PlayerAnim_Sweat
	
	{2, (const u8[]){10, 11, ASCR_CHGANI, PlayerAnim_Dead1}}, //PlayerAnim_Dead0
	{5, (const u8[]){50, ASCR_REPEAT}},                                                       //PlayerAnim_Dead1
	{6, (const u8[]){52, 51, 50,ASCR_CHGANI, PlayerAnim_Dead3}}, //PlayerAnim_Dead2
	{6, (const u8[]){52, 51, 50, ASCR_CHGANI, PlayerAnim_Dead3}},                                                       //PlayerAnim_Dead3
	{3, (const u8[]){50, 51, 52, ASCR_CHGANI, PlayerAnim_Dead3}},             //PlayerAnim_Dead4
	{3, (const u8[]){50, 51, 52, ASCR_CHGANI, PlayerAnim_Dead3}},             //PlayerAnim_Dead5
	
	{10, (const u8[]){30, 30, 30, ASCR_BACK, 1}}, //PlayerAnim_Dead4
	{ 3, (const u8[]){33, 34, 30, ASCR_REPEAT}},  //PlayerAnim_Dead5
};

//Sonic player functions
void Char_Sonic_SetFrame(void *user, u8 frame)
{
	Char_Sonic *this = (Char_Sonic*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_sonic_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Sonic_Tick(Character *character)
{
	Char_Sonic *this = (Char_Sonic*)character;			

	//Handle animation updates
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0 ||
	    (character->animatable.anim != CharAnim_Left &&
	     character->animatable.anim != CharAnim_LeftAlt &&
	     character->animatable.anim != CharAnim_Down &&
	     character->animatable.anim != CharAnim_DownAlt &&
	     character->animatable.anim != CharAnim_Up &&
	     character->animatable.anim != CharAnim_UpAlt &&
	     character->animatable.anim != CharAnim_Right &&
	     character->animatable.anim != CharAnim_RightAlt))
		Character_CheckEndSing(character);
	
	if (stage.flag & STAGE_FLAG_JUST_STEP)
	{
		//Perform idle dance
		if ((Animatable_Ended(&character->animatable) || (Animatable_Ended(&character->animatablecrazy))) &&
			(character->animatable.anim != CharAnim_Left &&
		     character->animatable.anim != CharAnim_LeftAlt &&
		     character->animatable.anim != PlayerAnim_LeftMiss &&
		     character->animatable.anim != CharAnim_Down &&
		     character->animatable.anim != CharAnim_DownAlt &&
		     character->animatable.anim != PlayerAnim_DownMiss &&
		     character->animatable.anim != CharAnim_Up &&
		     character->animatable.anim != CharAnim_UpAlt &&
		     character->animatable.anim != PlayerAnim_UpMiss &&
		     character->animatable.anim != CharAnim_Right &&
		     character->animatable.anim != CharAnim_RightAlt &&
		     character->animatable.anim != PlayerAnim_RightMiss) &&
			(stage.song_step & 0x7) == 0)
			character->set_anim(character, CharAnim_Idle);
	}

	//swaphealth bar color and icon
	if (stage.fleetway == true)
	{
	Animatable_Animate(&character->animatablecrazy, (void*)this, Char_Sonic_SetFrame);
	this->character.health_i = 1;
	this->character.health_b = 0xFFffaa30;
	}

	else
	{
	Animatable_Animate(&character->animatable, (void*)this, Char_Sonic_SetFrame);
	this->character.health_i = 0;
	this->character.health_b = 0xFF284289;
	}
	
	//Animate and draw character
	Character_Draw(character, &this->tex, &char_sonic_frame[this->frame]);
}

void Char_Sonic_SetAnim(Character *character, u8 anim)
{
	//Perform animation checks
	switch (anim)
	{
		case PlayerAnim_Dead0:
			character->focus_x = FIXED_DEC(0,1);
			character->focus_y = FIXED_DEC(-40,1);
			character->focus_zoom = FIXED_DEC(125,100);
			break;
	}
	
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Animatable_SetAnim(&character->animatablecrazy, anim);
	Character_CheckStartSing(character);
}

void Char_Sonic_Free(Character *character)
{
	Char_Sonic *this = (Char_Sonic*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Sonic_New(fixed_t x, fixed_t y)
{
	//Allocate boyfriend object
	Char_Sonic *this = Mem_Alloc(sizeof(Char_Sonic));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Sonic_New] Failed to allocate boyfriend object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Sonic_Tick;
	this->character.set_anim = Char_Sonic_SetAnim;
	this->character.free = Char_Sonic_Free;

	Animatable_Init(&this->character.animatable, char_sonic_anim);
	Animatable_Init(&this->character.animatablecrazy, char_fleetway_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = CHAR_SPEC_MISSANIM;
	
	//health icon
	this->character.health_i = 0;

	//health bar color
	this->character.health_b = 0xFF284289;
	
	this->character.focus_x = (stage.mode == StageMode_2P) ? FIXED_DEC(-110,1) : FIXED_DEC(-20,1) ;
	this->character.focus_y = FIXED_DEC(-65,1);
	this->character.focus_zoom = (stage.mode == StageMode_2P) ? FIXED_DEC(1,1) : FIXED_DEC(12,10);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\SONIC.ARC;1");
	
	const char **pathp = (const char *[]){
		"sonic0.tim",   //Sonic_ArcMain_Sonic0
		"sonic1.tim",   //Sonic_ArcMain_Sonic1
		"sonic2.tim",   //Sonic_ArcMain_Sonic2
		"sonic3.tim",   //Sonic_ArcMain_Sonic3
		"sonic4.tim",   //Sonic_ArcMain_Sonic4
		"sonic5.tim",   //Sonic_ArcMain_Sonic5
		"sonic6.tim",   //Sonic_ArcMain_Sonic6
        "fleet0.tim",   //Fleetway_ArcMain_Fleetway0
		"fleet1.tim",   //Fleetway_ArcMain_Fleetway1
		"fleet2.tim",   //Fleetway_ArcMain_Fleetway2
		"fleet3.tim",   //Fleetway_ArcMain_Fleetway3
		"fleet4.tim",   //Fleetway_ArcMain_Fleetway4
		"fleet5.tim",   //Fleetway_ArcMain_Fleetway5
		"fleet6.tim",   //Fleetway_ArcMain_Fleetway6
		"fleet7.tim",   //Fleetway_ArcMain_Fleetway7
		"fleet8.tim",   //Fleetway_ArcMain_Fleetway8
		"fleet9.tim",   //Fleetway_ArcMain_Fleetway9
		"fleet10.tim",   //Fleetway_ArcMain_Fleetway9
		"death.tim",   //Sonic_ArcMain_Death
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
