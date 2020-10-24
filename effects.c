#include <math.h>
#include <SDL/SDL_timer.h>
#include "ZeDemo.h"
#include "effects.h"
#include "precalcs.h"
#include "engine3d.h"

#define SWAP8(x) ( (((x) >> 4) & 0x0F) | (((x) << 4) & 0xF0) ) 
#define SWAP16(x) ( (((x) >> 8) & 0x00FF) | (((x) << 8) & 0xFF00) )
#define SWAP32(x)( (((x) >> 24) & 0x000000FF) | (((x) >>  8) & 0x0000FF00) | (((x) <<  8) & 0x00FF0000) | (((x) << 24) & 0xFF000000) ) 

extern unsigned int fsin1[2048], fsin2[2048], fsin3[2048];
extern int prticks;

extern unsigned short sky1[];
extern unsigned short sky2[];
short floorstuff[2*WIDTH*HEIGHT];

unsigned char dist_angle[2*WIDTH*HEIGHT];


extern unsigned char rbmp[SSIZE];
extern unsigned char gbmp[SSIZE];
extern unsigned char bbmp[SSIZE];

extern unsigned char rbuffer[SSIZE];
extern unsigned char gbuffer[SSIZE];
extern unsigned char bbuffer[SSIZE];

extern short radir0[SSIZE];
extern short radir1[SSIZE];
extern short radir2[SSIZE];

extern short radir3[SSIZE];
extern short radir4[SSIZE];
extern short radir5[SSIZE];


void Polarplasma(unsigned short *vram, unsigned short shade[])
{
	int x,y;
	unsigned char c;
	unsigned int c0;
    unsigned int *vram32 = (unsigned int*)vram;
    unsigned int k = prticks>>3;

    int i, j = 0;
    for (i=0; i<WIDTH*HEIGHT; i+=2)
    {
        c = dist_angle[j] + dist_angle[j+1] + k;
        c0 = shade[c];
        c = dist_angle[j+2] + dist_angle[j+3] + k;
        *vram32++ = (shade[c]<<16) | c0;
        j+=4;
    }
}



void Plasma(unsigned short *vram, unsigned short shade[])
{
    unsigned int *vram32 = (unsigned int*)vram;
    unsigned int k = (prticks>>4)%438;
    unsigned int x, y;
    for (y=0; y<240; y++)
        for (x=0; x<320; x+=8)
        {
            *vram32++ = shade[(fsin1[x] + fsin2[y] + fsin3[x+y+k]) & 255] | (shade[(fsin1[x+1] + fsin2[y] + fsin3[x+y+k+1]) & 255]<<16);
            *vram32++ = shade[(fsin1[x+2] + fsin2[y] + fsin3[x+y+k+2]) & 255] | (shade[(fsin1[x+3] + fsin2[y] + fsin3[x+y+k+3]) & 255]<<16);
            *vram32++ = shade[(fsin1[x+4] + fsin2[y] + fsin3[x+y+k+4]) & 255] | (shade[(fsin1[x+5] + fsin2[y] + fsin3[x+y+k+5]) & 255]<<16);
            *vram32++ = shade[(fsin1[x+6] + fsin2[y] + fsin3[x+y+k+6]) & 255] | (shade[(fsin1[x+7] + fsin2[y] + fsin3[x+y+k+7]) & 255]<<16);
        }
}

void DrawSky(unsigned short *vram)
{
    int x, y, xp, yp = 256*255;
    unsigned int *vram32 = (unsigned int*)vram;
    unsigned int c0;
    for (y=0; y<HEIGHT; y++)
    {
        for (x=0; x<WIDTH; x+=2)
        {
            xp = (x-(WIDTH>>1)) & 255;
            c0 = sky1[xp + yp];
            xp = (x+1-(WIDTH>>1)) & 255;
            *vram32++ = (sky1[xp + yp]<<16) | c0;
        }
        yp-=256;
    }
}

void Floor(unsigned short *vram, float ra, float zm)
{
	int x,y;
	int u,v;

    int ira = SDL_GetTicks()>>4;
    int izm = zm;

	unsigned int c,i;
    unsigned int *vram32 = (unsigned int*)vram;

 	i=0;
	for (y=0;y<HEIGHT - (HEIGHT>>2);y++)
	{
		for (x=0;x<WIDTH;x+=2)
		{
            u = floorstuff[i+1] & 127;
            v = (floorstuff[i] + ira) & 127;
			c = sky2[u + (v<<7)];

            u = floorstuff[i+3] & 127;
            v = (floorstuff[i+2] + ira) & 127;
			c = c | (sky2[u + (v<<7)]<<16);

            i+=4;
			*vram32++=c;
		}
	}
}

void Radial(unsigned short *avram, int fick)
{
	int x,y,i,ri,sum;
	int r,g,b;
    int k = prticks/12;

    int dcx = sin((double)k/73.0)*(WIDTH>>1);
    int dcy = fick * sin((double)k/112.0)*(HEIGHT>>1);
    int dci = dcx + dcy*WIDTH;
    unsigned short *vram;
    //Initialize the buffer or something
    unsigned int *rbuffer32 = (unsigned int*)rbuffer, *gbuffer32 = (unsigned int*)gbuffer, *bbuffer32 = (unsigned int*)bbuffer;
	for (i=0; i<SSIZE; i+=4)
	{
        *rbuffer32++ = rbmp[i] | (rbmp[i+1]<<8) | (rbmp[i+2]<<16) | (rbmp[i+3]<<24);
        *gbuffer32++ = gbmp[i] | (gbmp[i+1]<<8) | (gbmp[i+2]<<16) | (gbmp[i+3]<<24);
        *bbuffer32++ = bbmp[i] | (bbmp[i+1]<<8) | (bbmp[i+2]<<16) | (bbmp[i+3]<<24);
    }
/*
Quadrants:
Q2 | Q1
---|---    <= The Screen
Q3 | Q4

Q1 and Q4 are kinda broken rn.
*/
// Quadrant 1
 	vram=(unsigned short*)avram + WIDTH/2 + (HEIGHT/2-1)*WIDTH + dci;
	i=WIDTH/2 + (HEIGHT/2-1)*WIDTH + dci;
	ri=0;
    for (y=HEIGHT/2-1+dcy; y>=0; y--)
    {
        for (x=WIDTH/2+dcx; x<WIDTH; x++)
        {
            rbuffer[i] = (rbuffer[i] + rbuffer[i + radir3[ri]] + rbuffer[i + radir4[ri]] + rbuffer[i + radir5[ri]])>>2;
            gbuffer[i] = (gbuffer[i] + gbuffer[i + radir3[ri]] + gbuffer[i + radir4[ri]] + gbuffer[i + radir5[ri]])>>2;
            bbuffer[i] = (bbuffer[i] + bbuffer[i + radir3[ri]] + bbuffer[i + radir4[ri]] + bbuffer[i + radir5[ri]])>>2;
            //RGB565 0xFFFF
            *vram++ = ((rbuffer[i] >> 3) << 11) | ((gbuffer[i] >> 2) << 5) | (bbuffer[i] >> 3);
            i++;
            ri++;
        }
        i=i-3*(WIDTH/2)+dcx;
        ri+=WIDTH/2+dcx;
        vram=vram-3*(WIDTH/2)+dcx;
    }
// Quadrant 2
 	vram=(unsigned short*)avram + (WIDTH/2 - 1) + (HEIGHT/2 - 1)*WIDTH + dci;
	i=(WIDTH/2-1) + (HEIGHT/2-1)*WIDTH + dci;
	ri=0;
    for (y=HEIGHT/2-1+dcy; y>=0; y--)
    {
        for (x=WIDTH/2-1+dcx; x>=0; x--)
        {
            rbuffer[i] = (rbuffer[i] + rbuffer[i - radir0[ri]] + rbuffer[i - radir1[ri]] + rbuffer[i - radir2[ri]])>>2;
            gbuffer[i] = (gbuffer[i] + gbuffer[i - radir0[ri]] + gbuffer[i - radir1[ri]] + gbuffer[i - radir2[ri]])>>2;
            bbuffer[i] = (bbuffer[i] + bbuffer[i - radir0[ri]] + bbuffer[i - radir1[ri]] + bbuffer[i - radir2[ri]])>>2;
            //RGB565 0xFFFF
            //correct
            *vram-- = ((rbuffer[i] >> 3) << 11) | ((gbuffer[i] >> 2) << 5) | (bbuffer[i] >> 3);
            i--;
            ri++;
        }
        i=i-WIDTH/2+dcx;
        ri+=WIDTH/2-dcx;
        vram=vram-WIDTH/2+dcx;
    }
// Quadrant 3
 	vram=(unsigned short*)avram + (WIDTH/2 - 1) + (HEIGHT/2)*WIDTH + dci;
	i=(WIDTH/2-1) + (HEIGHT/2)*WIDTH + dci;
	ri=0;
    for (y=HEIGHT/2+dcy; y<HEIGHT; y++)
    {
        for (x=WIDTH/2-1+dcx; x>=0; x--)
        {
            rbuffer[i] = (rbuffer[i] + rbuffer[i - radir3[ri]] + rbuffer[i - radir4[ri]] + rbuffer[i - radir5[ri]])>>2;
            gbuffer[i] = (gbuffer[i] + gbuffer[i - radir3[ri]] + gbuffer[i - radir4[ri]] + gbuffer[i - radir5[ri]])>>2;
            bbuffer[i] = (bbuffer[i] + bbuffer[i - radir3[ri]] + bbuffer[i - radir4[ri]] + bbuffer[i - radir5[ri]])>>2;
            //RGB565 0xFFFF
            *vram-- = ((rbuffer[i] >> 3) << 11) | ((gbuffer[i] >> 2) << 5) | (bbuffer[i] >> 3);
            i--;
            ri++;
        }
        i+=3*(WIDTH/2)+dcx;
        ri+=WIDTH/2-dcx;
        vram+=3*(WIDTH/2)+dcx;
    }
// Quadrant 4
 	vram=(unsigned short*)avram + WIDTH/2 + (HEIGHT/2)*WIDTH + dci;
	i=WIDTH/2 + (HEIGHT/2)*WIDTH + dci;
	ri=0;
    for (y=HEIGHT/2+dcy; y<HEIGHT; y++)
    {
        for (x=WIDTH/2+dcx; x<WIDTH; x++)
        {
            rbuffer[i] = (rbuffer[i] + rbuffer[i + radir0[ri]] + rbuffer[i + radir1[ri]] + rbuffer[i + radir2[ri]])>>2;
            gbuffer[i] = (gbuffer[i] + gbuffer[i + radir0[ri]] + gbuffer[i + radir1[ri]] + gbuffer[i + radir2[ri]])>>2;
            bbuffer[i] = (bbuffer[i] + bbuffer[i + radir0[ri]] + bbuffer[i + radir1[ri]] + bbuffer[i + radir2[ri]])>>2;
            //RGB565 0xFFFF
            *vram++ = ((rbuffer[i] >> 3) << 11) | ((gbuffer[i] >> 2) << 5) | (bbuffer[i] >> 3);
            i++;
            ri++;
        }
        i+=WIDTH/2+dcx;
        ri+=WIDTH/2+dcx;
        vram+=WIDTH/2+dcx;
    }
}