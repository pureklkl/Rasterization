/* 
  disp.h -- cs580 HW1 include file for Display
*/
#pragma once
#include	"gz.h"

/* define general display pixel-type */
#ifndef GZ_PIXEL
typedef	struct {
  GzIntensity    red;	
  GzIntensity    green;
  GzIntensity    blue;
  GzIntensity    alpha;
  GzDepth	 z;
} GzPixel;
const int C_NUM = 3;
const int GzDefaultR = (123 << 4);
const int GzDefaultG = (234 << 4);
const int GzDefaultB = (178 << 4);
static const GzIntensity MAXRGBA = 4095;
#define GZ_PIXEL
#endif;

 
/* define a display type */
#ifndef GZ_DISPLAY
typedef struct {
  unsigned short	xres;
  unsigned short	yres;
  GzPixel		*fbuf;		/* frame buffer array */
} GzDisplay;
#define GZ_DISPLAY
#endif;

/* define bounds on display size in case of error */
#define	MAXXRES	1024	
#define	MAXYRES	1024

// simplify display fbuf indexing 
// (optional - use this or define your own indexing method) 
#define	ARRAY(x,y)	(x+(y*display->xres))	


// Function declarations
int GzNewFrameBuffer(char** framebuffer, int width, int height);
int GzNewDisplay(GzDisplay **display, int xRes, int yRes);
int GzFreeDisplay(GzDisplay	*display);
int GzGetDisplayParams(GzDisplay *display, int *xRes, int *yRes);
int GzInitDisplay(GzDisplay	*display);
int GzPutDisplay(GzDisplay	*display, int i, int j, 
		GzIntensity r, GzIntensity g, GzIntensity b, GzIntensity a, GzDepth z);
int GzGetDisplay(GzDisplay *display, int i, int j, 
		GzIntensity *r, GzIntensity *g, GzIntensity *b, GzIntensity *a, GzDepth	*z);
int GzFlushDisplay2File(FILE* outfile, GzDisplay *display);
int GzFlushDisplay2FrameBuffer(char* framebuffer, GzDisplay* display);

void GzDirectPut(GzDisplay *display, int i, int j, GzIntensity r, GzIntensity g, GzIntensity b, GzIntensity a, GzDepth z);

#ifdef _DEBUG
extern int rendnum;
#endif