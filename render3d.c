#include <string.h>
#include <stdlib.h>

#include "engine3d.h"
#include "render3d.h"
#include "ZeDemo.h"

unsigned char ball_c[256];
unsigned int ball_p[256];

extern unsigned short swp[MAXDATA];
int zdata[MAXDATA];


extern point3d fpts[MAXDATA];
extern point3d norms[MAXDATA];
extern point3d pt_norms[MAXDATA];
extern point2d spts[MAXDATA];
extern point3d spls[MAXDATA];
tcord point_tc[MAXDATA];

unsigned int RenderMode;

unsigned int zbuffer[SSIZE];

extern unsigned short shades[64][256];
extern int pdiv[4096];

extern unsigned short env1[];
extern unsigned short env2[];
extern unsigned short env3[];

extern int prticks;

static inline void drawpoint(point2d point, unsigned short *vram)
{
    if (point.x>=0 && point.x<WIDTH && point.y>=0 && point.y<HEIGHT)
		*(vram + point.x + point.y * WIDTH) = 0xFFFF;
}


static inline void drawball(point2d point, unsigned short *vram)
{
	unsigned short *vram0 = vram + (point.x - 8) + (point.y - 8) * WIDTH;
	int y, i, c;

	i = 0;
	for (y=16; y!=0; y--)
	{
		c = ball_c[i]; if (c!=0) *(vram0 + 0) = ball_p[c + point.c];
		c = ball_c[i+1]; if (c!=0) *(vram0 + 1) = ball_p[c + point.c];
		c = ball_c[i+2]; if (c!=0) *(vram0 + 2) = ball_p[c + point.c];
		c = ball_c[i+3]; if (c!=0) *(vram0 + 3) = ball_p[c + point.c];
		c = ball_c[i+4]; if (c!=0) *(vram0 + 4) = ball_p[c + point.c];
		c = ball_c[i+5]; if (c!=0) *(vram0 + 5) = ball_p[c + point.c];
		c = ball_c[i+6]; if (c!=0) *(vram0 + 6) = ball_p[c + point.c];
		c = ball_c[i+7]; if (c!=0) *(vram0 + 7) = ball_p[c + point.c];
		c = ball_c[i+8]; if (c!=0) *(vram0 + 8) = ball_p[c + point.c];
		c = ball_c[i+9]; if (c!=0) *(vram0 + 9) = ball_p[c + point.c];
		c = ball_c[i+10]; if (c!=0) *(vram0 + 10) = ball_p[c + point.c];
		c = ball_c[i+11]; if (c!=0) *(vram0 + 11) = ball_p[c + point.c];
		c = ball_c[i+12]; if (c!=0) *(vram0 + 12) = ball_p[c + point.c];
		c = ball_c[i+13]; if (c!=0) *(vram0 + 13) = ball_p[c + point.c];
		c = ball_c[i+14]; if (c!=0) *(vram0 + 14) = ball_p[c + point.c];
		c = ball_c[i+15]; if (c!=0) *(vram0 + 15) = ball_p[c + point.c];
		i+=16;
		vram0+=WIDTH;
	}
}


static inline void drawline (line2d line, unsigned short *vram)
{

	int x1 = spts[line.p0].x;
	int y1 = spts[line.p0].y;
	int x2 = spts[line.p1].x;
	int y2 = spts[line.p1].y;
	int c = line.c;

	int dx, dy, n, l;
	int x00, y00;
	int fp = 12;
	int vramofs;

	int x, y;

	dx = x2 - x1;
	dy = y2 - y1;

	if (abs(dy) < abs(dx))
	{
		if (x1>x2)
		{
			n = x1; x1 = x2; x2 = n;
			n = y1; y1 = y2; y2 = n;
		}

        if (dx!=0) l = ((dy<<fp)*pdiv[dx+2048])>>16;
        y00 = y1<<fp;
		for (x=x1; x<x2; x++)
		{
			vramofs = ((y00 += l)>>fp)*WIDTH + x;
            if (vramofs>=0 && vramofs<SSIZE) *(vram + vramofs) = c;
		}
	}
	else
	{

		if (y1>y2)
		{
			n = y1; y1 = y2; y2 = n;
			n = x1; x1 = x2; x2 = n;
		}

        if (dy!=0) l = ((dx<<fp)*pdiv[dy+2048])>>16;
        x00 = x1<<fp;

		for (y=y1; y<y2; y++)
		{
			vramofs = y*WIDTH + ((x00 += l)>>fp);
            if (vramofs>=0 && vramofs<SSIZE)  * (vram + vramofs) = c;
		}

	}
}


static inline void DrawFlatTriangle (poly2d poly, unsigned short *vram, unsigned short shade[])
{
int x0 =spts[poly.p0].x; int y0 =spts[poly.p0].y;
int x1 =spts[poly.p1].x; int y1 =spts[poly.p1].y;
int x2 =spts[poly.p2].x; int y2 =spts[poly.p2].y;
int c = shade[poly.c];

// ===== Sort =====

int temp;
if (y1<y0)
{
    temp = x0; x0 = x1; x1 = temp;
    temp = y0; y0 = y1; y1 = temp;
}
if (y2<y0)
{
    temp = x0; x0 = x2; x2 = temp;
    temp = y0; y0 = y2; y2 = temp;
}
if (y2<y1)
{
    temp = x1; x1 = x2; x2 = temp;
    temp = y1; y1 = y2; y2 = temp;
}

// ===== Interpolation variables =====

int n, fp = 8;
int lx01=0, lx12=0, lx02=0;

int dx01 = x1 - x0;
int dy01 = y1 - y0;

    if (dy01!=0)
        lx01 = ((dx01<<fp)*pdiv[dy01+2048])>>16;

int dx12 = x2 - x1;
int dy12 = y2 - y1;

    if (dy12!=0)
        lx12 = ((dx12<<fp)*pdiv[dy12+2048])>>16;

int dx02 = x2 - x0;
int dy02 = y2 - y0;

    if (dy02!=0)
        lx02 = ((dx02<<fp)*pdiv[dy02+2048])>>16;

int vramofs;
int x, y;

int x01 = x0<<fp;
int x02 = x01;

int sx1, sx2;

    int yp = y0 * WIDTH;
    for (y = y0; y<y1; y++)
    {
        sx1 = x01>>fp;
        sx2 = x02>>fp;

        if (sx1>sx2)
        {
            temp = sx1; sx1 = sx2; sx2 = temp;
        }

        yp+=WIDTH;
        vramofs = yp + sx1;
        for (x = sx1; x<sx2; x++)
        {
            if (vramofs>=0 && vramofs<SSIZE && x>=0 && x<WIDTH)
                *(vram+vramofs)=c;
            vramofs++;
        }
        x01+=lx01;
        x02+=lx02;
    }

    x01 = x1<<fp;

    yp = y1 * WIDTH;
    for (y = y1; y<y2; y++)
    {
        sx1 = x01>>fp;
        sx2 = x02>>fp;

        if (sx1>sx2)
        {
            temp = sx1; sx1 = sx2; sx2 = temp;
        }

        yp+=WIDTH;
        vramofs = yp + sx1;
        for (x = sx1; x<sx2; x++)
        {
            if (vramofs>=0 && vramofs<SSIZE && x>=0 && x<WIDTH)
                *(vram+vramofs)=c;
            vramofs++;
        }
        x01+=lx12;
        x02+=lx02;
    }
}


static inline void DrawFlatTriangleZB (poly2d poly, unsigned short *vram, unsigned short shade[])
{
int x0 =spts[poly.p0].x; int y0 =spts[poly.p0].y;
int x1 =spts[poly.p1].x; int y1 =spts[poly.p1].y;
int x2 =spts[poly.p2].x; int y2 =spts[poly.p2].y;
int c = shade[poly.c];

int z0 = fpts[poly.p0].z;
int z1 = fpts[poly.p1].z;
int z2 = fpts[poly.p2].z;

// ===== Sort =====

int temp;
if (y1<y0)
{
    temp = x0; x0 = x1; x1 = temp;
    temp = y0; y0 = y1; y1 = temp;
    temp = z0; z0 = z1; z1 = temp;
}
if (y2<y0)
{
    temp = x0; x0 = x2; x2 = temp;
    temp = y0; y0 = y2; y2 = temp;
    temp = z0; z0 = z2; z2 = temp;
}
if (y2<y1)
{
    temp = x1; x1 = x2; x2 = temp;
    temp = y1; y1 = y2; y2 = temp;
    temp = z1; z1 = z2; z2 = temp;
}

// ===== Interpolation variables =====

int n, fp = 8;
int lx01=0, lx12=0, lx02=0;
int lz01=0, lz12=0, lz02=0;


int dx01 = x1 - x0;
int dy01 = y1 - y0;
int dz01 = z1 - z0;

    if (dy01!=0)
    {
        lx01 = ((dx01<<fp)*pdiv[dy01+2048])>>16;
        lz01 = (dz01*pdiv[dy01+2048])>>16;
    }

int dx12 = x2 - x1;
int dy12 = y2 - y1;
int dz12 = z2 - z1;

    if (dy12!=0)
    {
        lx12 = ((dx12<<fp)*pdiv[dy12+2048])>>16;
        lz12 = (dz12*pdiv[dy12+2048])>>16;
    }

int dx02 = x2 - x0;
int dy02 = y2 - y0;
int dz02 = z2 - z0;

    if (dy02!=0)
    {
        lx02 = ((dx02<<fp)*pdiv[dy02+2048])>>16;
        lz02 = (dz02*pdiv[dy02+2048])>>16;
    }

int vramofs;
int x, y;

int x01 = x0<<fp;
int x02 = x01;
int z01 = z0;
int z02 = z01;

int dz;
int sx1, sx2;
int sz1, sz2;

    int yp = y0 * WIDTH;
    for (y = y0; y<y1; y++)
    {
        sx1 = x01>>fp;
        sx2 = x02>>fp;
        sz1 = z01;
        sz2 = z02;

        if (sx1>sx2)
        {
            temp = sx1; sx1 = sx2; sx2 = temp;
            temp = sz1; sz1 = sz2; sz2 = temp;
        }
        if (sx2!=sx1) dz = ((sz2 - sz1)*pdiv[(sx2-sx1)+2048])>>16;

        yp+=WIDTH;
        vramofs = yp + sx1;
        for (x = sx1; x<sx2; x++)
        {
            sz1+=dz;
            if ((vramofs>=0 && vramofs<SSIZE) && sz1<zbuffer[vramofs])
            {
                zbuffer[vramofs] = sz1;
                *(vram+vramofs)=c;
            }


            vramofs++;
        }
        x01+=lx01;
        x02+=lx02;
        z01+=lz01;
        z02+=lz02;
    }

    x01 = x1<<fp;
    z01 = z1;

    yp = y1 * WIDTH;
    for (y = y1; y<y2; y++)
    {
        sx1 = x01>>fp;
        sx2 = x02>>fp;
        sz1 = z01;
        sz2 = z02;

        if (sx1>sx2)
        {
            temp = sx1; sx1 = sx2; sx2 = temp;
            temp = sz1; sz1 = sz2; sz2 = temp;
        }

        if (sx2!=sx1) dz = ((sz2 - sz1)*pdiv[(sx2-sx1)+2048])>>16;

        yp+=WIDTH;
        vramofs = yp + sx1;
        for (x = sx1; x<sx2; x++)
        {
            sz1+=dz;
            if ((vramofs>=0 && vramofs<SSIZE) && sz1<zbuffer[vramofs])
            {
                zbuffer[vramofs] = sz1;
                *(vram+vramofs)=c;
            }
            vramofs++;
        }
        x01+=lx12;
        x02+=lx02;
        z01+=lz12;
        z02+=lz02;
    }
}

static inline void DrawGouraudTriangle (poly2d poly, unsigned short *vram, unsigned short shade[])
{
int x0 =spts[poly.p0].x; int y0 =spts[poly.p0].y;
int x1 =spts[poly.p1].x; int y1 =spts[poly.p1].y;
int x2 =spts[poly.p2].x; int y2 =spts[poly.p2].y;

int c;
int c0 = spts[poly.p0].c;
int c1 = spts[poly.p1].c;
int c2 = spts[poly.p2].c;

// ===== Sort =====

int temp;
if (y1<y0)
{
    temp = c0; c0 = c1; c1 = temp;
    temp = x0; x0 = x1; x1 = temp;
    temp = y0; y0 = y1; y1 = temp;
}
if (y2<y0)
{
    temp = c0; c0 = c2; c2 = temp;
    temp = x0; x0 = x2; x2 = temp;
    temp = y0; y0 = y2; y2 = temp;
}
if (y2<y1)
{
    temp = c1; c1 = c2; c2 = temp;
    temp = x1; x1 = x2; x2 = temp;
    temp = y1; y1 = y2; y2 = temp;
}

// ===== Interpolation variables =====

int n;
int fp = 8;
int lx01=0, lx12=0, lx02=0;
int lc01=0, lc12=0, lc02=0;

int dx01 = x1 - x0;
int dy01 = y1 - y0;
int dc01 = c1 - c0;

    if (dy01!=0)
    {
        lx01 = ((dx01<<fp)*pdiv[dy01+2048])>>16;
        lc01 = ((dc01<<fp)*pdiv[dy01+2048])>>16;
    }

int dx12 = x2 - x1;
int dy12 = y2 - y1;
int dc12 = c2 - c1;

    if (dy12!=0)
    {
        lx12 = ((dx12<<fp)*pdiv[dy12+2048])>>16;
        lc12 = ((dc12<<fp)*pdiv[dy12+2048])>>16;
    }

int dx02 = x2 - x0;
int dy02 = y2 - y0;
int dc02 = c2 - c0;

    if (dy02!=0)
    {
        lx02 = ((dx02<<fp)*pdiv[dy02+2048])>>16;
        lc02 = ((dc02<<fp)*pdiv[dy02+2048])>>16;
    }

int vramofs;
int x, y;

int x01 = x0<<fp;
int x02 = x01;
int c01 = c0<<fp;
int c02 = c01;

int dc;
int sc1, sc2;
int sx1, sx2;

    int yp = y0 * WIDTH;
    for (y = y0; y<y1; y++)
    {
        sx1 = x01>>fp;
        sx2 = x02>>fp;
        sc1 = c01;
        sc2 = c02;

        if (sx1>sx2)
        {
            temp = sx1; sx1 = sx2; sx2 = temp;
            temp = sc1; sc1 = sc2; sc2 = temp;
        }

        if (sx2!=sx1)
            dc = ((sc2 - sc1)*pdiv[(sx2-sx1)+2048])>>16;

        yp+=WIDTH;
        vramofs = yp + sx1;
        for (x = sx1; x<sx2; x++)
        {
            sc1+=dc;
            if (vramofs>=0 && vramofs<SSIZE && x>=0 && x<WIDTH)
            {
                c = sc1>>fp;
                if (c<0) c = 0;
                *(vram+vramofs)=shade[c];
            }
            vramofs++;
        }
        x01+=lx01;
        x02+=lx02;
        c01+=lc01;
        c02+=lc02;
    }

    x01 = x1<<fp;
    c01 = c1<<fp;

    yp = y1 * WIDTH;
    for (y = y1; y<y2; y++)
    {
        sx1 = x01>>fp;
        sx2 = x02>>fp;
        sc1 = c01;
        sc2 = c02;

        if (sx1>sx2)
        {
            temp = sx1; sx1 = sx2; sx2 = temp;
            temp = sc1; sc1 = sc2; sc2 = temp;
        }

        if (sx2!=sx1)
            dc = ((sc2 - sc1)*pdiv[(sx2-sx1)+2048])>>16;

        yp+=WIDTH;
        vramofs = yp + sx1;
        for (x = sx1; x<sx2; x++)
        {
            sc1+=dc;
            if (vramofs>=0 && vramofs<SSIZE && x>=0 && x<WIDTH)
            {
                c = sc1>>fp;
                if (c<0) c = 0;
                *(vram+vramofs)=shade[c];
            }
            vramofs++;
        }
        x01+=lx12;
        x02+=lx02;
        c01+=lc12;
        c02+=lc02;
    }
}

static inline void DrawGouraudTriangleZB (poly2d poly, unsigned short *vram, unsigned short shade[])
{
int x0 =spts[poly.p0].x; int y0 =spts[poly.p0].y;
int x1 =spts[poly.p1].x; int y1 =spts[poly.p1].y;
int x2 =spts[poly.p2].x; int y2 =spts[poly.p2].y;

int c;
int c0 = spts[poly.p0].c;
int c1 = spts[poly.p1].c;
int c2 = spts[poly.p2].c;

int z0 = fpts[poly.p0].z;
int z1 = fpts[poly.p1].z;
int z2 = fpts[poly.p2].z;

// ===== Sort =====

int temp;
if (y1<y0)
{
    temp = c0; c0 = c1; c1 = temp;
    temp = z0; z0 = z1; z1 = temp;
    temp = x0; x0 = x1; x1 = temp;
    temp = y0; y0 = y1; y1 = temp;
}
if (y2<y0)
{
    temp = c0; c0 = c2; c2 = temp;
    temp = z0; z0 = z2; z2 = temp;
    temp = x0; x0 = x2; x2 = temp;
    temp = y0; y0 = y2; y2 = temp;
}
if (y2<y1)
{
    temp = c1; c1 = c2; c2 = temp;
    temp = z1; z1 = z2; z2 = temp;
    temp = x1; x1 = x2; x2 = temp;
    temp = y1; y1 = y2; y2 = temp;
}

// ===== Interpolation variables =====

int n;
int fp = 8;
int lx01=0, lx12=0, lx02=0;
int lc01=0, lc12=0, lc02=0;
int lz01=0, lz12=0, lz02=0;

int dx01 = x1 - x0;
int dy01 = y1 - y0;
int dc01 = c1 - c0;
int dz01 = z1 - z0;

    if (dy01!=0)
    {
        lx01 = ((dx01<<fp)*pdiv[dy01+2048])>>16;
        lc01 = ((dc01<<fp)*pdiv[dy01+2048])>>16;
        lz01 = (dz01*pdiv[dy01+2048])>>16;
    }

int dx12 = x2 - x1;
int dy12 = y2 - y1;
int dc12 = c2 - c1;
int dz12 = z2 - z1;

    if (dy12!=0)
    {
        lx12 = ((dx12<<fp)*pdiv[dy12+2048])>>16;
        lc12 = ((dc12<<fp)*pdiv[dy12+2048])>>16;
        lz12 = (dz12*pdiv[dy12+2048])>>16;
    }

int dx02 = x2 - x0;
int dy02 = y2 - y0;
int dc02 = c2 - c0;
int dz02 = z2 - z0;

    if (dy02!=0)
    {
        lx02 = ((dx02<<fp)*pdiv[dy02+2048])>>16;
        lc02 = ((dc02<<fp)*pdiv[dy02+2048])>>16;
        lz02 = (dz02*pdiv[dy02+2048])>>16;
    }

int vramofs;
int x, y;

int x01 = x0<<fp;
int x02 = x01;
int c01 = c0<<fp;
int c02 = c01;
int z01 = z0;
int z02 = z01;

int dc, dz;
int sc1, sc2;
int sx1, sx2;
int sz1, sz2;

    int yp = y0 * WIDTH;
    for (y = y0; y<y1; y++)
    {
        sx1 = x01>>fp;
        sx2 = x02>>fp;
        sc1 = c01;
        sc2 = c02;
        sz1 = z01;
        sz2 = z02;

        if (sx1>sx2)
        {
            temp = sx1; sx1 = sx2; sx2 = temp;
            temp = sc1; sc1 = sc2; sc2 = temp;
            temp = sz1; sz1 = sz2; sz2 = temp;
        }

        if (sx2!=sx1)
        {
            dc = ((sc2 - sc1)*pdiv[(sx2-sx1)+2048])>>16;
            dz = ((sz2 - sz1)*pdiv[(sx2-sx1)+2048])>>16;
        }

        yp+=WIDTH;
        vramofs = yp + sx1;
        for (x = sx1; x<sx2; x++)
        {
            sc1+=dc;
            sz1+=dz;
            if ((vramofs>=0 && vramofs<SSIZE && x>=0 && x<WIDTH) && sz1<zbuffer[vramofs])
            {
                c = sc1>>fp;
                if (c<0) c=0;
                zbuffer[vramofs] = sz1;
                *(vram+vramofs)=shade[c];
            }
            vramofs++;
        }
        x01+=lx01;
        x02+=lx02;
        c01+=lc01;
        c02+=lc02;
        z01+=lz01;
        z02+=lz02;
    }

    x01 = x1<<fp;
    c01 = c1<<fp;
    z01 = z1;

    yp = y1 * WIDTH;
    for (y = y1; y<y2; y++)
    {
        sx1 = x01>>fp;
        sx2 = x02>>fp;
        sc1 = c01;
        sc2 = c02;
        sz1 = z01;
        sz2 = z02;

        if (sx1>sx2)
        {
            temp = sx1; sx1 = sx2; sx2 = temp;
            temp = sc1; sc1 = sc2; sc2 = temp;
            temp = sz1; sz1 = sz2; sz2 = temp;
        }

        if (sx2!=sx1)
        {
            dc = ((sc2 - sc1)*pdiv[(sx2-sx1)+2048])>>16;
            dz = ((sz2 - sz1)*pdiv[(sx2-sx1)+2048])>>16;
        }

        yp+=WIDTH;
        vramofs = yp + sx1;
        for (x = sx1; x<sx2; x++)
        {
            sc1+=dc;
            sz1+=dz;
            if ((vramofs>=0 && vramofs<SSIZE && x>=0 && x<WIDTH) && sz1<zbuffer[vramofs])
            {
                c = sc1>>fp;
                if (c<0) c=0;
                zbuffer[vramofs] = sz1;
                *(vram+vramofs)=shade[c];
            }
            vramofs++;
        }
        x01+=lx12;
        x02+=lx02;
        c01+=lc12;
        c02+=lc02;
        z01+=lz12;
        z02+=lz02;
    }
}

static inline void DrawTextureTriangle(poly2d poly, unsigned short *vram, int tshr, unsigned short texture[])
{
int x0 =spts[poly.p0].x; int y0 =spts[poly.p0].y;
int x1 =spts[poly.p1].x; int y1 =spts[poly.p1].y;
int x2 =spts[poly.p2].x; int y2 =spts[poly.p2].y;

int c;

int u0 = poly.tc0.u; int v0 = poly.tc0.v;
int u1 = poly.tc1.u; int v1 = poly.tc1.v;
int u2 = poly.tc2.u; int v2 = poly.tc2.v;

// ===== Sort =====

int temp;
if (y1<y0)
{
    temp = u0; u0 = u1; u1 = temp;
    temp = v0; v0 = v1; v1 = temp;
    temp = x0; x0 = x1; x1 = temp;
    temp = y0; y0 = y1; y1 = temp;
}
if (y2<y0)
{
    temp = u0; u0 = u2; u2 = temp;
    temp = v0; v0 = v2; v2 = temp;
    temp = x0; x0 = x2; x2 = temp;
    temp = y0; y0 = y2; y2 = temp;
}
if (y2<y1)
{
    temp = u1; u1 = u2; u2 = temp;
    temp = v1; v1 = v2; v2 = temp;
    temp = x1; x1 = x2; x2 = temp;
    temp = y1; y1 = y2; y2 = temp;
}

// ===== Interpolation variables =====

int n;
int fp = 8;
int lx01=0, lx12=0, lx02=0;
int lu01=0, lu12=0, lu02=0;
int lv01=0, lv12=0, lv02=0;

int dx01 = x1 - x0;
int dy01 = y1 - y0;
int du01 = u1 - u0;
int dv01 = v1 - v0;

    if (dy01!=0)
    {
        lx01 = ((dx01<<fp)*pdiv[dy01+2048])>>16;
        lu01 = ((du01<<fp)*pdiv[dy01+2048])>>16;
        lv01 = ((dv01<<fp)*pdiv[dy01+2048])>>16;
    }

int dx12 = x2 - x1;
int dy12 = y2 - y1;
int du12 = u2 - u1;
int dv12 = v2 - v1;

    if (dy12!=0)
    {
        lx12 = ((dx12<<fp)*pdiv[dy12+2048])>>16;
        lu12 = ((du12<<fp)*pdiv[dy12+2048])>>16;
        lv12 = ((dv12<<fp)*pdiv[dy12+2048])>>16;
    }

int dx02 = x2 - x0;
int dy02 = y2 - y0;
int du02 = u2 - u0;
int dv02 = v2 - v0;

    if (dy02!=0)
    {
        lx02 = ((dx02<<fp)*pdiv[dy02+2048])>>16;
        lu02 = ((du02<<fp)*pdiv[dy02+2048])>>16;
        lv02 = ((dv02<<fp)*pdiv[dy02+2048])>>16;
    }

int vramofs;
int x, y;

int x01 = x0<<fp;
int x02 = x01;
int u01 = u0<<fp;
int u02 = u01;
int v01 = v0<<fp;
int v02 = v01;

int du, dv;
int su1, su2;
int sv1, sv2;
int sx1, sx2;

int px, py;

    int yp = y0 * WIDTH;
    for (y = y0; y<y1; y++)
    {
        sx1 = x01>>fp;
        sx2 = x02>>fp;
        su1 = u01;
        su2 = u02;
        sv1 = v01;
        sv2 = v02;

        if (sx1>sx2)
        {
            temp = sx1; sx1 = sx2; sx2 = temp;
            temp = su1; su1 = su2; su2 = temp;
            temp = sv1; sv1 = sv2; sv2 = temp;
        }

        if (sx2!=sx1)
        {
            du = ((su2 - su1)*pdiv[(sx2-sx1)+2048])>>16;
            dv = ((sv2 - sv1)*pdiv[(sx2-sx1)+2048])>>16;
        }

        yp+=WIDTH;
        vramofs = yp + sx1;
        for (x = sx1; x<sx2; x++)
        {
            su1+=du;
            sv1+=dv;
            if (vramofs>=0 && vramofs<SSIZE)
            {
                c = texture[((su1>>fp)>>tshr) + (((sv1>>fp)>>tshr)<<(8-tshr))];
                *(vram+vramofs) = c;
            }
            vramofs++;
        }
        x01+=lx01;
        x02+=lx02;
        u01+=lu01;
        u02+=lu02;
        v01+=lv01;
        v02+=lv02;
    }

    x01 = x1<<fp;
    u01 = u1<<fp;
    v01 = v1<<fp;

    yp = y1 * WIDTH;
    for (y = y1; y<y2; y++)
    {
        sx1 = x01>>fp;
        sx2 = x02>>fp;
        su1 = u01;
        su2 = u02;
        sv1 = v01;
        sv2 = v02;

        if (sx1>sx2)
        {
            temp = sx1; sx1 = sx2; sx2 = temp;
            temp = su1; su1 = su2; su2 = temp;
            temp = sv1; sv1 = sv2; sv2 = temp;
        }

        if (sx2!=sx1)
        {
            du = ((su2 - su1)*pdiv[(sx2-sx1)+2048])>>16;
            dv = ((sv2 - sv1)*pdiv[(sx2-sx1)+2048])>>16;
        }

        yp+=WIDTH;
        vramofs = yp + sx1;
        for (x = sx1; x<sx2; x++)
        {
            su1+=du;
            sv1+=dv;
            if (vramofs>=0 && vramofs<SSIZE)
            {
                c = texture[((su1>>fp)>>tshr) + (((sv1>>fp)>>tshr)<<(8-tshr))];
                *(vram+vramofs) = c;
            }
            vramofs++;
        }
        x01+=lx12;
        x02+=lx02;
        u01+=lu12;
        u02+=lu02;
        v01+=lv12;
        v02+=lv02;
    }
}

static inline void DrawTextureTriangleZB(poly2d poly, unsigned short *vram, int tshr, unsigned short texture[])
{
int x0 =spts[poly.p0].x; int y0 =spts[poly.p0].y;
int x1 =spts[poly.p1].x; int y1 =spts[poly.p1].y;
int x2 =spts[poly.p2].x; int y2 =spts[poly.p2].y;

int c;

int u0 = poly.tc0.u; int v0 = poly.tc0.v;
int u1 = poly.tc1.u; int v1 = poly.tc1.v;
int u2 = poly.tc2.u; int v2 = poly.tc2.v;

int z0 = fpts[poly.p0].z;
int z1 = fpts[poly.p1].z;
int z2 = fpts[poly.p2].z;

// ===== Sort =====

int temp;
if (y1<y0)
{
    temp = u0; u0 = u1; u1 = temp;
    temp = v0; v0 = v1; v1 = temp;
    temp = z0; z0 = z1; z1 = temp;
    temp = x0; x0 = x1; x1 = temp;
    temp = y0; y0 = y1; y1 = temp;
}
if (y2<y0)
{
    temp = u0; u0 = u2; u2 = temp;
    temp = v0; v0 = v2; v2 = temp;
    temp = z0; z0 = z2; z2 = temp;
    temp = x0; x0 = x2; x2 = temp;
    temp = y0; y0 = y2; y2 = temp;
}
if (y2<y1)
{
    temp = u1; u1 = u2; u2 = temp;
    temp = v1; v1 = v2; v2 = temp;
    temp = z1; z1 = z2; z2 = temp;
    temp = x1; x1 = x2; x2 = temp;
    temp = y1; y1 = y2; y2 = temp;
}

// ===== Interpolation variables =====

int n;
int fp = 8;
int lx01=0, lx12=0, lx02=0;
int lu01=0, lu12=0, lu02=0;
int lv01=0, lv12=0, lv02=0;
int lz01=0, lz12=0, lz02=0;

int dx01 = x1 - x0;
int dy01 = y1 - y0;
int du01 = u1 - u0;
int dv01 = v1 - v0;
int dz01 = z1 - z0;

    if (dy01!=0)
    {
        lx01 = ((dx01<<fp)*pdiv[dy01+2048])>>16;
        lu01 = ((du01<<fp)*pdiv[dy01+2048])>>16;
        lv01 = ((dv01<<fp)*pdiv[dy01+2048])>>16;
        lz01 = (dz01*pdiv[dy01+2048])>>16;
    }

int dx12 = x2 - x1;
int dy12 = y2 - y1;
int du12 = u2 - u1;
int dv12 = v2 - v1;
int dz12 = z2 - z1;

    if (dy12!=0)
    {
        lx12 = ((dx12<<fp)*pdiv[dy12+2048])>>16;
        lu12 = ((du12<<fp)*pdiv[dy12+2048])>>16;
        lv12 = ((dv12<<fp)*pdiv[dy12+2048])>>16;
        lz12 = (dz12*pdiv[dy12+2048])>>16;
    }

int dx02 = x2 - x0;
int dy02 = y2 - y0;
int du02 = u2 - u0;
int dv02 = v2 - v0;
int dz02 = z2 - z0;

    if (dy02!=0)
    {
        lx02 = ((dx02<<fp)*pdiv[dy02+2048])>>16;
        lu02 = ((du02<<fp)*pdiv[dy02+2048])>>16;
        lv02 = ((dv02<<fp)*pdiv[dy02+2048])>>16;
        lz02 = (dz02*pdiv[dy02+2048])>>16;
    }

int vramofs;
int x, y;

int x01 = x0<<fp;
int x02 = x01;
int u01 = u0<<fp;
int u02 = u01;
int v01 = v0<<fp;
int v02 = v01;
int z01 = z0;
int z02 = z01;

int du, dv, dz;
int su1, su2;
int sv1, sv2;
int sx1, sx2;
int sz1, sz2;

int px, py;

    int yp = y0 * WIDTH;
    for (y = y0; y<y1; y++)
    {
        sx1 = x01>>fp;
        sx2 = x02>>fp;
        su1 = u01;
        su2 = u02;
        sv1 = v01;
        sv2 = v02;
        sz1 = z01;
        sz2 = z02;

        if (sx1>sx2)
        {
            temp = sx1; sx1 = sx2; sx2 = temp;
            temp = su1; su1 = su2; su2 = temp;
            temp = sv1; sv1 = sv2; sv2 = temp;
            temp = sz1; sz1 = sz2; sz2 = temp;
        }

        if (sx2!=sx1)
        {
            du = ((su2 - su1)*pdiv[(sx2-sx1)+2048])>>16;
            dv = ((sv2 - sv1)*pdiv[(sx2-sx1)+2048])>>16;
            dz = ((sz2 - sz1)*pdiv[(sx2-sx1)+2048])>>16;
        }

        yp+=WIDTH;
        vramofs = yp + sx1;
        for (x = sx1; x<sx2; x++)
        {
            su1+=du;
            sv1+=dv;
            sz1+=dz;
            if ((vramofs>=0 && vramofs<SSIZE) && sz1<zbuffer[vramofs])
            {
                zbuffer[vramofs] = sz1;
                c = texture[((su1>>fp)>>tshr) + (((sv1>>fp)>>tshr)<<(8-tshr))];
                *(vram+vramofs) = c;
            }
            vramofs++;
        }
        x01+=lx01;
        x02+=lx02;
        u01+=lu01;
        u02+=lu02;
        v01+=lv01;
        v02+=lv02;
        z01+=lz01;
        z02+=lz02;
    }

    x01 = x1<<fp;
    u01 = u1<<fp;
    v01 = v1<<fp;
    z01 = z1;

    yp = y1 * WIDTH;
    for (y = y1; y<y2; y++)
    {
        sx1 = x01>>fp;
        sx2 = x02>>fp;
        su1 = u01;
        su2 = u02;
        sv1 = v01;
        sv2 = v02;
        sz1 = z01;
        sz2 = z02;

        if (sx1>sx2)
        {
            temp = sx1; sx1 = sx2; sx2 = temp;
            temp = su1; su1 = su2; su2 = temp;
            temp = sv1; sv1 = sv2; sv2 = temp;
            temp = sz1; sz1 = sz2; sz2 = temp;
        }

        if (sx2!=sx1)
        {
            du = ((su2 - su1)*pdiv[(sx2-sx1)+2048])>>16;
            dv = ((sv2 - sv1)*pdiv[(sx2-sx1)+2048])>>16;
            dz = ((sz2 - sz1)*pdiv[(sx2-sx1)+2048])>>16;
        }

        yp+=WIDTH;
        vramofs = yp + sx1;
        for (x = sx1; x<sx2; x++)
        {
            su1+=du;
            sv1+=dv;
            sz1+=dz;
            if ((vramofs>=0 && vramofs<SSIZE) && sz1<zbuffer[vramofs])
            {
                zbuffer[vramofs] = sz1;
                c = texture[((su1>>fp)>>tshr) + (((sv1>>fp)>>tshr)<<(8-tshr))];
                *(vram+vramofs) = c;
            }
            vramofs++;
        }
        x01+=lx12;
        x02+=lx02;
        u01+=lu12;
        u02+=lu02;
        v01+=lv12;
        v02+=lv02;
        z01+=lz12;
        z02+=lz02;
    }
}


static inline void DrawEnvmappedTriangle(poly2d poly, unsigned short *vram, int tshr, unsigned short texture[])
{
int x0 =spts[poly.p0].x; int y0 =spts[poly.p0].y;
int x1 =spts[poly.p1].x; int y1 =spts[poly.p1].y;
int x2 =spts[poly.p2].x; int y2 =spts[poly.p2].y;

int c;

int u0 = point_tc[poly.p0].u; int v0 = point_tc[poly.p0].v;
int u1 = point_tc[poly.p1].u; int v1 = point_tc[poly.p1].v;
int u2 = point_tc[poly.p2].u; int v2 = point_tc[poly.p2].v;

// ===== Sort =====

int temp;
if (y1<y0)
{
    temp = u0; u0 = u1; u1 = temp;
    temp = v0; v0 = v1; v1 = temp;
    temp = x0; x0 = x1; x1 = temp;
    temp = y0; y0 = y1; y1 = temp;
}
if (y2<y0)
{
    temp = u0; u0 = u2; u2 = temp;
    temp = v0; v0 = v2; v2 = temp;
    temp = x0; x0 = x2; x2 = temp;
    temp = y0; y0 = y2; y2 = temp;
}
if (y2<y1)
{
    temp = u1; u1 = u2; u2 = temp;
    temp = v1; v1 = v2; v2 = temp;
    temp = x1; x1 = x2; x2 = temp;
    temp = y1; y1 = y2; y2 = temp;
}

// ===== Interpolation variables =====

int n;
int fp = 8;
int lx01=0, lx12=0, lx02=0;
int lu01=0, lu12=0, lu02=0;
int lv01=0, lv12=0, lv02=0;

int dx01 = x1 - x0;
int dy01 = y1 - y0;
int du01 = u1 - u0;
int dv01 = v1 - v0;

    if (dy01!=0)
    {
        lx01 = ((dx01<<fp)*pdiv[dy01+2048])>>16;
        lu01 = ((du01<<fp)*pdiv[dy01+2048])>>16;
        lv01 = ((dv01<<fp)*pdiv[dy01+2048])>>16;
    }

int dx12 = x2 - x1;
int dy12 = y2 - y1;
int du12 = u2 - u1;
int dv12 = v2 - v1;

    if (dy12!=0)
    {
        lx12 = ((dx12<<fp)*pdiv[dy12+2048])>>16;
        lu12 = ((du12<<fp)*pdiv[dy12+2048])>>16;
        lv12 = ((dv12<<fp)*pdiv[dy12+2048])>>16;
    }

int dx02 = x2 - x0;
int dy02 = y2 - y0;
int du02 = u2 - u0;
int dv02 = v2 - v0;

    if (dy02!=0)
    {
        lx02 = ((dx02<<fp)*pdiv[dy02+2048])>>16;
        lu02 = ((du02<<fp)*pdiv[dy02+2048])>>16;
        lv02 = ((dv02<<fp)*pdiv[dy02+2048])>>16;
    }

int vramofs;
int x, y;

int x01 = x0<<fp;
int x02 = x01;
int u01 = u0<<fp;
int u02 = u01;
int v01 = v0<<fp;
int v02 = v01;

int du, dv;
int su1, su2;
int sv1, sv2;
int sx1, sx2;

int px, py;

    int yp = y0 * WIDTH;
    for (y = y0; y<y1; y++)
    {
        sx1 = x01>>fp;
        sx2 = x02>>fp;
        su1 = u01;
        su2 = u02;
        sv1 = v01;
        sv2 = v02;

        if (sx1>sx2)
        {
            temp = sx1; sx1 = sx2; sx2 = temp;
            temp = su1; su1 = su2; su2 = temp;
            temp = sv1; sv1 = sv2; sv2 = temp;
        }

        if (sx2!=sx1)
        {
            du = ((su2 - su1)*pdiv[(sx2-sx1)+2048])>>16;
            dv = ((sv2 - sv1)*pdiv[(sx2-sx1)+2048])>>16;
        }

        yp+=WIDTH;
        vramofs = yp + sx1;
        for (x = sx1; x<sx2; x++)
        {
            su1+=du;
            sv1+=dv;
            if (vramofs>=0 && vramofs<SSIZE && x>=0 && x<WIDTH)
            {
                c = texture[((su1>>fp)>>tshr) + (((sv1>>fp)>>tshr)<<(8-tshr))];
                *(vram+vramofs) = c;
            }
            vramofs++;
        }
        x01+=lx01;
        x02+=lx02;
        u01+=lu01;
        u02+=lu02;
        v01+=lv01;
        v02+=lv02;
    }

    x01 = x1<<fp;
    u01 = u1<<fp;
    v01 = v1<<fp;

    yp = y1 * WIDTH;
    for (y = y1; y<y2; y++)
    {
        sx1 = x01>>fp;
        sx2 = x02>>fp;
        su1 = u01;
        su2 = u02;
        sv1 = v01;
        sv2 = v02;

        if (sx1>sx2)
        {
            temp = sx1; sx1 = sx2; sx2 = temp;
            temp = su1; su1 = su2; su2 = temp;
            temp = sv1; sv1 = sv2; sv2 = temp;
        }

        if (sx2!=sx1)
        {
            du = ((su2 - su1)*pdiv[(sx2-sx1)+2048])>>16;
            dv = ((sv2 - sv1)*pdiv[(sx2-sx1)+2048])>>16;
        }

        yp+=WIDTH;
        vramofs = yp + sx1;
        for (x = sx1; x<sx2; x++)
        {
            su1+=du;
            sv1+=dv;
            if (vramofs>=0 && vramofs<SSIZE && x>=0 && x<WIDTH)
            {
                c = texture[((su1>>fp)>>tshr) + (((sv1>>fp)>>tshr)<<(8-tshr))];
                *(vram+vramofs) = c;
            }
            vramofs++;
        }
        x01+=lx12;
        x02+=lx02;
        u01+=lu12;
        u02+=lu02;
        v01+=lv12;
        v02+=lv02;
    }
}


static inline void DrawEnvmappedTriangleZB(poly2d poly, unsigned short *vram, int tshr, unsigned short texture[])
{
int x0 =spts[poly.p0].x; int y0 =spts[poly.p0].y;
int x1 =spts[poly.p1].x; int y1 =spts[poly.p1].y;
int x2 =spts[poly.p2].x; int y2 =spts[poly.p2].y;

int c;

int u0 = point_tc[poly.p0].u; int v0 = point_tc[poly.p0].v;
int u1 = point_tc[poly.p1].u; int v1 = point_tc[poly.p1].v;
int u2 = point_tc[poly.p2].u; int v2 = point_tc[poly.p2].v;

int z0 = fpts[poly.p0].z;
int z1 = fpts[poly.p1].z;
int z2 = fpts[poly.p2].z;

// ===== Sort =====

int temp;
if (y1<y0)
{
    temp = u0; u0 = u1; u1 = temp;
    temp = v0; v0 = v1; v1 = temp;
    temp = z0; z0 = z1; z1 = temp;
    temp = x0; x0 = x1; x1 = temp;
    temp = y0; y0 = y1; y1 = temp;
}
if (y2<y0)
{
    temp = u0; u0 = u2; u2 = temp;
    temp = v0; v0 = v2; v2 = temp;
    temp = z0; z0 = z2; z2 = temp;
    temp = x0; x0 = x2; x2 = temp;
    temp = y0; y0 = y2; y2 = temp;
}
if (y2<y1)
{
    temp = u1; u1 = u2; u2 = temp;
    temp = v1; v1 = v2; v2 = temp;
    temp = z1; z1 = z2; z2 = temp;
    temp = x1; x1 = x2; x2 = temp;
    temp = y1; y1 = y2; y2 = temp;
}

// ===== Interpolation variables =====

int n;
int fp = 8;
int lx01=0, lx12=0, lx02=0;
int lu01=0, lu12=0, lu02=0;
int lv01=0, lv12=0, lv02=0;
int lz01=0, lz12=0, lz02=0;

int dx01 = x1 - x0;
int dy01 = y1 - y0;
int du01 = u1 - u0;
int dv01 = v1 - v0;
int dz01 = z1 - z0;

    if (dy01!=0)
    {
        lx01 = ((dx01<<fp)*pdiv[dy01+2048])>>16;
        lu01 = ((du01<<fp)*pdiv[dy01+2048])>>16;
        lv01 = ((dv01<<fp)*pdiv[dy01+2048])>>16;
        lz01 = (dz01*pdiv[dy01+2048])>>16;
    }

int dx12 = x2 - x1;
int dy12 = y2 - y1;
int du12 = u2 - u1;
int dv12 = v2 - v1;
int dz12 = z2 - z1;

    if (dy12!=0)
    {
        lx12 = ((dx12<<fp)*pdiv[dy12+2048])>>16;
        lu12 = ((du12<<fp)*pdiv[dy12+2048])>>16;
        lv12 = ((dv12<<fp)*pdiv[dy12+2048])>>16;
        lz12 = (dz12*pdiv[dy12+2048])>>16;
    }

int dx02 = x2 - x0;
int dy02 = y2 - y0;
int du02 = u2 - u0;
int dv02 = v2 - v0;
int dz02 = z2 - z0;

    if (dy02!=0)
    {
        lx02 = ((dx02<<fp)*pdiv[dy02+2048])>>16;
        lu02 = ((du02<<fp)*pdiv[dy02+2048])>>16;
        lv02 = ((dv02<<fp)*pdiv[dy02+2048])>>16;
        lz02 = (dz02*pdiv[dy02+2048])>>16;
    }

int vramofs;
int x, y;

int x01 = x0<<fp;
int x02 = x01;
int u01 = u0<<fp;
int u02 = u01;
int v01 = v0<<fp;
int v02 = v01;
int z01 = z0;
int z02 = z01;

int du, dv, dz;
int su1, su2;
int sv1, sv2;
int sx1, sx2;
int sz1, sz2;

int px, py;

    int yp = y0 * WIDTH;
    for (y = y0; y<y1; y++)
    {
        sx1 = x01>>fp;
        sx2 = x02>>fp;
        su1 = u01;
        su2 = u02;
        sv1 = v01;
        sv2 = v02;
        sz1 = z01;
        sz2 = z02;

        if (sx1>sx2)
        {
            temp = sx1; sx1 = sx2; sx2 = temp;
            temp = su1; su1 = su2; su2 = temp;
            temp = sv1; sv1 = sv2; sv2 = temp;
            temp = sz1; sz1 = sz2; sz2 = temp;
        }

        if (sx2!=sx1)
        {
            du = ((su2 - su1)*pdiv[(sx2-sx1)+2048])>>16;
            dv = ((sv2 - sv1)*pdiv[(sx2-sx1)+2048])>>16;
            dz = ((sz2 - sz1)*pdiv[(sx2-sx1)+2048])>>16;
        }

        yp+=WIDTH;
        vramofs = yp + sx1;
        for (x = sx1; x<sx2; x++)
        {
            su1+=du;
            sv1+=dv;
            sz1+=dz;
            if ((vramofs>=0 && vramofs<SSIZE) && sz1<zbuffer[vramofs])
            {
                zbuffer[vramofs] = sz1;
                c = texture[((su1>>fp)>>tshr) + (((sv1>>fp)>>tshr)<<(8-tshr))];
                *(vram+vramofs) = c;
            }
            vramofs++;
        }
        x01+=lx01;
        x02+=lx02;
        u01+=lu01;
        u02+=lu02;
        v01+=lv01;
        v02+=lv02;
        z01+=lz01;
        z02+=lz02;
    }

    x01 = x1<<fp;
    u01 = u1<<fp;
    v01 = v1<<fp;
    z01 = z1;

    yp = y1 * WIDTH;
    for (y = y1; y<y2; y++)
    {
        sx1 = x01>>fp;
        sx2 = x02>>fp;
        su1 = u01;
        su2 = u02;
        sv1 = v01;
        sv2 = v02;
        sz1 = z01;
        sz2 = z02;

        if (sx1>sx2)
        {
            temp = sx1; sx1 = sx2; sx2 = temp;
            temp = su1; su1 = su2; su2 = temp;
            temp = sv1; sv1 = sv2; sv2 = temp;
            temp = sz1; sz1 = sz2; sz2 = temp;
        }

        if (sx2!=sx1)
        {
            du = ((su2 - su1)*pdiv[(sx2-sx1)+2048])>>16;
            dv = ((sv2 - sv1)*pdiv[(sx2-sx1)+2048])>>16;
            dz = ((sz2 - sz1)*pdiv[(sx2-sx1)+2048])>>16;
        }

        yp+=WIDTH;
        vramofs = yp + sx1;
        for (x = sx1; x<sx2; x++)
        {
            su1+=du;
            sv1+=dv;
            sz1+=dz;
            if ((vramofs>=0 && vramofs<SSIZE) && sz1<zbuffer[vramofs])
            {
                zbuffer[vramofs] = sz1;
                c = texture[((su1>>fp)>>tshr) + (((sv1>>fp)>>tshr)<<(8-tshr))];
                *(vram+vramofs) = c;
            }
            vramofs++;
        }
        x01+=lx12;
        x02+=lx02;
        u01+=lu12;
        u02+=lu02;
        v01+=lv12;
        v02+=lv02;
        z01+=lz12;
        z02+=lz02;
    }
}

void zsort(int zsortdata[], object3d *obj)
{
    int i, mz;
    for (i=0; i<obj->npls; i++)
    {
        mz = (fpts[obj->poly[i].p0].z + fpts[obj->poly[i].p1].z + fpts[obj->poly[i].p2].z)>>2;
        zsortdata[i] = -mz;
		swp[i] = i;
    }
}

void Render(object3d *obj, unsigned short *vram)
{
    rotate3d_normals(obj);
    rotate3d_pt_normals(obj);
    CalcPolyColor(obj);
    CalcPointColor(obj);
    zsort (zdata, obj);
	quicksort(0, obj->npls - 1, zdata);

    int i;
    for (i=0; i<obj->npts; i++)
    {
        int tu = (pt_norms[i].x>>9) + 127;
        int tv = (pt_norms[i].y>>9) + 127;
        point_tc[i].u = abs(tu) & 255;
        point_tc[i].v = abs(tv) & 255;
    }

    int j, m;
    int vx0, vy0, vx1, vy1, n;

    for (i=0; i<obj->npls; i++)
    {
        j = swp[i];
        vx0 = spts[obj->poly[j].p0].x - spts[obj->poly[j].p1].x;
        vy0 = spts[obj->poly[j].p0].y - spts[obj->poly[j].p1].y;
        vx1 = spts[obj->poly[j].p2].x - spts[obj->poly[j].p1].x;
        vy1 = spts[obj->poly[j].p2].y - spts[obj->poly[j].p1].y;
        n = vx0 * vy1 - vx1 * vy0;
        if (n<0)
        {
            m = obj->poly[j].m;
            switch(obj->mtrl[m].rmode)
            {
                case WIRE:
                break;

                case FLAT:
                    obj->poly[j].c = spls[j].c;
                    DrawFlatTriangle(obj->poly[j], vram, obj->mtrl[m].shade[0]);
                break;

                case GOURAUD:
                    DrawGouraudTriangle(obj->poly[j], vram, obj->mtrl[m].shade[0]);
                break;

                case TEXTURE:
                    DrawTextureTriangle(obj->poly[j], vram, obj->mtrl[m].txtr[0]->tshr, obj->mtrl[m].txtr[0]->bitmap);
                break;

                case ENVMAP:
                    DrawEnvmappedTriangle(obj->poly[j], vram, obj->mtrl[m].txtr[0]->tshr, obj->mtrl[m].txtr[0]->bitmap);
                break;
            }
        }
    }
}

void RenderOld(object3d *obj, int shadenum, unsigned short *vram)
{
	int i, j;
    int vx0, vy0, vx1, vy1, n;
    int tu, tv, s;
    int texshr = 2;

	switch (RenderMode)
	{


	case POINTS:
		for (i=obj->npts-1; i>=0; i--)
            if (spts[i].x>=0 && spts[i].x<WIDTH && spts[i].y>=0 && spts[i].y<HEIGHT)
                *(vram + spts[i].x + spts[i].y * WIDTH) = 0xFFFF;
	break;


	case WIRE:
		for (i=obj->nlns-1; i>=0; i--)
            drawline(obj->line[i], vram);
	break;

	case FLAT:
        rotate3d_normals(obj);
        CalcPolyColor(obj);

        zsort (zdata, obj);
		quicksort(0, obj->npls - 1, zdata);

        for (i=0; i<obj->npls; i++)
        {
            j = swp[i];
            vx0 = spts[obj->poly[j].p0].x - spts[obj->poly[j].p1].x;
            vy0 = spts[obj->poly[j].p0].y - spts[obj->poly[j].p1].y;
            vx1 = spts[obj->poly[j].p2].x - spts[obj->poly[j].p1].x;
            vy1 = spts[obj->poly[j].p2].y - spts[obj->poly[j].p1].y;
            n = vx0 * vy1 - vx1 * vy0;
            if (n<0)
            {
                obj->poly[j].c = spls[j].c;
                DrawFlatTriangle(obj->poly[j], vram, shades[shadenum]);
            }
        }
	break;

	case (FLAT | ZBUFFER):
        rotate3d_normals(obj);
        CalcPolyColor(obj);

        //ClearZbuffer();
        memset(zbuffer, 0xFFFFFF, sizeof(unsigned int) * SSIZE);
        for (i=obj->npls-1; i>=0; i--)
        {
            obj->poly[i].c = spls[i].c;
            if (norms[i].z>=0)
                DrawFlatTriangleZB(obj->poly[i], vram, shades[shadenum]);
        }
	break;

	case GOURAUD:
        rotate3d_pt_normals(obj);
        CalcPointColor(obj);

        zsort (zdata, obj);
		quicksort(0, obj->npls - 1, zdata);

        for (i=0; i<obj->npls; i++)
        {
            j = swp[i];
            vx0 = spts[obj->poly[j].p0].x - spts[obj->poly[j].p1].x;
            vy0 = spts[obj->poly[j].p0].y - spts[obj->poly[j].p1].y;
            vx1 = spts[obj->poly[j].p2].x - spts[obj->poly[j].p1].x;
            vy1 = spts[obj->poly[j].p2].y - spts[obj->poly[j].p1].y;
            n = vx0 * vy1 - vx1 * vy0;
            if (n<0)
                DrawGouraudTriangle(obj->poly[j], vram, shades[shadenum]);
        }
    break;

	case (GOURAUD | ZBUFFER):
        rotate3d_normals(obj);
        rotate3d_pt_normals(obj);

        CalcPointColor(obj);
        //ClearZbuffer();
        memset(zbuffer, 0xFFFFFF, sizeof(unsigned int) * SSIZE);
        for (i=obj->npls-1; i>=0; i--)
        {
            if (norms[i].z>=0)
                DrawGouraudTriangleZB(obj->poly[i], vram, shades[shadenum]);
        }
    break;

	case (TEXTURE):
        zsort (zdata, obj);
		quicksort(0, obj->npls - 1, zdata);

        for (i=0; i<obj->npls; i++)
        {
            j = swp[i];
            vx0 = spts[obj->poly[j].p0].x - spts[obj->poly[j].p1].x;
            vy0 = spts[obj->poly[j].p0].y - spts[obj->poly[j].p1].y;
            vx1 = spts[obj->poly[j].p2].x - spts[obj->poly[j].p1].x;
            vy1 = spts[obj->poly[j].p2].y - spts[obj->poly[j].p1].y;
            n = vx0 * vy1 - vx1 * vy0;
            if (n<0)
            {
                DrawTextureTriangle(obj->poly[j], vram, texshr, env1);
            }
        }
    break;

	case (TEXTURE | ZBUFFER):
        rotate3d_normals(obj);

        //ClearZbuffer();
        memset(zbuffer, 0xFFFFFF, sizeof(unsigned int) * SSIZE);
        for (i=obj->npls-1; i>=0; i--)
        {
            if (norms[i].z>=0)
                DrawTextureTriangleZB(obj->poly[i], vram, 0, env1);
        }
    break;

	case (ENVMAP):
        rotate3d_pt_normals(obj);

        zsort (zdata, obj);
		quicksort(0, obj->npls - 1, zdata);

    	for (i=0; i<obj->npts; i++)
    	{
            tu = (pt_norms[i].x>>9) + 127;
            tv = (pt_norms[i].y>>9) + 127;
    		point_tc[i].u = abs(tu) & 255;
    		point_tc[i].v = abs(tv) & 255;
        }

        for (i=0; i<obj->npls; i++)
        {
            j = swp[i];
            vx0 = spts[obj->poly[j].p0].x - spts[obj->poly[j].p1].x;
            vy0 = spts[obj->poly[j].p0].y - spts[obj->poly[j].p1].y;
            vx1 = spts[obj->poly[j].p2].x - spts[obj->poly[j].p1].x;
            vy1 = spts[obj->poly[j].p2].y - spts[obj->poly[j].p1].y;
            n = vx0 * vy1 - vx1 * vy0;
//            if (n<0)
                DrawEnvmappedTriangle(obj->poly[j], vram, texshr, env1);
        }
    break;

	case (ENVMAP | ZBUFFER):
        rotate3d_normals(obj);
        rotate3d_pt_normals(obj);

        //ClearZbuffer();
        memset(zbuffer, 0xFFFFFF, sizeof(unsigned int) * SSIZE);
    	for (i=obj->npts-1; i>=0; i--)
    	{
            tu = (pt_norms[i].x>>9) + 127;
            tv = (pt_norms[i].y>>9) + 127;
    		point_tc[i].u = abs(tu) & 255;
    		point_tc[i].v = abs(tv) & 255;
        }


        for (i=obj->npls-1; i>=0; i--)
        {
            if (norms[i].z>=0)
                DrawEnvmappedTriangleZB(obj->poly[i], vram, 2, env1);
        }
    break;

	case VBALLS:
		for (i=obj->npts-1; i>=0; i--)
		{
			swp[i] = i;
			zdata[i] = -fpts[i].z;
		}

		quicksort(0, obj->npts - 1, zdata);

		for (i=obj->npts-1; i>=0; i--)
			drawball(spts[swp[i]], vram);
	break;

	default:
	break;
	}

}
