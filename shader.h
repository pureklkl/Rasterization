#pragma once
#include "Gz.h"
#include "TriPlanar.h"
#include "transform.h"
#include "help.h"
#define SHADER_LIGHT_MAX 10

class Shader
{
public:
	Shader();
	~Shader();

	void setInterpStyle(int interpStyle) {
		this->interpStyle = interpStyle;
	}
	int getInterpStyle() {
		return this->interpStyle;
	}
	void setKs(GzColor ks) {
		memcpy(this->ks, ks, 3 * sizeof(float));
	}
	void setKd(GzColor kd) {
		memcpy(this->kd, kd, 3 * sizeof(float));
	}
	void setKa(GzColor ka) {
		memcpy(this->ka, ka, 3 * sizeof(float));
	}
	void setKt(GzColor kt) {
		memcpy(this->ka, kt, 3 * sizeof(float));
	}
	void setSpecPower(float specpower) {
		this->specpower = specpower;
	}
	void setSLight(GzLight* sLight) {
		if (sLights[sLightnum] == NULL)
			sLights[sLightnum] = (GzLight *)malloc(sizeof(GzLight));
		*(sLights[sLightnum++]) = *sLight;//
	}
	void setALight(GzLight* aLight){
		*(this->aLight) = *aLight;
	}
	
	//calculate Ka*la, record planar color
	//initialize gColor or pNorm interplation planar according to interpolation type
	void init(GzCoord vertices[], GzCoord norm[], GzColor planarColor);

	void getColor(GzCoord position, GzColor color) {
		switch (interpStyle){
		case GZ_FLAT: drawNon(color); break;
		case GZ_COLOR: drawG(position, color); break;
		case GZ_NORMALS: drawP(position, color); break;
		default:
			break;
		}
		
	}

	void setIFTex(boolean drawTex) {
		this->drawTex = drawTex;
	}

	boolean getIFTex() {
		return this->drawTex;
	}

private:
	
	void drawP(GzCoord position, GzColor result) {
		GzCoord norm = { (interp[X])->evaluate(position[X],position[Y]),
			(interp[Y])->evaluate(position[X],position[Y]),
			(interp[Z])->evaluate(position[X],position[Y]) };
		Coordnormalize(norm);
		if (drawTex)
			recalAll(norm, result);
		else
			calAll(norm, result);
	};

	void drawG(GzCoord position, GzColor result) {
		result[RED] = (interp[RED])->evaluate(position[X], position[Y]);
		result[GREEN] = (interp[GREEN])->evaluate(position[X], position[Y]);
		result[BLUE] = (interp[BLUE])->evaluate(position[X], position[Y]);
		if (drawTex) {
			result[RED] *= ka[RED];
			result[BLUE] *= ka[BLUE];
			result[GREEN] *= ka[GREEN];
		}
	};

	void drawNon(GzColor result) {
		memcpy(result, planarColor, sizeof(float) * 3);
	};

	//norm should be normalized
	//test N*E, N*L if both negative, filp N. if different sign, skip

	//just return ka*aLight, only compute when initialized by a triangle
	void calA(GzColor result) { 
		CoordPerMulti(ka, aLight->color, result); 
	};
	//kd * sum(kd*(norm*sLight))
	void calD(GzCoord norm, GzColor result) {
		GzColor c;
		memset(result, 0, sizeof(float) * 3);
		for (int i = 0; i < sLightnum; i++) {
			float nl = validNorm(norm, sLights[i]);
			if (nl != 0.f) {
				memcpy(c, sLights[i]->color, sizeof(float) * 3);
				CoordMult(c, nl);
				CoordPerMulti(c, kd, c);
				CoordPlus(result, c, result);
			}
		}
	};
	//ks * sum(ks*(R*E))
	//TODO REMOVE re-calculation of nl(already done in D)
	void calS(GzCoord norm, GzColor result) {
		GzColor c;
		memset(result, 0, sizeof(float) * 3);
		for (int i = 0; i < sLightnum; i++) {
			float nl= validNorm(norm, sLights[i]);
			if (nl != 0.f) {
				GzCoord R;
				calR(norm, sLights[i], nl, R);
				float re= pow(CoordDotMult(R, E), specpower);
				memcpy(c, sLights[i]->color, sizeof(float) * 3);
				CoordMult(c, re);
				CoordPerMulti(c, ks, c);
				CoordPlus(result, c, result);
			}
		}
	};
	// 2(N*L)*N-L
	void calR(GzCoord norm, GzLight* light, float nl, GzColor result) {
		memcpy(result, norm, sizeof(float)*3);
		CoordMult(result, nl*2);
		CoordMinus(light->direction, result, result);
	};

	void calAll(GzCoord norm, GzColor result) {
		memset(result, 0, sizeof(float) * 3);
		CoordPlus(result, aColor, result);
		calD(norm, dColor);
		CoordPlus(result, dColor, result);
		calS(norm, sColor);
		CoordPlus(result, sColor, result);

	};

	//refresh ambient color for texture
	//by set ka/kd, other computation remains unchanged.
	void recalAll(GzCoord norm, GzColor result) {
		memset(result, 0, sizeof(float) * 3);
		calA(aColor);
		CoordPlus(result, aColor, result);
		calD(norm, dColor);
		CoordPlus(result, dColor, result);
		calS(norm, sColor);
		CoordPlus(result, sColor, result);

	};

	float validNorm(GzCoord norm, GzLight* light);

	void generalInterp(GzCoord vertices[], GzCoord attr[]);

	boolean drawTex;

	int interpStyle;
	GzColor ks;//specular coefficient
	GzColor kd;//diffuse coefficient
	GzColor ka;//ambient coefficient
	float specpower;
	GzLight* sLights[SHADER_LIGHT_MAX];//dirctional light
	int sLightnum;
	GzLight* aLight;//ambient light

	GzColor aColor, dColor, sColor;//record ambient color
	GzCoord E = { 0, 0, -1 };//L, E are in image space, R is computed, N is given
	GzColor planarColor;//record the planar color for non shading
	TriPlanar* interp[3];//color/norm interpolation planar for gouraud shading/phone shading, rgb/xyz
};

