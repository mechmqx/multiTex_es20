#include "genBmpTexture.h"
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
//#include <gl/glut.h>
#include "GLES2/gl2.h"

static int bmpImageLoad(char* filename, sIMGBMP* bmpTex)
{
	int ret = 0;
	uchar bmdHead[64] = "";
	FILE * fp =NULL;
	uchar* p = NULL;
	uint width = 0, height =0,dataOffset = 0,bitCount = 0, dataLen = 0;
	if (NULL == filename || bmpTex == NULL)
	{
		return -1;
	}

	fp = fopen((const char *)filename,"rb");
	if (NULL==fp)
	{
		return -2;
	}

	ret = fread(bmdHead,1,54,fp);
	if (54 != ret)
	{
		fclose(fp);
		fp = NULL;
		return -3;
	}

	if ((0x42 != bmdHead[0]) || (0x4D != bmdHead[1]))
	{
		fclose(fp);
		fp = NULL;
		return -4;
	}

	p = &(bmdHead[10]);
	dataOffset = *((uint*)p);
	p=&(bmdHead[18]);
	width = *((uint*)p);
	p=&(bmdHead[22]);
	height = *((uint*)p);
	p=&(bmdHead[28]);
	bitCount = *((uint*)p);

	dataLen = width*height*bitCount/8;
	if (NULL==bmpTex->data)
	{
		bmpTex->data=(uchar*)malloc(dataLen*sizeof(uchar));
	}

	if (NULL==bmpTex->data)
	{
		fclose(fp);
		fp=NULL;
		return -5;
	}

	fseek(fp,dataOffset,SEEK_SET);
	if (dataLen!=fread(bmpTex->data,1,dataLen,fp))
	{
		free(bmpTex->data);
		bmpTex->data=NULL;
		return -6;
	}

	fclose(fp);
	fp = NULL;
	bmpTex->width=width;
	bmpTex->height=height;
	bmpTex->bit=bitCount;

	return 0;
}


static int bmpTextureCreate(sIMGBMP* bmpTex,int *ptexID)
{
	GLint ret =0;
	GLenum format;
	GLenum type;
	GLint components;
	int textID = 0;

	if (bmpTex == NULL)
	{
		return -1;
	}

	if (bmpTex->data == NULL )
	{
		return -2;
	}

	type = GL_UNSIGNED_BYTE;

	switch (bmpTex->bit)
	{
	case 24:
		format = GL_RGB;//GL_BGR_EXT;
		components = GL_RGB;
		break;
	case 32:
		format = GL_RGBA;//GL_BGRA_EXT;
		components = GL_RGBA;
		break;
	default:
		return -3;
		break;
	}

	// create new texture object
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1,(GLuint*)&(textID));
	if (0==textID)
	{
		return -4;
	}
	glBindTexture(GL_TEXTURE_2D,textID);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	{
		//test code
		GLint maxsize = 0;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE,&maxsize);
		printf("[mqx test] the max size of the texture current OpenGL surpport is : %d",maxsize);
	}

	glTexImage2D(GL_TEXTURE_2D,0,components,bmpTex->width,bmpTex->height,0,format,type,bmpTex->data);

	//test code
	ret = glGetError();
	if (GL_NO_ERROR != ret)
	{
		printf("[mqx test] create 2d texture error, and the error code = 0x%x",ret);
	}

	free(bmpTex->data);
	bmpTex->data = NULL;
	*ptexID = textID;
	return 0;
}

int createBmpTexture(char* filename,int *pTexId)
{
	sIMGBMP bmpImg;
	int ret =-1,textId=0;
	memset((void*)(&bmpImg),0,sizeof(sIMGBMP));
	ret = bmpImageLoad(filename,&bmpImg);
	if (ret !=0)
	{
		printf("[mqx test] texture file load fail!\n");
		return -1;
	}
	ret = bmpTextureCreate(&bmpImg,&textId);
	if (0!=ret)
	{
		printf("[mqx test] create bmp texture fail!\n");
		return -2;
	}else
	{
		*pTexId = textId;
	}
	return 0;
}