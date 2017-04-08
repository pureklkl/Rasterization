/*   CS580 HW1 display functions to be completed   */

#include   "stdafx.h"  
#include	"gz.h"
#include	"disp.h"
#include	"help.h"

int GzNewFrameBuffer(char** framebuffer, int width, int height)
{
/* HW1.1 create a framebuffer for MS Windows display:
 -- allocate memory for framebuffer : 3 bytes(b, g, r) x width x height
 -- pass back pointer 
 */
	*framebuffer = (char *)malloc(C_NUM*width*height);
	if (*framebuffer != NULL)
		return GZ_SUCCESS;
	else
		return GZ_FAILURE;
}

int GzNewDisplay(GzDisplay	**display, int xRes, int yRes)
{
/* HW1.2 create a display:
  -- allocate memory for indicated resolution
  -- pass back pointer to GzDisplay object in display
*/
	if (xRes < 0 || yRes < 0)
		return GZ_FAILURE;
	
	*display = new GzDisplay();
	if (*display == NULL)
		return GZ_FAILURE;
	(*display)->fbuf = (GzPixel *)malloc(xRes*yRes * sizeof(GzPixel));
	if (*display == NULL)
		return GZ_FAILURE;
	(*display)->xres = xRes;
	(*display)->yres = yRes;
	return GZ_SUCCESS;
}


int GzFreeDisplay(GzDisplay	*display)
{
	/* HW1.3 clean up, free memory */
		//free will not fail, but 
		/* if the argument does not match a pointer earlier returned by the calloc, malloc, or realloc function, or if the space has been deallocated by a call to free or realloc, the behavior is undefined.*/
	if (display != NULL){
		if (display->fbuf != NULL) {
			free(display->fbuf);
		}
		free(display);
	}
	
	return GZ_SUCCESS;
}


int GzGetDisplayParams(GzDisplay *display, int *xRes, int *yRes)
{
/* HW1.4 pass back values for a display */
	*xRes = display->xres;
	*yRes = display->yres;
	return GZ_SUCCESS;
}
#ifdef _DEBUG
int rendnum = 0;
#endif // _DEBUG
int GzInitDisplay(GzDisplay	*display)
{
/* HW1.5 set everything to some default values - start a new frame */
#ifdef _DEBUG
	rendnum = 0;
#endif
	for(int x=0;x<display->xres;x++)
		for (int y = 0; y < display->yres; y++) {
			GzDirectPut(display, x, y, GzDefaultR, GzDefaultG, GzDefaultB, 1, INT_MAX);
		}
	return GZ_SUCCESS;
}

void GzDirectPut(GzDisplay *display, int i, int j, GzIntensity r, GzIntensity g, GzIntensity b, GzIntensity a, GzDepth z) {
	(display->fbuf + ARRAY(i, j))->blue = b;
	(display->fbuf + ARRAY(i, j))->green = g;
	(display->fbuf + ARRAY(i, j))->red = r;
	(display->fbuf + ARRAY(i, j))->alpha = a;
	(display->fbuf + ARRAY(i, j))->z = z;
}

int GzPutDisplay(GzDisplay *display, int i, int j, GzIntensity r, GzIntensity g, GzIntensity b, GzIntensity a, GzDepth z)
{
/* HW1.6 write pixel values into the display */
	
	clampVal<int>(&i, 0, display->xres-1);
	clampVal<int>(&j, 0, display->yres-1);

	clampVal<short>(&r, 0, MAXRGBA);
	clampVal<short>(&g, 0, MAXRGBA);
	clampVal<short>(&b, 0, MAXRGBA);
	clampVal<short>(&a, 0, MAXRGBA);
	//clampVal(&z, 0, ??);

	GzDirectPut(display, i, j, r, g, b, a, z);

	return GZ_SUCCESS;
}


int GzGetDisplay(GzDisplay *display, int i, int j, GzIntensity *r, GzIntensity *g, GzIntensity *b, GzIntensity *a, GzDepth *z)
{
/* HW1.7 pass back a pixel value to the display */

	clampVal<int>(&i, 0, display->xres-1);
	clampVal<int>(&j, 0, display->yres-1);

	i = display->xres;
	j = display->yres;
	*r = (display->fbuf + ARRAY(i, j))->red;
	*g = (display->fbuf + ARRAY(i, j))->green;
	*b = (display->fbuf + ARRAY(i, j))->blue;
	*a = (display->fbuf + ARRAY(i, j))->alpha;
	*z = (display->fbuf + ARRAY(i, j))->z;
	return GZ_SUCCESS;
}


int GzFlushDisplay2File(FILE* outfile, GzDisplay *display)
{

/* HW1.8 write pixels to ppm file -- "P6 %d %d 255\r" */
	//we write it line by line:)
	char* xBuffer = new char[display->xres*C_NUM];
	fprintf(outfile, "P6\n%d %d\n255\n", display->xres, display->yres);
	for (int y = 0; y < display->yres; y++) {
		char* xp = xBuffer;
		for (int x = 0; x < display->xres; x++) {
			*xp= (((display->fbuf + ARRAY(x, y))->red) >> 4);
			xp++;
			*xp = (((display->fbuf + ARRAY(x, y))->green) >> 4);
			xp++;
			*xp = (((display->fbuf + ARRAY(x, y))->blue) >> 4);
			xp++;
		}
		fwrite(xBuffer, sizeof(char), display->xres*C_NUM, outfile);
	}
	free(xBuffer);
	return GZ_SUCCESS;
}

int GzFlushDisplay2FrameBuffer(char* framebuffer, GzDisplay *display)
{

/* HW1.9 write pixels to framebuffer: 
	- put the pixels into the frame buffer
	- CAUTION: when storing the pixels into the frame buffer, the order is blue, green, and red 
	- NOT red, green, and blue !!!
*/
	if (framebuffer == NULL||display==NULL||display->fbuf==NULL)
		return GZ_FAILURE;

	//hope the fbuf has right size for xres*yres
	for(int y=0;y<display->yres;y++)
		for (int x = 0; x < display->xres; x++) {
			*framebuffer = (((display->fbuf + ARRAY(x, y))->blue)>>4);
			framebuffer++;
			*framebuffer = (((display->fbuf + ARRAY(x, y))->green) >> 4);
			framebuffer++;
			*framebuffer = (((display->fbuf + ARRAY(x, y))->red) >> 4);
			framebuffer++;
		}
#ifdef _DEBUG
	char d[256];
	sprintf(d, "Triangle Renderd: %d\n", rendnum);
	OutputDebugString(_T(d));
#endif // _DEBUG
	return GZ_SUCCESS;
}