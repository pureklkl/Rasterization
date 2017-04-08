/* Texture functions for cs580 GzLib	*/
#include    "stdafx.h" 
#include	"stdio.h"
#include	"Gz.h"
#include	"help.h"
#include	"transform.h"
GzColor	*image=NULL;
int xs, ys;
int reset = 1;

float bilinearInterpolation(float ui, float vi, int C);

/* Image texture function */
int tex_fun(float u, float v, GzColor color)
{
  unsigned char		pixel[3];
  unsigned char     dummy;
  char  		foo[8];
  int   		i, j;
  FILE			*fd;

  if (reset) {          /* open and load texture file */
    fd = fopen ("texture", "rb");
    if (fd == NULL) {
      fprintf (stderr, "texture file not found\n");
      exit(-1);
    }
    fscanf (fd, "%s %d %d %c", foo, &xs, &ys, &dummy);
    image = (GzColor*)malloc(sizeof(GzColor)*(xs+1)*(ys+1));
    if (image == NULL) {
      fprintf (stderr, "malloc for texture image failed\n");
      exit(-1);
    }

    for (i = 0; i < xs*ys; i++) {	/* create array of GzColor values */
      fread(pixel, sizeof(pixel), 1, fd);
      image[i][RED] = (float)((int)pixel[RED]) * (1.0f / 255.0f);
      image[i][GREEN] = (float)((int)pixel[GREEN]) * (1.0f / 255.0f);
      image[i][BLUE] = (float)((int)pixel[BLUE]) * (1.0f / 255.0f);
      }

    reset = 0;          /* init is done */
	fclose(fd);
  }

/* bounds-test u,v to make sure nothing will overflow image array bounds */
/* determine texture cell corner values and perform bilinear interpolation */
/* set color to interpolated GzColor value and return */
  float ui = u*xs, vi = v*ys;
  clampVal(&ui, 0.f, (float)(xs-1));
  clampVal(&vi, 0.f, (float)(ys-1));

  //Bilinear interpolation
  color[RED] = bilinearInterpolation(ui, vi, RED);
  color[GREEN] = bilinearInterpolation(ui, vi, GREEN);
  color[BLUE] = bilinearInterpolation(ui, vi, BLUE);

  return GZ_SUCCESS;
}

/* Procedural texture function */
//Voronoi...
#define REGION_NUM (20)
float calEc(float x1, float y1, float x2, float y2) {
	return pow(x1 - x2, 2) + pow(y1 - y2, 2);
}
int sptex_fun(float u, float v, GzColor color) {
	static float ptx[REGION_NUM], pty[REGION_NUM];
	static GzColor ptc[REGION_NUM];
	if (reset) {
		srand(time(NULL));//comment it if you want the result to remain the same
		xs = 512;
		ys = 512;
		for (int i = 0; i < REGION_NUM; i++) {
			ptx[i] = (float)rand() / RAND_MAX*xs;
			pty[i] = (float)rand() / RAND_MAX*ys;
			ptc[i][RED] = (float)rand() / RAND_MAX;
			ptc[i][BLUE] = (float)rand() / RAND_MAX;
			ptc[i][GREEN] = (float)rand() / RAND_MAX;
		}
		reset = 0;
	}

	float ui = u*xs, vi = v*ys;
	clampVal(&ui, 0.f, (float)xs);
	clampVal(&vi, 0.f, (float)ys);

	int nearI = 0;
	float dis = calEc(ui, vi, ptx[0], pty[0]);
	for (int i = 1; i < REGION_NUM; i++) {
		float newdis = calEc(ui, vi, ptx[i], pty[i]);
		if (dis > newdis) {
			dis = newdis;
			nearI = i;
		}
	}
	memcpy(color, ptc[nearI], 3 * sizeof(float));
	CoordMult(color, pow(0.95,sqrt(dis)/8.f));
	return GZ_SUCCESS;
}

int ptex_fun(float u, float v, GzColor color) {
	float ui = u*256.f, vi = v*256.f;
	clampVal(&ui, 0.f, 256.f);
	clampVal(&vi, 0.f, 256.f);
	unsigned char c = ui*vi;
	float fc = (float)c / 256.f;
	color[RED] = fc;
	color[GREEN] = fc;
	color[BLUE] = 1.0f;
	return GZ_SUCCESS;
}


/*
It does not look good...
#define scaleNum 6
static int scales[scaleNum];

int ptex_fun(float u, float v, GzColor color)
{
	static int maxGray[scaleNum];
	if (reset) {
		xs = 512;
		ys = 512;
		for (int i = 0; i < scaleNum; i++) {
			scales[i] = pow(2, i);
			int xss = xs / scales[i], xsc = xs/scales[i] / 2;
			maxGray[i] = (xss - xsc)*(xss - xsc) * 2;
		}
		reset = 0;
	}
	static float colorInScale[scaleNum];
	float ui = u*xs, vi = v*ys;
	clampVal(&ui, 0.f, (float)xs);
	clampVal(&vi, 0.f, (float)ys);
	for (int i = 0; i < scaleNum; i++) {
		float xss = xs / scales[i], yss = ys / scales[i];
		float cx = floor(ui / xss)*xss + xss / 2,
			  cy = floor(vi / yss)*yss + yss / 2;
		float dis = ((ui - cx)*(ui - cx) + (vi - cy)*(vi - cy))/maxGray[i];
		colorInScale[i] = 1-dis;
	}

	float c = 0.f, weightSum = 0.f;
	for (int i = 0; i < scaleNum; i++) {
		float weight = 1.f / (pow(2,i));
		c += colorInScale[i]*weight;
		weightSum += weight;
	}
	c /= weightSum;
	//c = colorInScale[1];
	color[RED] = c;
	color[BLUE] = 0;
	color[GREEN] = 0;
	return GZ_SUCCESS;
}*/

float bilinearInterpolation(float ui, float vi, int C) {
	float s = ui - (float)floor(ui), t = vi - (float)floor(vi);
	int ax = (int)floor(ui), dx = ax, ay  = (int)floor(vi), by = ay;
	int cx = (int)ceil(ui), bx = cx, cy = (int)ceil(vi), dy = cy;
	float color = s*t*image[cx + cy*xs][C]
		  + (1 - s)*t*image[dx + dy*xs][C]
		  + s*(1 - t)*image[bx + by*xs][C]
	+ (1 - t)*(1 - s)*image[ax + ay*xs][C];
	return color;
}

/* Free texture memory */
int GzFreeTexture()
{
	if(image!=NULL)
		free(image);
	return GZ_SUCCESS;
}

