#pragma once
#include "gz.h"
#define _USE_MATH_DEFINES


void CalXiw(GzMatrix Xiw,GzCoord lookat, GzCoord position, GzCoord worldup);
void CalXpi(GzMatrix Xpi, float FOV);
void CalXsp(GzMatrix Xsp, int xres, int yres);

void MatrixMultCoord(GzMatrix u, GzCoord v);
void MatrixMult(GzMatrix u, GzMatrix v, GzMatrix result);

void GzCoordSet(GzCoord p, float xp, float yp, float zp);
float Coordnormalize(GzCoord p);
void CoordPerMulti(GzCoord v1, GzCoord v2, GzCoord result);
float CoordDotMult(GzCoord v1, GzCoord v2);
void CoordMult(GzCoord v, float c);
void CoordMinus(GzCoord src, GzCoord dest, GzCoord result);
void CoordPlus(GzCoord v1, GzCoord v2, GzCoord result);