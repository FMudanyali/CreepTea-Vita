
#define vbn 512

#define		MAXDATA 262144

#define		POINTS		1
#define		WIRE		2
#define		FLAT		4
#define		GOURAUD		8
#define		ENVMAP		16
#define     TEXTURE      32

#define		VBALLS		0

#define		TEXTUREPLASMA 128
#define     ZBUFFER     256


#define     LIGHTVIEW   0
#define     LIGHTMOVE   1

#define PI 3.14151693
#define D2R 180.0/PI

typedef struct vector3d
{
    int x;
    int y;
    int z;
} vector3d;

typedef struct point2d
{
	int x;
	int y;
	unsigned short c;
} point2d;


typedef struct point3d
{
	int x;
	int y;
	int z;
	unsigned short c;
} point3d;

typedef struct texture
{
    unsigned int tshr;
    unsigned short *bitmap;
}texture;

typedef struct material
{
    int rmode;
    int nshade;
    int ntexture;
    unsigned short **shade;
    texture **txtr;
} material;

typedef struct tcord
{
    int u, v;
} tcord;

typedef struct poly2d
{
	int p0; tcord tc0;
	int p1; tcord tc1;
	int p2; tcord tc2;
	int c;
    int m;
} poly2d;


typedef struct line2d
{
	int p0;
	int p1;
	int c;
}line2d;


typedef struct rot3d
{
	float x;
	float y;
	float z;
}rot3d;


typedef struct pos3d
{
	float x;
	float y;
	float z;
}pos3d;


typedef struct object3d
{
    int npts, npls, nlns;

    point3d *point;
    poly2d *poly;
    line2d *line;

    vector3d *normal;
    vector3d *pt_normal;

    material *mtrl;

    rot3d rot;
    pos3d pos;
}object3d;

vector3d CrossProduct(vector3d v1, vector3d v2);
int DotProduct(vector3d v1, vector3d v2);
vector3d Normalize(vector3d v);
vector3d NegVec(vector3d v);

void Init3d();
void RunScene3d(unsigned short *vram, int sn);

void Calc3d(object3d *obj);

void rotate3d (object3d *obj);
void translate3d (object3d *obj);
void project3d (object3d *obj);

void rotate3d_normals (object3d *obj);
void rotate3d_pt_normals (object3d *obj);

void CalcPointColor(object3d *obj);
void CalcPolyColor(object3d *obj);

void quicksort (int lo, int hi, int data[]);

