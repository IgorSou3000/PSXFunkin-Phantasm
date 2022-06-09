/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "sonicm.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../random.h"
#include "../main.h"

//Sonic Menu player types
enum
{
	SonicMenu_ArcMain_SonicMenu0,
	SonicMenu_ArcMain_SonicMenu1,
	
	SonicMenu_ArcMain_Max,
};

#define SonicMenu_Arc_Max SonicMenu_ArcMain_Max

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[SonicMenu_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
	
} Char_SonicMenu;

//Sonic Menu player definitions
static const CharFrame char_sonicm_frame[] = {
	{SonicMenu_ArcMain_SonicMenu0, {  2,   2,  88, 121}, { 57, 118}}, //0 idle 1
	{SonicMenu_ArcMain_SonicMenu0, {116,   2,  88, 120}, { 56, 119}}, //1 idle 2
	{SonicMenu_ArcMain_SonicMenu0, {  3, 131,  88, 121}, { 55, 119}}, //2 idle 3
	{SonicMenu_ArcMain_SonicMenu0, {116, 130,  89, 122}, { 55, 120}}, //3 idle 4
	{SonicMenu_ArcMain_SonicMenu1, {  0,   0,  90, 123}, { 56, 121}}, //4 idle 5
	{SonicMenu_ArcMain_SonicMenu1, {113,   0,  90, 123}, { 55, 121}}, //5 idle 6
	
};

static const Animation char_sonicm_anim[PlayerAnim_Max] = {
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
	
	{5, (const u8[]){23, 24, 25, 26, 26, 26, 26, 26, 26, 26, ASCR_CHGANI, PlayerAnim_Dead1}}, //PlayerAnim_Dead0
	{5, (const u8[]){26, ASCR_REPEAT}},                                                       //PlayerAnim_Dead1
	{3, (const u8[]){27, 28, 29, 30, 30, 30, 30, 30, 30, 30, ASCR_CHGANI, PlayerAnim_Dead3}}, //PlayerAnim_Dead2
	{3, (const u8[]){30, ASCR_REPEAT}},                                                       //PlayerAnim_Dead3
	{3, (const u8[]){31, 32, 30, 30, 30, 30, 30, ASCR_CHGANI, PlayerAnim_Dead3}},             //PlayerAnim_Dead4
	{3, (const u8[]){33, 34, 30, 30, 30, 30, 30, ASCR_CHGANI, PlayerAnim_Dead3}},             //PlayerAnim_Dead5
	
	{10, (const u8[]){30, 30, 30, ASCR_BACK, 1}}, //PlayerAnim_Dead4
	{ 3, (const u8[]){33, 34, 30, ASCR_REPEAT}},  //PlayerAnim_Dead5
};

//SonicMenu player functions
void Char_SonicMenu_SetFrame(void *user, u8 frame)
{
	Char_SonicMenu *this = (Char_SonicMenu*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_sonicm_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_SonicMenu_Tick(Character *character)
{
	Char_SonicMenu *this = (Char_SonicMenu*)character;			

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
		if (Animatable_Ended(&character->animatable) &&
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
	
	//Animate and draw character
	Animatable_Animate(&character->animatable, (void*)this, Char_SonicMenu_SetFrame);
	Character_Draw(character, &this->tex, &char_sonicm_frame[this->frame]);
}

void Char_SonicMenu_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_SonicMenu_Free(Character *character)
{
	Char_SonicMenu *this = (Char_SonicMenu*)character;
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_SonicM_New(fixed_t x, fixed_t y)
{
	//Allocate boyfriend object
	Char_SonicMenu *this = Mem_Alloc(sizeof(Char_SonicMenu));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_SonicMenu_New] Failed to allocate boyfriend object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_SonicMenu_Tick;
	this->character.set_anim = Char_SonicMenu_SetAnim;
	this->character.free = Char_SonicMenu_Free;

	Animatable_Init(&this->character.animatable, char_sonicm_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = CHAR_SPEC_MISSANIM;
	
	//health icon
	this->character.health_i = 0;
	
	this->character.focus_x = (stage.mode == StageMode_2P) ? FIXED_DEC(-110,1) : FIXED_DEC(-20,1) ;
	this->character.focus_y = FIXED_DEC(-65,1);
	this->character.focus_zoom = (stage.mode == StageMode_2P) ? FIXED_DEC(1,1) : FIXED_DEC(12,10);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\SONICM.ARC;1");
	
	const char **pathp = (const char *[]){
		"sonic0.tim",   //SonicMenu_ArcMain_SonicMenu0
		"sonic1.tim",   //SonicMenu_ArcMain_SonicMenu1
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
