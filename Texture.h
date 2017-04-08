#pragma once
#include "TriPlanar.h"
#include "Gz.h"
#include "transform.h"
class Texture
{
public:
	Texture(GzTexture texFun);
	~Texture();
	
	//Vz = Vzs/(Zmax-Vzs) used for perspective correction
	void buildTexTri(GzCoord vertices[], GzTextureIndex t[]) {
		GzCoord attrPoint[2][3];
		for (int i = 0; i < 3; i++)
			for (int c = 0; c < 2; c++) {
				float Vz = calVz(vertices[i][Z]);
				GzCoordSet(attrPoint[c][i], vertices[i][X], vertices[i][Y], t[i][c] / (Vz + 1));
			}
		for (int c = 0; c < 2; c++)
			texPlanar[c]->buildFromPoint(attrPoint[c]);
	}

	void getuv(int x, int y, float Vz, GzTextureIndex t) {
		t[U] = texPlanar[U]->evaluate(x, y)*(Vz + 1);
		t[V] = texPlanar[V]->evaluate(x, y)*(Vz + 1);
	}

	void getColor(int x, int y, int z, GzColor c) {
		GzTextureIndex t;
		getuv(x, y, calVz(z), t);
		texFun(t[U], t[V], c);
	}

	void setTexFun(GzTexture texFun) {
		this->texFun = texFun;
	}

private:
	float calVz(int z) {
		return (float)((double)z / (double)(INT_MAX - z));
	}
	TriPlanar* texPlanar[2];//u,v
	GzTexture texFun;
};

