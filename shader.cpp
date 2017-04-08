#include "stdafx.h"
#include "shader.h"


Shader::Shader()
{
	for (int i = 0; i < SHADER_LIGHT_MAX; i++)
		sLights[i] = NULL;
	for (int i = 0; i < 3; i++)
		interp[i] = NULL;
	sLightnum = 0;
	aLight = (GzLight *)malloc(sizeof(GzLight));
	drawTex = false;
}


Shader::~Shader()
{
	for (int i = 0; i < sLightnum; i++)
		free(sLights[i]);
	if (aLight != NULL)
		free(aLight);
	for (int i = 0; i < 3; i++) {
		if(interp[i]!=NULL)
			delete interp[i];
	}
}

void Shader::init(GzCoord vertices[], GzCoord norm[], GzColor planarColor) {
	
	if (interpStyle == GZ_FLAT) {
		memcpy(this->planarColor, planarColor, sizeof(float) * 3);
	}
	else {
		for (int i = 0; i < 3; i++)
			if (interp[i] == NULL)
				interp[i] = new TriPlanar();
		calA(aColor);
		if (interpStyle == GZ_COLOR) {
			GzColor verticesColor[3];//

			for (int i = 0; i < 3; i++)
				calAll(norm[i], verticesColor[i]);

			generalInterp(vertices, verticesColor);
		}
		else if (interpStyle == GZ_NORMAL) {
			generalInterp(vertices, norm);
		}
	}
}


// {{x1, y1, r1},{x2, y2, r2},{x3, y3, r3}
//	{x1, y1, g1},{x2, y2, g2},{x3, y3, g3}
//  {x1, y1, b1},{x2, y2, b2},{x3, y3, b3}}
void Shader::generalInterp(GzCoord vertices[], GzCoord attr[]) {
	GzCoord attrPoint[3][3];
	for (int i = 0; i < 3; i++)
		for (int c = 0; c < 3; c++)
			GzCoordSet(attrPoint[c][i], vertices[i][X], vertices[i][Y], attr[i][c]);
	for (int c = 0; c < 3; c++)
		interp[c]->buildFromPoint(attrPoint[c]);
}

float Shader::validNorm(GzCoord norm, GzLight* light) {
	float nl = CoordDotMult(norm, light->direction);
	if (nl*CoordDotMult(norm, E) > 0.f) {
		if (nl < 0.f) {
			nl = -nl;
			CoordMult(norm, -1);
		}
		return nl;
	}
	else {
		return 0.f;
	}
}