
#define pi 3.14151693
#define rang 128
#define rang2 256
#define d2r 180.0/pi

#define Twidth 128
#define Theight 128
#define shl_theight 7

#define blob_width 128
#define blob_height 128

#define nstars 256

typedef struct ColorRGB
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
} ColorRGB;


void MakeColors(unsigned short cols[], ColorRGB c0, ColorRGB c1, int n0, int n1);
void MakeSines(int sines[], int n, float freq, float hght1, float hght2);
void SetColors();

void precalcs();
void DrawText_(int xtp, int ytp, int cn, char *text, int bpp, unsigned short *vram);

void InitRadialBitmap1();
void InitRadialBitmap2();

