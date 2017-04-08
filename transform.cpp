#include "StdAfx.h"
#include "transform.h"
#include <memory.h>
#include <intsafe.h>
#include <math.h>

void CoordDivid(GzCoord v, float c) {
	for (int i = 0; i < 3; i++) {
		v[i] /= c;
	}
}

void MatrixMultCoord(GzMatrix u, GzCoord v) {
	float vt[4], v4[4];
	memcpy(vt, v, sizeof(v[0]) * 3);
	vt[3] = 1;
	for (int i = 0; i < 4; i++) {
		v4[i] = 0;
		for (int j = 0; j < 4; j++) {
			v4[i] += u[i][j] * vt[j];
		}
	}
	CoordDivid(v4, v4[3]);
	memcpy(v, v4, sizeof(v[0]) * 3);
}

void MatrixMult(GzMatrix u, GzMatrix v, GzMatrix result) {
	for (int i = 0; i < 4; i++) 
		for (int j = 0; j < 4; j++) {
			result[i][j] = 0;
			for (int k = 0; k < 4; k++) {
				result[i][j] += u[i][k] * v[k][j];
			}
		}
}

void GzCoordSet(GzCoord p, float xp, float yp, float zp) {
	p[X] = xp;
	p[Y] = yp;
	p[Z] = zp;
}

void CoordPlus(GzCoord v1, GzCoord v2, GzCoord result) {
	for (int i = 0; i < 3; i++)
		result[i] = v1[i] + v2[i];
}

void CoordMinus(GzCoord src, GzCoord dest, GzCoord result) {
	for (int i = 0; i < 3; i++)
		result[i] = dest[i] - src[i];
}

float CoordDotMult(GzCoord v1, GzCoord v2) {
	float result = 0.f;
	for (int i = 0; i < 3; i++) {
		result += v1[i] * v2[i];
	}
	return result;
}
/*
	left hand
	ij=k, jk=i, ki=j;
	ji=-k, kj=-i, ik=-j;
	uxv=(u0i+u1j+u2k)x(v0i+v1j+v2k)
	=u0v1k-u0v2j-u1v0k+u1v2i+u2v0j-u2v1i
	=(u1v2-u2v1)i+(u2v0-u0v2)j+(u0v1-u1v0)k
*/
void CoordXMult(GzCoord u, GzCoord v, GzCoord result) {
	result[X] = u[1] * v[2] - u[2] * v[1];
	result[Y] = u[2] * v[0] - u[0] * v[2];
	result[Z] = u[0] * v[1] - u[1] * v[0];
}

void CoordMult(GzCoord v, float c) {
	for (int i = 0; i < 3; i++) {
		v[i] *= c;
	}
}

float Coordnormalize(GzCoord v) {
	float norm = CoordDotMult(v, v);
	norm = sqrtf(norm);
	CoordDivid(v, norm);
	return norm;
}

void CoordPerMulti(GzCoord v1, GzCoord v2, GzCoord result) {
	for (int i = 0; i < 3; i++) {
		result[i] = v1[i] * v2[i];
	}
}

void orthogonalize(GzCoord v, GzCoord base) {
	GzCoord tmp;
	memcpy(tmp, base, sizeof(base[0]) * 3);
	CoordMult(tmp, CoordDotMult(v, base));
	CoordMinus(tmp, v, v);
}

void CalXiw(GzMatrix Xiw, GzCoord lookat, GzCoord position, GzCoord worldup) {
	memset(Xiw, 0, sizeof(Xiw[0][0]) * 4 * 4);
	
	GzCoord cam_Z;
	CoordMinus(position, lookat, cam_Z);
	Coordnormalize(cam_Z);
	
	GzCoord cam_Y;
	memcpy(cam_Y, worldup, sizeof(worldup[0]) * 3);
	orthogonalize(cam_Y, cam_Z);
	Coordnormalize(cam_Y);

	GzCoord cam_X;
	CoordXMult(cam_Y, cam_Z, cam_X);
	//Coordnormalize(cam_X);

	Xiw[0][0] = cam_X[X]; Xiw[0][1] = cam_X[Y]; Xiw[0][2] = cam_X[Z]; Xiw[0][3] = -CoordDotMult(cam_X, position);
	Xiw[1][0] = cam_Y[X]; Xiw[1][1] = cam_Y[Y]; Xiw[1][2] = cam_Y[Z]; Xiw[1][3] = -CoordDotMult(cam_Y, position);
	Xiw[2][0] = cam_Z[X]; Xiw[2][1] = cam_Z[Y]; Xiw[2][2] = cam_Z[Z]; Xiw[2][3] = -CoordDotMult(cam_Z, position);
	Xiw[3][3] = 1;

}

void CalXpi(GzMatrix Xpi, float FOV) {
	memset(Xpi, 0, sizeof(Xpi[0][0]) * 4 * 4);
	Xpi[0][0] = Xpi[1][1] = Xpi[3][3] = 1;
	Xpi[2][2] = Xpi[3][2] = tan(FOV * M_PI/180.f / 2.f);
}

void CalXsp(GzMatrix Xsp, int xres, int yres) {
	memset(Xsp, 0, sizeof(Xsp[0][0]) * 4 * 4);
	Xsp[0][0] = Xsp[0][3] = xres >> 1;
	Xsp[1][1] = -(yres >> 1);
	Xsp[1][3] = yres >> 1;
	Xsp[2][2] = INT_MAX;
	Xsp[3][3] = 1;
}



