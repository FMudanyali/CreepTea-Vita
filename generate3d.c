#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "engine3d.h"
#include "generate3d.h"
#include "ZeDemo.h"

object3d object[16];

extern point2d spts[MAXDATA];
unsigned char addtimes[MAXDATA];

extern unsigned short shades[64][256];
extern unsigned short env1[];
extern unsigned short env2[];
extern unsigned short env3[];

extern int fsin4[2048], fsin5[2048], fsin6[2048];

extern int prticks;

void CalcNorms(object3d *obj, int neg)
{
    vector3d v1,v2;

    obj->normal = (vector3d*)malloc(obj->npls * sizeof(vector3d));

    int i;
    for (i=0; i<obj->npls; i++)
    {
        v1.x = obj->point[obj->poly[i].p2].x - obj->point[obj->poly[i].p1].x;
        v1.y = obj->point[obj->poly[i].p2].y - obj->point[obj->poly[i].p1].y;
        v1.z = obj->point[obj->poly[i].p2].z - obj->point[obj->poly[i].p1].z;

        v2.x = obj->point[obj->poly[i].p1].x - obj->point[obj->poly[i].p0].x;
        v2.y = obj->point[obj->poly[i].p1].y - obj->point[obj->poly[i].p0].y;
        v2.z = obj->point[obj->poly[i].p1].z - obj->point[obj->poly[i].p0].z;

        obj->normal[i] = Normalize(CrossProduct(v1,v2));
        if (neg==1) obj->normal[i] = NegVec(obj->normal[i]);
    }
}

void CalcPtNorms(object3d *obj)
{
    obj->pt_normal = (vector3d*)malloc(obj->npts * sizeof(vector3d));

    int i;
    for (i=0; i<MAXDATA; i++)
        addtimes[i] = 0;

    for (i=0; i<obj->npts; i++)
    {
        obj->pt_normal[i].x = 0;
        obj->pt_normal[i].y = 0;
        obj->pt_normal[i].z = 0;
    }

    for (i=0; i<obj->npls; i++)
    {
        obj->pt_normal[obj->poly[i].p0].x += obj->normal[i].x;
        obj->pt_normal[obj->poly[i].p0].y += obj->normal[i].y;
        obj->pt_normal[obj->poly[i].p0].z += obj->normal[i].z;
        obj->pt_normal[obj->poly[i].p1].x += obj->normal[i].x;
        obj->pt_normal[obj->poly[i].p1].y += obj->normal[i].y;
        obj->pt_normal[obj->poly[i].p1].z += obj->normal[i].z;
        obj->pt_normal[obj->poly[i].p2].x += obj->normal[i].x;
        obj->pt_normal[obj->poly[i].p2].y += obj->normal[i].y;
        obj->pt_normal[obj->poly[i].p2].z += obj->normal[i].z;
        addtimes[obj->poly[i].p0]++;
        addtimes[obj->poly[i].p1]++;
        addtimes[obj->poly[i].p2]++;
    }

    for (i=0; i<obj->npts; i++)
    {
        if (addtimes[i]!=0)
        {
            obj->pt_normal[i].x /= addtimes[i];
            obj->pt_normal[i].y /= addtimes[i]; 
            obj->pt_normal[i].z /= addtimes[i];
            obj->pt_normal[i] = Normalize(obj->pt_normal[i]);
        }
    }
}

void ReversePolygonOrder(object3d *obj)
{
    int i, a, b, c;
    for (i=0; i<obj->npls; i++)
    {
        a = obj->poly[i].p0;
        b = obj->poly[i].p1;
        c = obj->poly[i].p2;
        obj->poly[i].p0 = c;
        obj->poly[i].p1 = b;
        obj->poly[i].p2 = a;
    }
}


void Load32bit3doFile(char* filename, object3d *obj, int neg, float stdiv)
{
	FILE *obj3d;
	obj3d=fopen(filename,"rb");

	obj->npts=fgetc(obj3d)+(fgetc(obj3d)<<8)+(fgetc(obj3d)<<16)+(fgetc(obj3d)<<24);
	obj->nlns=fgetc(obj3d)+(fgetc(obj3d)<<8)+(fgetc(obj3d)<<16)+(fgetc(obj3d)<<24);
	obj->npls=fgetc(obj3d)+(fgetc(obj3d)<<8)+(fgetc(obj3d)<<16)+(fgetc(obj3d)<<24);

    obj->point = (point3d*)malloc(obj->npts * sizeof(point3d));
    obj->line = (line2d*)malloc(obj->nlns * sizeof(line2d));
    obj->poly = (poly2d*)malloc(obj->npls * sizeof(poly2d));

    int i;
	for (i=0; i<obj->npts; i++)
	{
		obj->point[i].x = (fgetc(obj3d)+(fgetc(obj3d)<<8)+(fgetc(obj3d)<<16)+(fgetc(obj3d)<<24) - 65536)/stdiv;
		obj->point[i].y = (fgetc(obj3d)+(fgetc(obj3d)<<8)+(fgetc(obj3d)<<16)+(fgetc(obj3d)<<24) - 65536)/stdiv;
		obj->point[i].z = (fgetc(obj3d)+(fgetc(obj3d)<<8)+(fgetc(obj3d)<<16)+(fgetc(obj3d)<<24) - 65536)/stdiv;
		obj->point[i].c = 0xFFFF;
	}

	for (i=0; i<obj->nlns; i++)
	{
        obj->line[i].p0 = fgetc(obj3d)+(fgetc(obj3d)<<8)+(fgetc(obj3d)<<16)+(fgetc(obj3d)<<24);
        obj->line[i].p1 = fgetc(obj3d)+(fgetc(obj3d)<<8)+(fgetc(obj3d)<<16)+(fgetc(obj3d)<<24);
        obj->line[i].c = 0xFFFFFF;
	}

	for (i=0; i<obj->npls; i++)
	{
        obj->poly[i].p0 = fgetc(obj3d)+(fgetc(obj3d)<<8)+(fgetc(obj3d)<<16)+(fgetc(obj3d)<<24);
        obj->poly[i].p1 = fgetc(obj3d)+(fgetc(obj3d)<<8)+(fgetc(obj3d)<<16)+(fgetc(obj3d)<<24);
        obj->poly[i].p2 = fgetc(obj3d)+(fgetc(obj3d)<<8)+(fgetc(obj3d)<<16)+(fgetc(obj3d)<<24);
	}

	fclose(obj3d);

    CalcNorms(obj, neg);
    CalcPtNorms(obj);
}


void Load16bit3do(object3d *obj, unsigned char* objdata, int neg)
{
    obj->npts = *objdata + (*(objdata+1)<<8);
    obj->nlns = *(objdata+2) + (*(objdata+3)<<8);
    obj->npls = *(objdata+4) + (*(objdata+5)<<8);

    obj->point = malloc(obj->npts * sizeof(point3d));
    obj->line = malloc(obj->nlns * sizeof(line2d));
    obj->poly = malloc(obj->npls * sizeof(poly2d));

    objdata+=6;
    int i;
    for (i=0; i<obj->npts; i++)
    {
        obj->point[i].x = (*(objdata++) - 128) >> 1;
        obj->point[i].y = (*(objdata++) - 128) >> 1;
        obj->point[i].z = (*(objdata++) - 128) >> 1;
        obj->point[i].c = 0xFFFF;
    }

    for (i=0; i<obj->nlns; i++)
    {
        obj->line[i].p0 = *objdata + (*(objdata+1)<<8);
        obj->line[i].p1 = *(objdata+2) + (*(objdata+3)<<8);
        objdata+=4;
        obj->line[i].c = 0xFFFF;
    }

    for (i=0; i<obj->npls; i++)
    {
        obj->poly[i].p0 = *objdata + (*(objdata+1)<<8);
        obj->poly[i].p1 = *(objdata+2) + (*(objdata+3)<<8);
        obj->poly[i].p2 = *(objdata+4) + (*(objdata+5)<<8);
        objdata+=6;
    }

    CalcNorms(obj, neg);
    CalcPtNorms(obj);
}


void Wave(object3d *obj)
{
    int x, y;
    int grid_width = 32;
    int grid_height = 32;
    int tm1 = (prticks % (75<<5)) >> 5;
    int tm2 = (prticks % (50<<4)) >> 4;

    int k = 0, l;
    for (y=0; y<=grid_height; y++)
    {
        l = fsin6[y+tm1];
        for (x=0; x<=grid_width; x++)
        {
//            obj->point[k].z = sin((x+(prticks/192.0))/3.0)*12.0 + sin((y+(prticks/128.0))/2.0)*4.0 + sin((x + y+(prticks/96.0))/1.5)*8.0;
                obj->point[k].z = fsin4[x+y] + fsin5[x+tm2] + l;
            k++;
        }
    }
}

void InitTestObject(object3d *obj, int objn)
{
    int tl = 4, tr = 252;
    int i, k;
    int adj = 2;

    int grid_width = 32;
    int grid_height = 32;
    int x, y;

    printf("DEBUG: InitTestObject() objn case Number = %d\n", objn);
    switch(objn)
    {
        case 0:
            obj->npts = (grid_width+1) * (grid_height+1);
            obj->npls = grid_width * grid_height * 2;

            obj->point = malloc(obj->npts * sizeof(point3d));
            obj->poly = malloc(obj->npls * sizeof(poly2d));

            k = 0;
            for (y=0; y<=grid_height; y++)
            {
                for (x=0; x<=grid_width; x++)
                {
                    obj->point[k].x = (x - (grid_width>>1))*10;
                    obj->point[k].y = (y - (grid_height>>1))*10;
                    obj->point[k].z = sin(x/1)*8 + sin(y/1)*12 + sin((x+y)/2.0)*12 + sin(x/2.0 + y/1.0)*8 + sin(x/4.0 + sin(y/8.0)*4.0)*4;
                    k++;
                }
            }

            k = 0;
            for (y=0; y<grid_height; y++)
            {
                for (x=0; x<grid_width; x++)
                {
                    obj->poly[k].p0 = x+y*(grid_width+1); obj->poly[k].p1 = x+1+y*(grid_width+1); obj->poly[k].p2 = x+(y+1)*(grid_width+1);
                    obj->poly[k+1].p0 = x+(y+1)*(grid_width+1); obj->poly[k+1].p1 = x+1+y*(grid_width+1); obj->poly[k+1].p2 = x+1+(y+1)*(grid_width+1);
                    k+=2;
                }
            }

            obj->pos.x = 0; obj->pos.y = 0; obj->pos.z = 256;
            obj->rot.x = 0; obj->rot.y = 0; obj->rot.z = 0;

            obj->mtrl = malloc(1 * sizeof(material));

            obj->mtrl[0].rmode = GOURAUD;
            obj->mtrl[0].nshade = 1;
            obj->mtrl[0].shade = malloc(obj->mtrl[0].nshade * sizeof(unsigned short*));
            obj->mtrl[0].shade[0] = shades[1];

            for (i=0; i<obj->npls; i++)
                obj->poly[i].m = 0;

            CalcNorms(obj, 1);
            CalcPtNorms(obj);
        break;

        case 1:
            obj->npts = (grid_width+1) * (grid_height+1);
            obj->npls = grid_width * grid_height * 2;

            obj->point = malloc(obj->npts * sizeof(point3d));
            obj->poly = malloc(obj->npls * sizeof(poly2d));

            k = 0;
            for (y=0; y<=grid_height; y++)
            {
                for (x=0; x<=grid_width; x++)
                {
                    obj->point[k].x = (x - (grid_width>>1))*10;
                    obj->point[k].y = (y - (grid_height>>1))*10;
                    obj->point[k].z = sin(x/4.0)*16 + sin(y/4.0)*16;
                    k++;
                }
            }

            k = 0;
            for (y=0; y<grid_height; y++)
            {
                for (x=0; x<grid_width; x++)
                {
                    obj->poly[k].p0 = x+y*(grid_width+1); obj->poly[k].p1 = x+1+y*(grid_width+1); obj->poly[k].p2 = x+(y+1)*(grid_width+1);
                    obj->poly[k+1].p0 = x+(y+1)*(grid_width+1); obj->poly[k+1].p1 = x+1+y*(grid_width+1); obj->poly[k+1].p2 = x+1+(y+1)*(grid_width+1);
                    k+=2;
                }
            }

            obj->pos.x = 0; obj->pos.y = 0; obj->pos.z = 256;
            obj->rot.x = 0; obj->rot.y = 0; obj->rot.z = 0;

            obj->mtrl = malloc(1 * sizeof(material));
            obj->mtrl[0].rmode = ENVMAP;
            obj->mtrl[0].ntexture = 1;
            obj->mtrl[0].txtr = malloc(obj->mtrl[0].ntexture * sizeof(texture*));
            obj->mtrl[0].txtr[0] = malloc(sizeof(texture));
            obj->mtrl[0].txtr[0]->tshr = 2;
            obj->mtrl[0].txtr[0]->bitmap = env1;
            for (i=0; i<obj->npls; i++)
                obj->poly[i].m = 0;

            CalcNorms(obj, 1);
            CalcPtNorms(obj);
        break;

        case 2:
            Load32bit3doFile("app0:magnolia.3do", obj, 1, 1024);

            obj->mtrl = malloc(1 * sizeof(material));
            obj->mtrl[0].rmode = GOURAUD;
            obj->mtrl[0].nshade = 1;
            obj->mtrl[0].shade = malloc(obj->mtrl[0].nshade * sizeof(unsigned short*));
            obj->mtrl[0].shade[0] = shades[2];
            for (i=0; i<obj->npls; i++)
                obj->poly[i].m = 0;

            obj->pos.x = 0; obj->pos.y = 180; obj->pos.z = 8192;
            obj->rot.x = 0; obj->rot.y = 0; obj->rot.z = 0;
        break;

        case 3:
            obj->npts = (grid_width+1) * (grid_height+1);
            obj->npls = grid_width * grid_height * 2;

            obj->point = malloc(obj->npts * sizeof(point3d));
            obj->poly = malloc(obj->npls * sizeof(poly2d));

            k = 0;
            for (y=0; y<=grid_height; y++)
            {
                for (x=0; x<=grid_width; x++)
                {
                    obj->point[k].x = (x - (grid_width>>1))*10;
                    obj->point[k].y = (y - (grid_height>>1))*10;
                    obj->point[k].z = sin(x/3.0)*16 + sin(y/2.0)*16 + sin((x+y)/1.0)*8;
                    k++;
                }
            }

            k = 0;
            for (y=0; y<grid_height; y++)
            {
                for (x=0; x<grid_width; x++)
                {
                    obj->poly[k].p0 = x+y*(grid_width+1); obj->poly[k].p1 = x+1+y*(grid_width+1); obj->poly[k].p2 = x+(y+1)*(grid_width+1);
                    obj->poly[k+1].p0 = x+(y+1)*(grid_width+1); obj->poly[k+1].p1 = x+1+y*(grid_width+1); obj->poly[k+1].p2 = x+1+(y+1)*(grid_width+1);
                    k+=2;
                }
            }

            obj->pos.x = 0; obj->pos.y = 0; obj->pos.z = 384;
            obj->rot.x = 0; obj->rot.y = 0; obj->rot.z = 0;

            obj->mtrl = malloc(1 * sizeof(material));
            obj->mtrl[0].rmode = ENVMAP;
            obj->mtrl[0].ntexture = 1;
            obj->mtrl[0].txtr = malloc(obj->mtrl[0].ntexture * sizeof(texture*));
            obj->mtrl[0].txtr[0] = malloc(sizeof(texture));
            obj->mtrl[0].txtr[0]->tshr = 2;
            obj->mtrl[0].txtr[0]->bitmap = env1;
            for (i=0; i<obj->npls; i++)
                obj->poly[i].m = 0;

            ReversePolygonOrder(obj);

            CalcNorms(obj, 1);
            CalcPtNorms(obj);
        break;

        case 6:
            Load32bit3doFile("app0:gp2x.3do", obj, 1, 256);

            obj->mtrl = malloc(2 * sizeof(material));

            obj->mtrl[0].rmode = ENVMAP;
            obj->mtrl[0].ntexture = 1;
            obj->mtrl[0].txtr = malloc(obj->mtrl[0].ntexture * sizeof(texture*));
            obj->mtrl[0].txtr[0] = malloc(sizeof(texture));
            obj->mtrl[0].txtr[0]->tshr = 2;
            obj->mtrl[0].txtr[0]->bitmap = env3;

            obj->mtrl[1].rmode = ENVMAP;
            obj->mtrl[1].ntexture = 1;
            obj->mtrl[1].txtr = malloc(obj->mtrl[1].ntexture * sizeof(texture*));
            obj->mtrl[1].txtr[0] = malloc(sizeof(texture));
            obj->mtrl[1].txtr[0]->tshr = 2;
            obj->mtrl[1].txtr[0]->bitmap = env2;

            for (i=0; i<64; i++)
                obj->poly[i].m = 1;
            for (i=64; i<356; i++)
                obj->poly[i].m = 0;
            for (i=356; i<494; i++)
                obj->poly[i].m = 1;
            for (i=494; i<obj->npls; i++)
                obj->poly[i].m = 0;

            obj->pos.x = 0; obj->pos.y = 0; obj->pos.z = 512;
            obj->rot.x = PI/2; obj->rot.y = 0; obj->rot.z = PI/8;
        break;

        default:
        break;
    }    
}
