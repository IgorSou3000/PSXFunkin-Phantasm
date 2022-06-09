/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "week1.h"

#include "../archive.h"
#include "../mem.h"
#include "../stage.h"
#include "../timer.h"

//Week 1 background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	Gfx_Tex tex_back0; //back

	//making fade cuz i am lazy
	fixed_t fade, fadespd;
	boolean startfade;
} Back_Week1;

void Back_Week1_DrawFG(StageBack * back)
{
	Back_Week1 *this = (Back_Week1*)back;
     
	 //fade in the moment startfade be different of stage.fleetway
	if (this->startfade != stage.fleetway)
	{
	this->fade = FIXED_DEC(255,1);
	this->startfade = stage.fleetway;
	}

	//Draw white fade
	if (this->fade > 0)
	{
	static const RECT flash = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
	u8 flash_col = this->fade >> FIXED_SHIFT;
	Gfx_BlendRect(&flash, flash_col, flash_col, flash_col, 1);
	this->fade -= FIXED_MUL(FIXED_DEC(720,1), timer_dt);
	}
}
void Back_Week1_DrawBG(StageBack *back)
{
	Back_Week1 *this = (Back_Week1*)back;
	
	fixed_t fx, fy;

	fx = stage.camera.x;
	fy = stage.camera.y;

	if (stage.mode == StageMode_2P)
	{
     //fleetway back
	RECT back_src = {128, 0, 128, 72};
	RECT_FIXED back_dst = {
		FIXED_DEC(-200,1) - fx,
		FIXED_DEC(-150,1) - fy,
		FIXED_DEC(180,1),
		FIXED_DEC(320,1)
	};
	
	Stage_DrawTex(&this->tex_back0, &back_src, &back_dst, stage.camera.bzoom);

	//sonic back
	RECT back2_src = {0, 0, 128, 72};
	RECT_FIXED back2_dst = {
		FIXED_DEC(-20,1) - fx,
		FIXED_DEC(-150,1) - fy,
		FIXED_DEC(180,1),
		FIXED_DEC(320,1)
	};
	
	Stage_DrawTex(&this->tex_back0, &back2_src, &back2_dst, stage.camera.bzoom);
	}
	else
	{
	//fleetway back
	if (stage.fleetway == true)
	{
	RECT back_src = {128, 0, 128, 72};
	RECT_FIXED back_dst = {
		FIXED_DEC(-240,1) - fx,
		FIXED_DEC(-150,1) - fy,
		FIXED_DEC(540,1),
		FIXED_DEC(310,1)
	};
	
	Stage_DrawTex(&this->tex_back0, &back_src, &back_dst, stage.camera.bzoom);
	}
	//sonic back
	else
	{
	RECT back_src = {0, 0, 128, 72};
	RECT_FIXED back_dst = {
		FIXED_DEC(-240,1) - fx,
		FIXED_DEC(-150,1) - fy,
		FIXED_DEC(540,1),
		FIXED_DEC(310,1)
	};
	
	Stage_DrawTex(&this->tex_back0, &back_src, &back_dst, stage.camera.bzoom);
	}
  }
}

void Back_Week1_Free(StageBack *back)
{
	Back_Week1 *this = (Back_Week1*)back;
	
	//Free structure
	Mem_Free(this);
}

StageBack *Back_Week1_New(void)
{
	//Allocate background structure
	Back_Week1 *this = (Back_Week1*)Mem_Alloc(sizeof(Back_Week1));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_fg = Back_Week1_DrawFG;
	this->back.draw_md = NULL;
	this->back.draw_bg = Back_Week1_DrawBG;
	this->back.free = Back_Week1_Free;
	
	//Load background textures
	IO_Data arc_back = IO_Read("\\WEEK1\\BACK.ARC;1");
	Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "back0.tim"), 0);
	Mem_Free(arc_back);

	this->startfade = stage.fleetway = false;

	Gfx_SetClear(255, 255, 255);
	
	return (StageBack*)this;
}
