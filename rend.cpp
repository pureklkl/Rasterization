/* CS580 Homework 3 */

#include	"stdafx.h"
#include	"stdio.h"
#include	"math.h"
#include	"Gz.h"
#include	"rend.h"

#include	"LEE.h"
#include	"transform.h"

#include	<math.h>
#include	<memory.h>

GzMatrix unitary = { {1,0,0,0},
					 {0,1,0,0},
					 {0,0,1,0},
					 {0,0,0,1} };

int GzPusImageMatrix(GzRender *render, GzMatrix	matrix);
int GzPushNormMatrix(GzRender *render, GzMatrix	matrix);

int GzRotXMat(float degree, GzMatrix mat)
{
// Create rotate matrix : rotate along x axis
// Pass back the matrix using mat value
	float r = degree*M_PI / 180.f;
	memset(mat, 0, 4 * 4 * sizeof(mat[0][0]));
	mat[0][0] = 1;
	mat[1][1] = cos(r); mat[1][2] = -sin(r);
	mat[2][1] = sin(r); mat[2][2] = cos(r);
	return GZ_SUCCESS;
}


int GzRotYMat(float degree, GzMatrix mat)
{
// Create rotate matrix : rotate along y axis
// Pass back the matrix using mat value
	float r = degree*M_PI / 180.f;
	memset(mat, 0, 4 * 4 * sizeof(mat[0][0]));
	mat[0][0] = cos(r); mat[0][2] = sin(r);
	mat[1][1] = 1;
	mat[2][0] = -sin(r); mat[2][2] = cos(r);
	mat[3][3] = 1;
	return GZ_SUCCESS;
}


int GzRotZMat(float degree, GzMatrix mat)
{
// Create rotate matrix : rotate along z axis
// Pass back the matrix using mat value
	memset(mat, 0, 4 * 4 * sizeof(mat[0][0]));
	float r = degree*M_PI / 180.f;
	mat[0][0] = cos(r); mat[0][1] = -sin(r);
	mat[1][0] = sin(r); mat[1][1] = cos(r);
	mat[2][2] = 1;
	mat[3][3] = 1;
	return GZ_SUCCESS;
}


int GzTrxMat(GzCoord translate, GzMatrix mat)
{
// Create translation matrix
// Pass back the matrix using mat value
	memset(mat, 0, 4 * 4 * sizeof(mat[0][0]));
	mat[0][0] = mat[1][1] = mat[2][2] = mat[3][3] = 1;
	mat[0][3] = translate[0];
	mat[1][3] = translate[1];
	mat[2][3] = translate[2];
	return GZ_SUCCESS;
}


int GzScaleMat(GzCoord scale, GzMatrix mat)
{
// Create scaling matrix
// Pass back the matrix using mat value
	memset(mat, 0, sizeof(mat[0][0]));
	mat[0][0] = scale[0];
	mat[1][1] = scale[1];
	mat[2][2] = scale[2];
	mat[3][3] = 1;
	return GZ_SUCCESS;
}


//----------------------------------------------------------
// Begin main functions

void initRender(GzRender *render) {
	render->camera.FOV = DEFAULT_FOV;
	render->tex_fun = NULL;
	render->filters = NULL;
	GzCoordSet(render->camera.lookat, 0, 0, 0);
	GzCoordSet(render->camera.position, DEFAULT_IM_X, DEFAULT_IM_Y, DEFAULT_IM_Z);
	GzCoordSet(render->camera.worldup, 0, 1, 0);

	CalXsp(render->Xsp, render->display->xres, render->display->yres);
	render->matlevel = 0;
	GzPusImageMatrix(render, render->Xsp);
	GzPushNormMatrix(render, unitary);
	render->matlevel++;
}

int GzNewRender(GzRender **render, GzDisplay	*display)
{
/*  
- malloc a renderer struct 
- setup Xsp and anything only done once 
- save the pointer to display 
- init default camera 
*/ 
	*render = (GzRender *)malloc(sizeof(GzRender));
	if (*render != NULL) {
		(*render)->display = display;
		initRender(*render);
		return GZ_SUCCESS;
	}
	else {
		return GZ_FAILURE;
	}

}


int GzFreeRender(GzRender *render)
{
/* 
-free all renderer resources
*/
	if (render != NULL)
		delete render;
	return GZ_SUCCESS;
}

int GzBeginRender(GzRender *render)
{
/*  
- setup for start of each frame - init frame buffer color,alpha,z
- compute Xiw and projection xform Xpi from camera definition 
- init Ximage - put Xsp at base of stack, push on Xpi and Xiw 
- now stack contains Xsw and app can push model Xforms when needed 
*/ 
	GzInitDisplay(render->display);
	LEE* raster = LEE::getLEE();
	raster->setDisplay(render->display);

	GzMatrix tmp;
	CalXpi(tmp, render->camera.FOV);
	GzPusImageMatrix(render, tmp);
	GzPushNormMatrix(render, unitary);
	render->matlevel++;
	
	CalXiw(tmp, render->camera.lookat, render->camera.position, render->camera.worldup);
	GzPushMatrix(render, tmp);
	return GZ_SUCCESS;
}

int GzPutCamera(GzRender *render, GzCamera *camera)
{
/*
- overwrite renderer camera structure with new camera definition
*/
	memcpy(&render->camera, camera, sizeof(render->camera));
	return GZ_SUCCESS;	
}

#ifdef _DEBUG
void debugMatrix(GzMatrix matrix) {
	char d[256];
	sprintf(d, "%f, %f, %f, %f\n%f, %f, %f, %f\n%f, %f, %f, %f\n%f, %f, %f, %f\n",
								 matrix[0][0], matrix[0][1], matrix[0][2], matrix[0][3],
								 matrix[1][0], matrix[1][1], matrix[1][2], matrix[1][3],
								 matrix[2][0], matrix[2][1], matrix[2][2], matrix[2][3],
								 matrix[3][0], matrix[3][1], matrix[3][2], matrix[3][3]);
	OutputDebugString(_T(d));
}
#endif // _DEBUG

int GzPushMatrix(GzRender *render, GzMatrix	matrix)
{
	if (GzPusImageMatrix(render, matrix)==GZ_SUCCESS) {
		if (GzPushNormMatrix(render, matrix)== GZ_SUCCESS) {
			render->matlevel++;
			return GZ_SUCCESS;
		}
	}
	return GZ_FAILURE;
}
int GzPusImageMatrix(GzRender *render, GzMatrix	matrix) {
	/*
	- push a matrix onto the Ximage stack
	- check for stack overflow
	*/
	if (render->matlevel < MATLEVELS) {
		if (render->matlevel == 0) {
			memcpy(render->Ximage[0], matrix, sizeof(matrix[0][0]) * 4 * 4);
		}
		else {
			MatrixMult(render->Ximage[render->matlevel - 1], matrix, render->Ximage[render->matlevel]);
		}
#ifdef _DEBUG
		OutputDebugString(_T("push matrix:\n"));
		debugMatrix(matrix);
		OutputDebugString(_T("stack top:\n"));
		debugMatrix(render->Ximage[render->matlevel]);
#endif
		return GZ_SUCCESS;
	}
	else {
		return GZ_FAILURE;
	}
}
int GzPushNormMatrix(GzRender *render, GzMatrix	matrix)
{
	/*
	- push a matrix onto the Xnorm stack
	- check for stack overflow
	*/
	if (render->matlevel < MATLEVELS) {
		GzMatrix Q;
		memcpy(Q, matrix, sizeof(matrix[0][0]) * 4 * 4);
		Q[0][3] = 0; Q[1][3] = 0; Q[2][3] = 0;//remove translate
		float scale = sqrt(pow(Q[0][0], 2) + pow(Q[0][1], 2) + pow(Q[0][2], 2));//normalize
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				Q[i][j] /= scale;
		//TODO [Q^-1]^t non uniform scale
		if (render->matlevel == 0) {
			memcpy(render->Xnorm[0], Q, sizeof(Q[0]) * 4 * 4);
		}
		else {
			MatrixMult(render->Xnorm[render->matlevel - 1], Q, render->Xnorm[render->matlevel]);
		}
#ifdef _DEBUG
		OutputDebugString(_T("push matrix to norm:\n"));
		debugMatrix(Q);
		OutputDebugString(_T("norm stack top:\n"));
		debugMatrix(render->Xnorm[render->matlevel]);
#endif
		return GZ_SUCCESS;
	}
	else {
		return GZ_FAILURE;
	}
}

int GzPopMatrix(GzRender *render)
{
/*
- pop a matrix off the Ximage stack
- check for stack underflow
*/
	if (render->matlevel > 0) {
		render->matlevel--;
		return GZ_SUCCESS;
	}
	else {
		return GZ_FAILURE;
	}
}


int GzPutAttribute(GzRender	*render, int numAttributes, GzToken	*nameList, 
	GzPointer	*valueList) /* void** valuelist */
{
/*
- set renderer attribute states (e.g.: GZ_RGB_COLOR default color)
- later set shaders, interpolaters, texture maps, and lights
*/
	LEE* raster = LEE::getLEE();
	float* gc;
	for (int i = 0; i < numAttributes; i++) {
		switch (nameList[i])
		{
		case GZ_RGB_COLOR:
			gc = (float *)(valueList[i]);
			render->flatcolor[0] = *gc;
			raster->setRed(*gc);
			gc++;
			render->flatcolor[1] = *gc;
			raster->setGreen(*gc);
			gc++;
			render->flatcolor[2] = *gc;
			raster->setBlue(*gc);
			break;
		case GZ_INTERPOLATE:
			render->interp_mode = *(int *)valueList[i];
			//raster->getShader()->setInterpStyle(*(int *)valueList[i]);
			break;
		case GZ_AMBIENT_COEFFICIENT:
			memcpy(render->Ka, (float *)(valueList[i]), 3 * sizeof(float));
			//raster->getShader()->setKa((float *)(valueList[i]));
			break;
		case GZ_SPECULAR_COEFFICIENT:
			memcpy(render->Ks, (float *)(valueList[i]), 3 * sizeof(float));
			//raster->getShader()->setKs((float *)(valueList[i]));
			break;
		case GZ_DIFFUSE_COEFFICIENT:
			memcpy(render->Kd, (float *)(valueList[i]), 3 * sizeof(float));
			//raster->getShader()->setKd((float *)(valueList[i]));
			break;
		case GZ_DIRECTIONAL_LIGHT:
			raster->getShader()->setSLight((GzLight *)(valueList[i]));
			break;
		case GZ_AMBIENT_LIGHT:
			raster->getShader()->setALight((GzLight *)(valueList[i]));
			break;
		case GZ_DISTRIBUTION_COEFFICIENT:
			raster->getShader()->setSpecPower(*(float *)valueList[i]);
			break;
		case GZ_TEXTURE_MAP:
			if ((GzTexture)valueList[i] != NULL) {
				raster->getShader()->setIFTex(true);
				raster->setTex((GzTexture)valueList[i]);
			}
			break;
		case GZ_ANTIALISING_FILTER:
			render->filters = (AAFilter *)(valueList[i]);
			raster->setAA(render->display);
			raster->getAA()->setFilters((AAFilter *)(valueList[i]));
			break;
		default:
			break;
		}
	}
	return GZ_SUCCESS;
}
boolean rectangleOverlapped(int l1, int u1, int r1, int b1,
						    int l2, int u2, int r2, int b2) {
	if (l1 > r2 || l2 > r1) {
		return false;
	}
	if (b1 < u2 || b2 < u1) {
		return false;
	}
	return true;
}
boolean valid(GzCoord triVertices[], int xres, int yres) {
	for (int i = 0; i < 3; i++) {
		if (triVertices[i][2] < 0)
			return false;
	}
/*
	It seems difficult to find an efficent algorithm to determin whether a triangle and a rectangle overlapped,
	so we just test the bounding box.
	TODO : put it into LEE
*/
	LEE* raster = LEE::getLEE();
	raster->boundBox(triVertices);
	int xmin, xmax, ymin, ymax;
	raster->getBoundBox(xmin, ymin, xmax, ymax);
	return rectangleOverlapped(0, 0, xres, yres, xmin, ymin, xmax, ymax);
}

int GzPutTriangle(GzRender	*render, int numParts, GzToken *nameList, GzPointer	*valueList)
/* numParts : how many names and values */
{
/*  
- pass in a triangle description with tokens and values corresponding to 
      GZ_POSITION:3 vert positions in model space 
- Xform positions of verts using matrix on top of stack 
- Clip - just discard any triangle with any vert(s) behind view plane 
       - optional: test for triangles with all three verts off-screen (trivial frustum cull)
- invoke triangle rasterizer  
*/ 
	LEE* raster = LEE::getLEE();
	GzCoord * position = NULL;
	GzCoord * normal = NULL;
	GzTextureIndex * tex = NULL;
	for (int i = 0; i < numParts; i++) {
		switch (nameList[i])
		{
		case GZ_POSITION: position = (GzCoord *)valueList[i];
			break;
		case GZ_NORMAL:	normal = (GzCoord *)valueList[i]; 
			break;
		case GZ_TEXTURE_INDEX: tex = (GzTextureIndex *)valueList[i];
			break;
		case GZ_NULL_TOKEN:
			break;
		default:
			break;
		}
	}

	if (position != NULL) {
		
		MatrixMultCoord(render->Ximage[render->matlevel - 1], position[0]);
		MatrixMultCoord(render->Ximage[render->matlevel - 1], position[1]);
		MatrixMultCoord(render->Ximage[render->matlevel - 1], position[2]);

		int sample = 1;
		bool enableAA = false;
		GzCoord positionCopy[3];
		if (render->filters != NULL) {
			sample = AAKERNEL_SIZE;
			enableAA = true;
			memcpy(positionCopy, position, 3 * 3 * sizeof(position[0][0]));
		}
		if (normal != NULL) {
			MatrixMultCoord(render->Xnorm[render->matlevel - 1], normal[0]);
			MatrixMultCoord(render->Xnorm[render->matlevel - 1], normal[1]);
			MatrixMultCoord(render->Xnorm[render->matlevel - 1], normal[2]);
		}
		for (int i = 0; i < sample; i++) {

			if (enableAA) {
				memcpy(position, positionCopy, 3 * 3 * sizeof(position[0][0]));
				MatrixMultCoord(*raster->getAA()->getTxMat(i), position[0]);
				MatrixMultCoord(*raster->getAA()->getTxMat(i), position[1]);
				MatrixMultCoord(*raster->getAA()->getTxMat(i), position[2]);
				raster->setDisplay(raster->getAA()->getBuffer(i));
			}

			if (valid(position, render->display->xres, render->display->yres)) {
				if (normal != NULL) {
					raster->getShader()->setInterpStyle(render->interp_mode);
					if (raster->getShader()->getIFTex() && tex != NULL) {
						static GzColor base = { 1.f, 1.f, 1.f };
						if (render->interp_mode == GZ_NORMALS) {
							raster->getShader()->setKs(render->Ks);
						}
						else {
							raster->getShader()->setKs(base);
							raster->getShader()->setKd(base);
							raster->getShader()->setKa(base);
						}
						raster->draw(position, normal, tex);
					}
					else {
						raster->getShader()->setKa(render->Ka);
						raster->getShader()->setKd(render->Kd);
						raster->getShader()->setKs(render->Ks);
						raster->draw(position, normal);
					}
				}
#ifdef _DEBUG
				rendnum++;
#endif
			}
			else {
				raster->draw(position);
			}
		}
	}

	return GZ_SUCCESS;
}

