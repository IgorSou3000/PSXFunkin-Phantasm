/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "loadscr.h"

#include "gfx.h"
#include "timer.h"
#include "io.h"
#include "audio.h"
#include "trans.h"
#include "network.h"
#include "random.h"

u8 randomload;

//Loading screen functions
void LoadScr_Start(void)
{
	//this chance for special loading
	if (randomload <= 0)
	randomload = RandomRange(1, 20);

	//Stop music and make sure frame has been drawn
	Audio_StopXA();
	Network_Process();
	Gfx_Flip();
	
	//Load loading screen texture
	RECT loading_src = {0, 0, 255, 255};
	
	RECT loadingrandom_dst = {(SCREEN_WIDTH - 300) >> 1, (SCREEN_HEIGHT - 224) >> 1, 300, 224};
	RECT loading_dst = {(SCREEN_WIDTH - 300) >> 1, (SCREEN_HEIGHT - 242) >> 1, 320, 240};

	Gfx_Tex loading_tex;
	if (randomload == 20)
	{
	Gfx_SetClear(255, 255, 255);
	Gfx_LoadTex(&loading_tex, IO_Read("\\MENU\\LOADRAN.TIM;1"), GFX_LOADTEX_FREE);
	}
	else
	{
	Gfx_SetClear(0, 0, 0);
	Gfx_LoadTex(&loading_tex, IO_Read("\\MENU\\LOADING.TIM;1"), GFX_LOADTEX_FREE);
	}
	Timer_Reset();
	
	//Draw loading screen and run transition
	Trans_Set();
	
	while (!Trans_Idle())
	{
		//Draw loading screen and end frame
		Timer_Tick();
		Trans_Tick();
		if (randomload == 20)
		Gfx_DrawTex(&loading_tex, &loading_src, &loadingrandom_dst);
		else
		Gfx_DrawTex(&loading_tex, &loading_src, &loading_dst);
		Network_Process();
		Gfx_Flip();
	}
	
	//Draw an extra frame to avoid double buffering issues
	if (randomload == 20)
	Gfx_DrawTex(&loading_tex, &loading_src, &loadingrandom_dst);
	else
	Gfx_DrawTex(&loading_tex, &loading_src, &loading_dst);
	Network_Process();
	Gfx_Flip();
}

void LoadScr_End(void)
{
	//Handle transition out
	Timer_Reset();
	Trans_Clear();
	Trans_Start();
	Gfx_DisableClear();
	while (!Trans_Tick())
	{
		Timer_Tick();
		Network_Process();
		Gfx_Flip();
	}
	Gfx_EnableClear();
	randomload = 0;
}
