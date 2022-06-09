/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "monika.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../random.h"
#include "../main.h"

//Monika player types
enum
{
	Monika_ArcMain_Monika0,
	Monika_ArcMain_Monika1,
	Monika_ArcMain_Monika2,
	Monika_ArcMain_Monika3,
	Monika_ArcMain_Monika4,
	Monika_ArcMain_Monika5,
	Monika_ArcMain_Monika6,
	Monika_ArcMain_Monika7,
	Monika_ArcMain_Monika8,
	Monika_ArcMain_Monika9,
	Monika_ArcMain_Monika10,

	Lunatic_ArcMain_Lunatic0,
	Lunatic_ArcMain_Lunatic1,
	Lunatic_ArcMain_Lunatic2,
	Lunatic_ArcMain_Lunatic3,
	Lunatic_ArcMain_Lunatic4,
	Lunatic_ArcMain_Lunatic5,
	Lunatic_ArcMain_Lunatic6,
	Lunatic_ArcMain_Lunatic7,
	Lunatic_ArcMain_Lunatic8,
	Lunatic_ArcMain_Lunatic9,
	Lunatic_ArcMain_Lunatic10,
	Lunatic_ArcMain_Lunatic11,
	Lunatic_ArcMain_Lunatic12,
	Monika_ArcMain_Dead0, //BREAK
	
	Monika_ArcMain_Max,
};

#define Monika_Arc_Max Monika_ArcMain_Max

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Monika_Arc_Max];
	
	Gfx_Tex tex, tex_retry;
	u8 frame, tex_id;
	
} Char_Monika;

//Monika player definitions
static const CharFrame char_monika_frame[] = {
	{Monika_ArcMain_Monika0, {  4,   7, 113, 149}, { 80, 142}}, //0 idle 1
	{Monika_ArcMain_Monika0, {129,   4, 111, 149}, { 79, 142}}, //1 idle 2
	{Monika_ArcMain_Monika1, {  6,   5, 111, 149}, { 80, 142}}, //2 idle 3
	{Monika_ArcMain_Monika1, {128,   5, 110, 148}, { 80, 141}}, //3 idle 4
	{Monika_ArcMain_Monika2, {  9,   3, 108, 148}, { 79, 141}}, //4 idle 5
	{Monika_ArcMain_Monika2, {133,   3, 106, 148}, { 78, 141}}, //5 idle 6
	
	{Monika_ArcMain_Monika3, {   3,  12,  98, 143}, { 89, 136}}, //6 left 1
	{Monika_ArcMain_Monika3, { 127,  11,  99, 143}, { 90, 136}}, //7 left 2
	{Monika_ArcMain_Monika4, {  11,   3,  98, 143}, { 88, 136}}, //8 left 3
	{Monika_ArcMain_Monika4, { 140,   3,  98, 143}, { 87, 136}}, //9 left 4
	
	{Monika_ArcMain_Monika5, {  6,   8, 105, 133}, { 81, 126}}, //10 down 1
	{Monika_ArcMain_Monika5, {136,   7, 104, 134}, { 81, 127}}, //11 down 2
	{Monika_ArcMain_Monika6, {  8,   2, 103, 133}, { 80, 127}}, //12 down 3
	{Monika_ArcMain_Monika6, {132,   3, 101, 132}, { 79, 126}}, //13 down 4
	
	{Monika_ArcMain_Monika7, {  2,   7,  95, 151}, { 77, 144}}, //14 up 1
	{Monika_ArcMain_Monika7, {105,   9,  96, 150}, { 78, 143}}, //15 up 2
	{Monika_ArcMain_Monika8, {  4,   5,  97, 149}, { 78, 142}}, //16 up 3
	{Monika_ArcMain_Monika8, {131,   5,  98, 149}, { 78, 142}}, //17 up 4
	
	{Monika_ArcMain_Monika9, {  1,   2, 109, 143}, { 80, 136}}, //18 right 1
	{Monika_ArcMain_Monika9, {123,   2, 108, 142}, { 79, 135}}, //19 right 2
	{Monika_ArcMain_Monika10, {  5,  2, 106, 142}, { 79, 135}}, //20 right 3
	{Monika_ArcMain_Monika10, {128,  3, 105, 141}, { 79, 134}}, //21 right 4

	{Lunatic_ArcMain_Lunatic0, {  0,   0, 144, 156}, { 86, 145}}, //22 idle 1
	{Lunatic_ArcMain_Lunatic1, {  0,   0, 144, 156}, { 86, 145}}, //23 idle 2
	{Lunatic_ArcMain_Lunatic2, {  0,   0, 144, 156}, { 86, 145}}, //24 idle 3
	{Lunatic_ArcMain_Lunatic3, {  0,   0, 144, 156}, { 85, 145}}, //25 idle 4
	{Lunatic_ArcMain_Lunatic4, {  0,   0, 144, 156}, { 85, 145}}, //26 idle 5
	{Lunatic_ArcMain_Lunatic5, {  0,   0, 144, 156}, { 88, 142}}, //27 idle 6

	{Lunatic_ArcMain_Lunatic6, {  0,   0, 144, 156}, {104, 144}}, //28 left 1
	{Lunatic_ArcMain_Lunatic7, {  0,   0, 144, 156}, { 99, 140}}, //29 left 2

	{Lunatic_ArcMain_Lunatic8, {  0,   0, 144, 154}, { 94, 132}}, //30 down 1
	{Lunatic_ArcMain_Lunatic9, {  0,   0, 144, 154}, { 91, 134}}, //31 down 2

	{Lunatic_ArcMain_Lunatic10, {  0,   0, 144, 160}, { 82, 151}}, //32 up 1
	{Lunatic_ArcMain_Lunatic11, {  0,   0, 144, 160}, { 81, 149}}, //33 up 2

	{Lunatic_ArcMain_Lunatic12, {  0,   0, 148, 160}, { 81, 140}}, //34 right 1

	{Lunatic_ArcMain_Lunatic12, {  0,   0, 1, 1}, { 81, 140}}, //35 right 1
};

static const Animation char_monika_anim[PlayerAnim_Max] = {
	{2, (const u8[]){ 0,  1,  2,  3,  4,  5, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 6,  7,  8,  9, ASCR_BACK, 1}},             //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_LeftAlt
	{2, (const u8[]){10, 11, 12, 13, ASCR_BACK, 1}},             //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_DownAlt
	{2, (const u8[]){14, 15, 16, 17, ASCR_BACK, 1}},             //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_UpAlt
	{2, (const u8[]){18, 19, 20, 21, ASCR_BACK, 1}},             //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_RightAlt

	{1, (const u8[]){ 6, 20, 20, 21, ASCR_BACK, 1}},     //PlayerAnim_LeftMiss
	{1, (const u8[]){10, 22, 22, 23, ASCR_BACK, 1}},     //PlayerAnim_DownMiss
	{1, (const u8[]){12, 24, 24, 25, ASCR_BACK, 1}},     //PlayerAnim_UpMiss
	{1, (const u8[]){16, 26, 26, 27, ASCR_BACK, 1}},     //PlayerAnim_RightMiss
	
	{2, (const u8[]){13, 14, 15, ASCR_BACK, 1}},         //PlayerAnim_Peace
	{2, (const u8[]){16, 17, 18, 19, ASCR_REPEAT}},      //PlayerAnim_Sweat
	
	{5, (const u8[]){35, ASCR_CHGANI, PlayerAnim_Dead1}}, //PlayerAnim_Dead0
	{5, (const u8[]){35, ASCR_REPEAT}},                                                       //PlayerAnim_Dead1
	{3, (const u8[]){35, ASCR_CHGANI, PlayerAnim_Dead3}}, //PlayerAnim_Dead2
	{3, (const u8[]){35, ASCR_REPEAT}},                                                       //PlayerAnim_Dead3
	{3, (const u8[]){35, ASCR_CHGANI, PlayerAnim_Dead3}},             //PlayerAnim_Dead4
	{3, (const u8[]){35, ASCR_CHGANI, PlayerAnim_Dead3}},             //PlayerAnim_Dead5
	
	{10, (const u8[]){30, 30, 30, ASCR_BACK, 1}}, //PlayerAnim_Dead4
	{ 3, (const u8[]){33, 34, 30, ASCR_REPEAT}},  //PlayerAnim_Dead5
};
static const Animation char_fleetway_anim[PlayerAnim_Max] = {
	{2, (const u8[]){22, 23, 24, 25, 26, 27, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){28, 29, ASCR_BACK, 1}},             //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_LeftAlt
	{2, (const u8[]){30, 31, ASCR_BACK, 1}},             //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_DownAlt
	{2, (const u8[]){32, 33, ASCR_BACK, 1}},             //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_UpAlt
	{2, (const u8[]){34, ASCR_BACK, 1}},             //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_RightAlt

	{1, (const u8[]){ 6, 20, 20, 21, ASCR_BACK, 1}},     //PlayerAnim_LeftMiss
	{1, (const u8[]){10, 22, 22, 23, ASCR_BACK, 1}},     //PlayerAnim_DownMiss
	{1, (const u8[]){12, 24, 24, 25, ASCR_BACK, 1}},     //PlayerAnim_UpMiss
	{1, (const u8[]){16, 26, 26, 27, ASCR_BACK, 1}},     //PlayerAnim_RightMiss
	
	{2, (const u8[]){13, 14, 15, ASCR_BACK, 1}},         //PlayerAnim_Peace
	{2, (const u8[]){16, 17, 18, 19, ASCR_REPEAT}},      //PlayerAnim_Sweat
	
	{5, (const u8[]){35, ASCR_CHGANI, PlayerAnim_Dead1}}, //PlayerAnim_Dead0
	{5, (const u8[]){35, ASCR_REPEAT}},                                                       //PlayerAnim_Dead1
	{3, (const u8[]){35, ASCR_CHGANI, PlayerAnim_Dead3}}, //PlayerAnim_Dead2
	{3, (const u8[]){35, ASCR_REPEAT}},                                                       //PlayerAnim_Dead3
	{3, (const u8[]){35, ASCR_CHGANI, PlayerAnim_Dead3}},             //PlayerAnim_Dead4
	{3, (const u8[]){35, ASCR_CHGANI, PlayerAnim_Dead3}},             //PlayerAnim_Dead5
	
	{10, (const u8[]){30, 30, 30, ASCR_BACK, 1}}, //PlayerAnim_Dead4
	{ 3, (const u8[]){33, 34, 30, ASCR_REPEAT}},  //PlayerAnim_Dead5
};

//Monika player functions
void Char_Monika_SetFrame(void *user, u8 frame)
{
	Char_Monika *this = (Char_Monika*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_monika_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Monika_Tick(Character *character)
{
	Char_Monika *this = (Char_Monika*)character;			

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
	Animatable_Animate(&character->animatablecrazy, (void*)this, Char_Monika_SetFrame);
	this->character.health_i = 3;
	this->character.health_b = 0xFFffaa30;
	}

	else
	{
	Animatable_Animate(&character->animatable, (void*)this, Char_Monika_SetFrame);
	this->character.health_i = 2;
	this->character.health_b = 0xFF288928;
	}
	
	//Animate and draw character
	Character_Draw(character, &this->tex, &char_monika_frame[this->frame]);
}

void Char_Monika_SetAnim(Character *character, u8 anim)
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

void Char_Monika_Free(Character *character)
{
	Char_Monika *this = (Char_Monika*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Monika_New(fixed_t x, fixed_t y)
{
	//Allocate boyfriend object
	Char_Monika *this = Mem_Alloc(sizeof(Char_Monika));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Monika_New] Failed to allocate boyfriend object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Monika_Tick;
	this->character.set_anim = Char_Monika_SetAnim;
	this->character.free = Char_Monika_Free;

	Animatable_Init(&this->character.animatable, char_monika_anim);
	Animatable_Init(&this->character.animatablecrazy, char_fleetway_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	//health icon
	this->character.health_i = 2;

	//health bar color
	this->character.health_b = 0xFF288928;
	
	this->character.focus_x = (stage.mode == StageMode_2P) ? FIXED_DEC(-110,1) : FIXED_DEC(-20,1) ;
	this->character.focus_y = FIXED_DEC(-65,1);
	this->character.focus_zoom = (stage.mode == StageMode_2P) ? FIXED_DEC(1,1) : FIXED_DEC(12,10);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\MONIKA.ARC;1");
	
	const char **pathp = (const char *[]){
		"monika0.tim",   //Monika_ArcMain_Monika0
		"monika1.tim",   //Monika_ArcMain_Monika1
		"monika2.tim",   //Monika_ArcMain_Monika2
		"monika3.tim",   //Monika_ArcMain_Monika3
		"monika4.tim",   //Monika_ArcMain_Monika4
		"monika5.tim",   //Monika_ArcMain_Monika5
		"monika6.tim",   //Monika_ArcMain_Monika6
		"monika7.tim",   //Monika_ArcMain_Monika7
		"monika8.tim",   //Monika_ArcMain_Monika8
		"monika9.tim",   //Monika_ArcMain_Monika9
		"monika10.tim",   //Monika_ArcMain_Monika10

		"lunatic0.tim",   //Lunatic_ArcMain_Lunatic0
		"lunatic1.tim",   //Lunatic_ArcMain_Lunatic1
		"lunatic2.tim",   //Lunatic_ArcMain_Lunatic2
		"lunatic3.tim",   //Lunatic_ArcMain_Lunatic3
		"lunatic4.tim",   //Lunatic_ArcMain_Lunatic4
		"lunatic5.tim",   //Lunatic_ArcMain_Lunatic5
		"lunatic6.tim",   //Lunatic_ArcMain_Lunatic6
		"lunatic7.tim",   //Lunatic_ArcMain_Lunatic7
		"lunatic8.tim",   //Lunatic_ArcMain_Lunatic8
		"lunatic9.tim",   //Lunatic_ArcMain_Lunatic9
		"luna10.tim",   //Lunatic_ArcMain_Lunatic10
		"luna11.tim",   //Lunatic_ArcMain_Lunatic11
		"luna12.tim",   //Lunatic_ArcMain_Lunatic12
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
