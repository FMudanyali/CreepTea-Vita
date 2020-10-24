#include <stdio.h>
#include <math.h>
#include <psp2/kernel/clib.h>

#include "precalcs.h"
#include "engine3d.h"
#include "ZeDemo.h"

unsigned short shades[64][256];
int pdiv[4096];

int fsin1[2048], fsin2[2048], fsin3[2048];
int fsin4[2048], fsin5[2048], fsin6[2048];

extern unsigned char bitfonts[];
unsigned char fonts[59*64];

short floorstuff[2*WIDTH*HEIGHT];

unsigned char dist_angle[2*WIDTH*HEIGHT];

unsigned char rbmp[SSIZE];
unsigned char gbmp[SSIZE];
unsigned char bbmp[SSIZE];

unsigned char rbuffer[SSIZE];
unsigned char gbuffer[SSIZE];
unsigned char bbuffer[SSIZE];

short radir0[SSIZE];
short radir1[SSIZE];
short radir2[SSIZE];

short radir3[SSIZE];
short radir4[SSIZE];
short radir5[SSIZE];

extern unsigned short sky1[], sky3[];

void InitRadialBitmap1()
{
    int x, y, i = 0;
    unsigned short c;
    for (y=0; y<HEIGHT; y++)
    {
        for (x=0; x<WIDTH; x ++)
        {
            c = sky1[(x&255) + (((HEIGHT-1)-y)<<8)];
            rbmp[i] = ((c>>11) & 31) << 3;
            gbmp[i] = ((c>>5) & 63) << 2;
            bbmp[i] = (c & 31) << 3;
            i++;
        }
    }
}

void InitRadialBitmap2()
{
    int x, y, i = 0;
    unsigned short c;
    for (y=0; y<HEIGHT; y++)
    {
        for (x=0; x<WIDTH; x ++)
        {
            c = sky3[x + ((HEIGHT-1)-y)*320];
            rbmp[i] = ((c>>11) & 31) << 3;
            gbmp[i] = ((c>>5) & 63) << 2;
            bbmp[i] = (c & 31) << 3;
            i++;
        }
    }
}

void InitRadial()
{
    InitRadialBitmap1();

    float rb_shortness = 4.0f;

    int xi, yi;
    int i=0;
    int x, y;
    for (y=0; y<HEIGHT; y++)
    {
        for (x=0; x<WIDTH; x++)
        {
            xi = -(x/rb_shortness);
            yi = -(y/rb_shortness);
            radir0[i] = (xi>>1) + (yi>>1)*WIDTH;
            radir1[i] = (xi>>2) + (yi>>2)*WIDTH;
            radir2[i] = (xi>>3) + (yi>>3)*WIDTH;
            i++;
        }
    }

    i=0;
    for (y=0; y<HEIGHT; y++)
    {
        for (x=0; x<WIDTH; x++)
        {
            xi = -(x/rb_shortness);
            yi = y/rb_shortness;
            radir3[i] = (xi>>1) + (yi>>1)*WIDTH;
            radir4[i] = (xi>>2) + (yi>>2)*WIDTH;
            radir5[i] = (xi>>3) + (yi>>3)*WIDTH;
            i++;
        }
    }
}

void InitPolar()
{
	int x, y, c;

	float w=Twidth/2;
	int i=0;
	for (y=-HEIGHT/2; y<HEIGHT/2; y++)
	{
		for (x=-WIDTH/2; x<WIDTH/2; x++)
		{
			dist_angle[i++]=(int)((w*Twidth)*(1/sqrt(x*x+y*y)));
			dist_angle[i++]=((int)(2.0 * Twidth * atan2(y,x)/pi)) & 255;
		}
	}
}

void InitFloor()
{
    float w=128;
    int i=0, x, y;
    int w0=WIDTH/2;
    int h0=HEIGHT/2;
    
    int adj = -WIDTH>>2;

    for (y=-h0+adj;y<h0+adj;y++)
    {
    	for (x=-w0;x<w0;x++)
    	{
            floorstuff[i++]=(int)((w*128)*(1/sqrt(y*y))) & 127;
            floorstuff[i++]=(1/sqrt(y*y))* x*128;
        }
    }
}

void InitFonts()
{
	int x, y, i =0;
	int n, c;

	for (n=0; n<59; n++)
	{
		for (y=0; y<8; y++)
		{
			c = bitfonts[i++];
			for (x=0; x<8; x++)
			{
				fonts[(n << 6) + x + (y<<3)] = ((c >>  (7 - x)) & 1) * 255;
			}
		}
	}
}


void DrawFont(int xp, int yp, int ch, int bpp, unsigned short* vram)
{
    unsigned char *vram8;
    unsigned short *vram16;
    int cp = ch << 6;
    int x, y, yc, yi;

    switch(bpp)
    {
        case 8:

        vram8 = (unsigned char*)vram + xp + yp* WIDTH;
        for (y=0; y<8; y++)
        {
            yc = yp + y;
            if ((yc>=1) && (yc<HEIGHT - 1))
            {
                yi = y << 3;
                for (x=0; x<8; x++)
                {
                    *vram8++ |= fonts[cp + yi + x];
                }
                vram8-=8;
            }
            vram8+=WIDTH;
        }
        break;
        
        case 16:

        vram16 = (unsigned short*)vram + xp + yp * WIDTH;
        for (y=0; y<8; y++)
        {
            yc = yp + y;
            if ((yc>=1) && (yc<HEIGHT - 1))
            {
                yi = y << 3;
                for (x=0; x<8; x++)
                {
                    *vram16++ |= shades[3][fonts[cp + yi + x]];
                }
                vram16-=8;
            }
            vram16+=WIDTH;
        }
        break;
        
        default:
            break;
    }    
}


void DrawText_(int xtp, int ytp, int cn, char *text, int bpp, unsigned short *vram)
{
	int n;
	char c;

	for (n = 0; n<cn; n++)
	{
		c = *text++;
		if (c>31 && c<92) DrawFont(xtp, ytp, c - 32, bpp, vram);
			else if (c==0) n = cn;
		xtp+=8; if (xtp>WIDTH - 1) n = cn;
	}
}



void MakeColors(unsigned short cols[], ColorRGB c0, ColorRGB c1, int n0, int n1)
{
	float dr,dg,db;
	float cr,cg,cb;

	cr=c0.r; cg=c0.g; cb=c0.b;

	dr=((float)c1.r - (float)c0.r)/(float)(n1 - n0 + 1);
	dg=((float)c1.g - (float)c0.g)/(float)(n1 - n0 + 1);
	db=((float)c1.b - (float)c0.b)/(float)(n1 - n0 + 1);

    int i;
	for (i=n0; i<=n1; i++)
	{
		cr+=dr;	cg+=dg;	cb+=db;
		cols[i]= ((int)cr<<11) | ((int)cg<<5) | (int)cb;
	}
}

void MakeSines(int sines[], int n, float freq, float hght1, float hght2)
{
    int i;
	for (i=0; i<n; i++)
		sines[i]=sin(i/freq)*hght1+hght2;
}

void SetColors()
{
	ColorRGB c0[4] = {15,0,31, 31,31,0, 31,63,0, 31,63,31};
	ColorRGB c1[4] = {7,3,0, 15,7,0, 31,15,0, 31,63,31};
	ColorRGB c2[4] = {15,7,0, 31,15,7, 31,63,15, 31,63,31};
	ColorRGB conc[4] = {7,15,7, 15,31,15, 15,55,31, 31,63,31};

	ColorRGB plsm1[3] = {31,31,7, 7,0,15, 15,7,31};
	ColorRGB plsm2[3] = {0,0,0, 7,31,7, 0,0,0};
	ColorRGB plsm3[3] = {15,31,0, 15,63,0, 31,63,15};
	ColorRGB plsm4[3] = {15,0,0, 31,63,15, 15,31,0};
	ColorRGB plsm5[3] = {31,0,0, 15,0,31, 31,63,31};
	ColorRGB plsm6[3] = {0,0,0, 31,63,31, 0,0,0};

	ColorRGB backmask[4] = {15,31,15, 15,0,15, 31,0,15, 31,63,15};
	ColorRGB polar[4] = {0,0,15, 7,31,7, 31,31,0, 31,63,31};
	ColorRGB plasma[4] = {15,0,31, 15,31,31, 31,63,31, 15,63,31};

    ColorRGB blobcol[3] = {0,0,0, 15,15,31, 23,63,31};
    MakeColors (shades[32], blobcol[0], blobcol[1], 0, 127);
    MakeColors (shades[32], blobcol[1], blobcol[2], 128, 255);


	MakeColors(shades[0], c0[0], c0[1], 0, 159);
	MakeColors(shades[0], c0[1], c0[2], 160, 191);
	MakeColors(shades[0], c0[2], c0[3], 192, 255);

	MakeColors(shades[1], c1[0], c1[1], 0, 127);
	MakeColors(shades[1], c1[1], c1[2], 128, 191);
	MakeColors(shades[1], c1[2], c1[3], 192, 255);

	MakeColors(shades[2], c2[0], c2[1], 0, 63);
	MakeColors(shades[2], c2[1], c2[2], 64, 191);
	MakeColors(shades[2], c2[2], c2[3], 192, 255);

	MakeColors(shades[3], conc[0], conc[1], 0, 63);
	MakeColors(shades[3], conc[1], conc[2], 64, 191);
	MakeColors(shades[3], conc[2], conc[3], 192, 255);

	MakeColors(shades[4], plsm1[0], plsm1[1], 0, 63);
	MakeColors(shades[4], plsm1[1], plsm1[2], 64, 127);
	MakeColors(shades[4], plsm1[2], plsm1[1], 128, 191);
	MakeColors(shades[4], plsm1[1], plsm1[0], 192, 255);

	MakeColors(shades[5], plsm2[0], plsm2[1], 0, 63);
	MakeColors(shades[5], plsm2[1], plsm2[2], 64, 127);
	MakeColors(shades[5], plsm2[2], plsm2[1], 128, 191);
	MakeColors(shades[5], plsm2[1], plsm2[0], 192, 255);

	MakeColors(shades[6], plsm3[0], plsm3[1], 0, 63);
	MakeColors(shades[6], plsm3[1], plsm3[2], 64, 127);
	MakeColors(shades[6], plsm3[2], plsm3[1], 128, 191);
	MakeColors(shades[6], plsm3[1], plsm3[0], 192, 255);

	MakeColors(shades[7], plsm4[0], plsm4[1], 0, 63);
	MakeColors(shades[7], plsm4[1], plsm4[2], 64, 127);
	MakeColors(shades[7], plsm4[2], plsm4[1], 128, 191);
	MakeColors(shades[7], plsm4[1], plsm4[0], 192, 255);

	MakeColors(shades[8], plsm5[0], plsm5[1], 0, 63);
	MakeColors(shades[8], plsm5[1], plsm5[2], 64, 127);
	MakeColors(shades[8], plsm5[2], plsm5[1], 128, 191);
	MakeColors(shades[8], plsm5[1], plsm5[0], 192, 255);

	MakeColors(shades[9], plsm6[0], plsm6[1], 0, 63);
	MakeColors(shades[9], plsm6[1], plsm6[2], 64, 127);
	MakeColors(shades[9], plsm6[2], plsm6[1], 128, 191);
	MakeColors(shades[9], plsm6[1], plsm6[0], 192, 255);

	MakeColors(shades[10], backmask[0], backmask[1], 0, 127);
	MakeColors(shades[10], backmask[1], backmask[2], 128, 191);
	MakeColors(shades[10], backmask[2], backmask[3], 192, 255);

	MakeColors(shades[11], polar[0], polar[1], 0, 127);
	MakeColors(shades[11], polar[1], polar[2], 128, 191);
	MakeColors(shades[11], polar[2], polar[3], 192, 255);

	MakeColors(shades[12], plasma[0], plasma[1], 128, 159);
	MakeColors(shades[12], plasma[1], plasma[2], 160, 191);
	MakeColors(shades[12], plasma[2], plasma[3], 192, 223);
	MakeColors(shades[12], plasma[3], plasma[0], 224, 255);
}

void PrecDivs()
{
    int fp = 16;
    int i;
    for (i=0; i<4096; i++)
    {
        if ((i-2048)!=0)
            pdiv[i] = (1<<fp)/(i-2048);
        else
            pdiv[i] = (1<<fp);
    }
}

void SinePrecs()
{
    float d = 2.6;
	MakeSines(fsin1, 2048, 30, 96/d, 96/d);
	MakeSines(fsin2, 2048, 40, 112/d, 112/d);
	MakeSines(fsin3, 2048, 70, 128/d, 128/d);

	MakeSines(fsin4, 2048, 16, 8, 0);
	MakeSines(fsin5, 2048, 8, 16, 0);
	MakeSines(fsin6, 2048, 12, 32, 0);
}

void EffectsInit()
{
    SinePrecs();
    PrecDivs();
    InitFloor();
    InitPolar();
    InitRadial();
}


void precalcs()
{
    InitFonts();
    SetColors();
    EffectsInit();
    Init3d();
}
