#include <math.h>
#include <math_neon.h>
#include <stdio.h>
#include <sys/unistd.h>
#include <SDL/SDL.h>
#include <mikmod.h>
#include <newlib.h>
#include "ZeDemo.h"
#include "effects.h"
#include "precalcs.h"
#include "engine3d.h"
#include <psp2/power.h>
#include <psp2/kernel/clib.h>
#include <psp2/kernel/processmgr.h>

SDL_Surface *screen;
SDL_Joystick *joystick;
SDL_Event event;

MODULE *module;

int quit = 0;

int nfrm=0, pfrm=0;
int fps=0, atime=0;
int partime=0, prticks=0;
char sbuffer[64];

int part = 0;
int nparts = 5;

extern int objshow_num, objshow_rmode, objshow_shade;
int maxobj = 7, maxshade = 13;
int rmode = 4;
int zbflag = 0;

extern int render_shit;
extern unsigned short shades[64][256];

extern unsigned short loading[];

void InitMusic()
{
	MikMod_RegisterAllDrivers();
    MikMod_RegisterAllLoaders();
	md_mode |= DMODE_SOFT_MUSIC;
	MikMod_Init("");
    module = Player_Load("app0:creeptea.mod", 64, 0);
    if (module) {
        Player_Start(module);
        }
}

void Wait(int seconds)
{
    int atime = SDL_GetTicks();
    while(SDL_GetTicks() - atime < (seconds * 1000)){};
}

void ClearScreen()
{
    memset((unsigned int*)screen->pixels, 0, sizeof(unsigned short) * SSIZE);
}



void Script()
{
    unsigned short *vram = (unsigned short*)screen->pixels;

    prticks = SDL_GetTicks() - partime;
    int leblast = 2192;

    //printf("DEBUG: Script() Part case Number = %d\n", part);
	switch(part)
	{
		case 0:
            Floor(vram, 64, 64);
            objshow_num = 0;
            objshow_shade = 1;
            objshow_rmode = GOURAUD;
            render_shit = 0;
			RunScene3d(vram,0);
			if (prticks>leblast*6) part = 1;
		break;

        case 1:
			Radial(vram,1);
			if (prticks>leblast*11.75) part = 2;
		break;

		case 2:
            DrawSky(vram);
            objshow_num = 1;
            objshow_shade = 1;
            objshow_rmode = ENVMAP;
            render_shit = 0;
			RunScene3d(vram,1);
			if (prticks>leblast*23) part = 3;
		break;

		case 3:
            Polarplasma(vram, shades[4]);
            objshow_num = 2;
            objshow_shade = 6;
            objshow_rmode = GOURAUD;
            render_shit = 0;
			RunScene3d(vram,0);
			if (prticks>leblast*34.5) part = 4;
		break;

		case 4:
            Plasma(vram, shades[5]);
            objshow_num = 3;
            objshow_shade = 2;
            objshow_rmode = GOURAUD;
            render_shit = 0;
			RunScene3d(vram,0);
			if (prticks>leblast*46) part = 5;
		break;

		case 5:
            ClearScreen();
            objshow_num = 6;
            render_shit = 1;
			RunScene3d(vram,0);
			if (prticks>leblast*57.5)
			{
                InitRadialBitmap2();
                part = 6;
            }
		break;

		case 6:
			Radial(vram,0);
			if (prticks>leblast*68.5)
                quit = 1;
		break;

		default:
		break;
	}
}


void Init()
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0)
    {
        printf("DEBUG: SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0. Failed\n");
    	sceKernelExitProcess(0);
    }

	screen = SDL_SetVideoMode(WIDTH, HEIGHT, 16, SDL_SWSURFACE);
	if (!screen)
    {
        printf("DEBUG: screen Failed.\n");
        sceKernelExitProcess(0);
    }
    SDL_SetVideoModeScaling(118, 0, 725, 544);
	joystick = SDL_JoystickOpen(0);
	if (!joystick)
    {
        printf("DEBUG: joystick failed.\n");
        sceKernelExitProcess(0);
    }
	SDL_ShowCursor(SDL_DISABLE);
    SDL_LockSurface(screen);
    int i;
    unsigned short *vram = (unsigned short*)screen->pixels;
    for (i = 0; i<SSIZE; i++)
        *(vram+i) = loading[i];
    SDL_UnlockSurface(screen);
    SDL_Flip(screen);
    precalcs();
    partime = SDL_GetTicks();
    InitMusic();
}

void CountFps()
{
	if (SDL_GetTicks()-atime>=1000)
	{
		atime = SDL_GetTicks();
		fps=(nfrm-pfrm);
		pfrm=nfrm;
	}
//	sprintf(sbuffer, "FPS = %d", fps);
//  DrawText_(8, 16, 16, sbuffer, 16, (unsigned short*)screen->pixels);
}

void CheckJoy()
{
	while (SDL_PollEvent(&event))
	{
		switch (event.type) 
		{
        case SDL_JOYBUTTONDOWN:
            switch(event.jbutton.button)
            {
                case GP2X_BUTTON_START:
                    quit = 1;
                    break;
/*
                case GP2X_BUTTON_L:
                    part = (part + 1) % nparts;
                    break;
                case GP2X_BUTTON_R:
                    part--;
                    if (part<0) part = nparts - 1;
                    break;
*/
	        }
        	break;
		}
	}
}


int main(int argc, char *argv[])
{
    //MAXIMUM SPEEEED
    scePowerSetArmClockFrequency(444);
    scePowerSetBusClockFrequency(222);
    Init();
	
    do{
    SDL_LockSurface(screen);
	Script();
	CountFps();
	nfrm++;
    SDL_UnlockSurface(screen);
    SDL_Flip(screen);
    if (Player_Active())
        MikMod_Update();
	CheckJoy();
	}while(!quit);

    SDL_Quit();
    Player_Stop();
    Player_Free(module);
    MikMod_Exit();
	sceKernelExitProcess(0);
	return 0;
}
