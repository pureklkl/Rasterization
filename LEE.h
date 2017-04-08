#pragma once
#include "gz.h"
#include "disp.h"
#include "LineEq.h"
#include "TriPlanar.h"
#include "help.h"
#include "shader.h"
#include "Texture.h"
#include "AntiAliasing.h"

class LEE
{
public:
	static LEE* getLEE() {
		static LEE leeInstance;
		return &leeInstance;
	};

	void setDisplay(GzDisplay *display) {
		this->display = display;
		this->xres = display->xres;
		this->yres = display->yres;
#ifdef _DEBUG
		if(drawed==NULL)
			drawed = (bool *)malloc(this->xres*this->yres * sizeof(bool));
#endif
	}

	void setRed(float cr) {
		rgb[RED] = cr;
	}
	void setBlue(float cb) {
		rgb[BLUE] = cb;
	}
	void setGreen(float cg) {
		rgb[GREEN] = cg;
	}

	void getBoundBox(int& l, int& u, int& r, int& b) {
		l = this->l;
		u = this->u;
		r = this->r;
		b = this->b;
	}

	Shader* getShader() {
		return shader;
	}

	void setAA(GzDisplay* display) {
		if (aa != NULL)
			delete aa;
		aa = new AntiAliasing(display);
	}

	AntiAliasing* getAA() {
		return aa;
	}

	void setTex(GzTexture texFun) {
		if (texture != NULL)
			delete texture;
		texture = new Texture(texFun);
	}
	// set vertices
	// adjust shared vertices
	// sort vertices
	// calculate ABC for line equation
	//
	// 
	//input: triangle vertices
	//output: area of the triangle
	QType buildTriangle(GzCoord triVertices[]);

	// judge if given pt from bounding box should be drawn
	// expression evaluation & z buffer
	//
	// input: coordinate in screen 
	void draw(GzCoord triVertices[], GzCoord normal[], GzTextureIndex t[]);
	void draw(GzCoord triVertices[], GzCoord normal[]);
	void draw(GzCoord triVertices[]);

	//find boundbox
	void boundBox(GzCoord triVertices[]);


private:
	
	
	bool coveredEdge(QType A, QType B);
	void putPixel(int x, int y, int z, bool& drawn);
	void putShaderPixel(int i, int j, int z, bool& drawn);
	void putTexPixel(int i, int j, int z, bool& drawn);

	void putBuffer(int i, int j, short color[]);
	
	
	void drawPixel(GzCoord triVertices[], QType area, void(LEE::*pf)(int i, int j, int z, bool& drawn));
	void drawFlatPixel(int z, void(LEE::*pf)(int i, int j, int z, bool& drawn));
	//void drawDepth(GzCoord triVertices[]);

	LEE();
	~LEE();
	
	
	GzCoord * _triVertices;
	GzDisplay *display;
	int xres, yres;//copy from disp
	int l, r, u, b;//bounding box left, right, up, bottom
	GzColor rgb;//color r, g, b;
	short srgb[3];//color r, g, b in short;
	LineEq<QType>* edges[3];
	TriPlanar* triPl;//planar for z interpolation
	Shader* shader;
	Texture* texture;
	AntiAliasing* aa;

#ifdef _DEBUG
	//test duplicated rendering 
	bool * drawed;
#endif // DEBUG

};


typedef void(LEE::*pf)(int i, int j, int z, bool& drawn);