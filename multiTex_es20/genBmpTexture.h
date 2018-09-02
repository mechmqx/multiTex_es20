#ifndef __GENBMPTEXTURE__
#define __GENBMPTEXTURE__

typedef unsigned char uchar;
typedef unsigned int   uint;

typedef struct tagImgBMP{
	uchar* data;
	int width;
	int height;
	int bit;
}sIMGBMP;

static int bmpImageLoad(char* filename, sIMGBMP* bmpTex);
static int bmpTextureCreate(sIMGBMP* bmpTex,int *texID);
int createBmpTexture(char* filename,int *pTexId);
#endif 
