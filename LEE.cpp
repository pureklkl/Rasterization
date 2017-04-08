#include "stdafx.h"
#include "LEE.h"

#define CALL_MEMBER_FN(object,ptrToMember)  ((object).*(ptrToMember))

LEE::LEE(){
	for (int i = 0; i < 3; i++) {
		edges[i] = new LineEq<QType>(SCALEFACTOR);
	}
	triPl = new TriPlanar();
	shader = new Shader();
	texture = NULL;
	aa = NULL;
	drawed = NULL;
}


LEE::~LEE(){
	for (int i = 0; i < 3; i++) {
		if (edges[i] != NULL)
			delete edges[i];
	}
	if (triPl != NULL)
		delete triPl;
	if (shader != NULL)
		delete shader;
	if (texture != NULL)
		delete texture;
	if (aa != NULL)
		delete aa;
#ifdef _DEBUG
	if (drawed != NULL)
		free(drawed);
#endif
}

void LEE::boundBox(GzCoord triVertices[]) {
	/*
	find bounding box -> find min/max among 4 points for x/y -> 3 vertices and window
	*/
	float xmin, xmax, ymin, ymax;
	minMax3<float>(triVertices[0][X], triVertices[1][X], triVertices[2][X], xmin, xmax);
	minMax3<float>(triVertices[0][Y], triVertices[1][Y], triVertices[2][Y], ymin, ymax);
	l = (int)(xmin < 0.f ? 0.f : xmin);
	u = (int)(ymin < 0.f ? 0.f : ymin);
	int ixmax = (int)(xmax + 1.f),
		iymax = (int)(ymax + 1.f);
	r = ixmax >= xres ? xres - 1 : ixmax;
	b = iymax >= yres ? yres - 1 : iymax;
}

QType LEE::buildTriangle(GzCoord triVertices[]) {
	// TODO: adjust integer vertices 
	/*
		compute edges
	*/
	edges[0]->calLineEq(triVertices[0], triVertices[1]);
	edges[1]->calLineEq(triVertices[1], triVertices[2]);
	edges[2]->calLineEq(triVertices[2], triVertices[0]);
	/*
		compute area of the triangle, make sure the vertices is given in clockwise
	*/
	QType area = edges[0]->getC() + edges[1]->getC() + edges[2]->getC();
	if (area == 0)
		return area;
	if (area < 0) {
		edges[0]->reverse();
		edges[1]->reverse();
		edges[2]->reverse();
		area = -area;
	}
	boundBox(triVertices);

	return area;
}

bool LEE::coveredEdge(QType A, QType B) {
	if (A > 0)
		return true;
	else if (A == 0 && B > 0)
		return true;
	else
		return false;
}

void LEE::putTexPixel(int i, int j, int z, bool& drawn) {
	drawn = true;

	if ((display->fbuf + ARRAY(i, j))->z >= z) {
#ifdef _DEBUG
		if (*(drawed + ARRAY(i, j)) && (display->fbuf + ARRAY(i, j))->z == z) {
			printf("duplicated rendering detected at x=%d, y=%d", i, j);
		}
		*(drawed + ARRAY(i, j)) = true;
#endif
		(display->fbuf + ARRAY(i, j))->z = z;

		GzCoord position = { i, j, 0 };
		GzColor color;
		texture->getColor(i, j, z, color);
		if (shader->getInterpStyle() == GZ_COLOR)
			shader->setKt(color);
		else if(shader->getInterpStyle() == GZ_NORMALS){
			shader->setKa(color);
			shader->setKd(color);
		}
		shader->getColor(position, color);
		short scolor[3];
		for (int i = 0; i < 3; i++) {
			scolor[i] = ctoi(color[i]);
			clampVal<short>(&scolor[i], 0, MAXRGBA);
		}
		putBuffer(i, j, scolor);

	}
}

void LEE::putShaderPixel(int i, int j, int z, bool& drawn) {
	drawn = true;

	if ((display->fbuf + ARRAY(i, j))->z >= z) {
#ifdef _DEBUG
		if (*(drawed + ARRAY(i, j)) && (display->fbuf + ARRAY(i, j))->z == z) {
			printf("duplicated rendering detected at x=%d, y=%d", i, j);
		}
		*(drawed + ARRAY(i, j)) = true;
#endif
		(display->fbuf + ARRAY(i, j))->z = z;
		
		GzCoord position = { i, j, 0 };
		GzColor color;
		shader->getColor(position, color);
		short scolor[3];
		for (int i = 0; i < 3; i++) {
			scolor[i] = ctoi(color[i]);
			clampVal<short>(&scolor[i], 0, MAXRGBA);
		}
		putBuffer(i, j, scolor);

	}
}

void LEE::putPixel(int i, int j, int z, bool& drawn) {
	drawn = true;

	if ((display->fbuf + ARRAY(i, j))->z >= z) {
#ifdef _DEBUG
		if (*(drawed + ARRAY(i, j)) && (display->fbuf + ARRAY(i, j))->z == z) {
			printf("duplicated rendering detected at x=%d, y=%d", i, j);
		}
		*(drawed + ARRAY(i, j)) = true;
#endif
		(display->fbuf + ARRAY(i, j))->z = z;
		putBuffer(i, j, srgb);

	}
}

void LEE::putBuffer(int i, int j, short color[]) {

	(display->fbuf + ARRAY(i, j))->blue = color[BLUE];
	(display->fbuf + ARRAY(i, j))->green = color[GREEN];
	(display->fbuf + ARRAY(i, j))->red = color[RED];

}

void LEE::draw(GzCoord triVertices[]) {
	QType area = buildTriangle(triVertices);
	if (area == 0)
		return;
	for (int i = 0; i < 3; i++) {
		srgb[i] = ctoi(rgb[i]);
		clampVal<short>(&srgb[i], 0, MAXRGBA);
	}
	drawPixel(triVertices, area, &LEE::putPixel);
}

void LEE::draw(GzCoord triVertices[], GzCoord normal[]) {
	//TODO integer coordinate adjust
	if (shader->getInterpStyle() != GZ_FLAT) {
		QType area = buildTriangle(triVertices);
		if (area==0)
			return;
		shader->init(triVertices, normal, rgb);
		drawPixel(triVertices, area, &LEE::putShaderPixel);
	}
	else {
		draw(triVertices);
	}
	
}

void LEE::draw(GzCoord triVertices[], GzCoord normal[], GzTextureIndex t[]) {
	//TODO integer coordinate adjust
	if (shader->getInterpStyle() != GZ_FLAT && texture != NULL) {
		QType area = buildTriangle(triVertices);
		if (area == 0)
			return;

		shader->init(triVertices, normal, rgb);
		texture->buildTexTri(triVertices, t);
		drawPixel(triVertices, area, &LEE::putTexPixel);
	}
	else if (shader->getInterpStyle() != GZ_FLAT) {
		draw(triVertices, normal);
	}
	else{
		draw(triVertices);
	}
}

void LEE::drawPixel(GzCoord triVertices[], QType area, pf p) {

	if ((int)triVertices[0][Z] == (int)triVertices[1][Z] && (int)triVertices[1][Z] == (int)triVertices[2][Z]) {
		//if it is flat, we don't need z interpolation
		drawFlatPixel((int)triVertices[0][Z], p);
	}
	else {
		triPl->calPlanar(edges, triVertices[0][Z], triVertices[1][Z], triVertices[2][Z], area);

		int x = l, y = u;
		QType A0 = edges[0]->getA(), A1 = edges[1]->getA(), A2 = edges[2]->getA(),
			B0 = edges[0]->getB(), B1 = edges[1]->getB(), B2 = edges[2]->getB(),
			Ap = triPl->getAp(), Bp = triPl->getBp(), Cp = triPl->getCp();

		QType e0 = edges[0]->evaluate(l, u),
			e1 = edges[1]->evaluate(l, u),
			e2 = edges[2]->evaluate(l, u);
		QType	z = triPl->evaluate(l, u);

		for (; y <= b; y++) {
			QType	xe0 = e0,
				xe1 = e1,
				xe2 = e2;
			QType	xz = z;
			bool drawn = false;
			for (x = l; x <= r; x++) {
				if (((LONGLONG)xe0 | (LONGLONG)xe1 | (LONGLONG)xe2) > 0) {
					CALL_MEMBER_FN(*this, p)(x, y, xz, drawn);
				}
				else if (((LONGLONG)xe0 | (LONGLONG)xe1 | (LONGLONG)xe2) < 0) {
					if (drawn) {
						break;
					}
				}
				else if ((LONGLONG)xe0 == 0 && coveredEdge(A0, B0)) {
					CALL_MEMBER_FN(*this, p)(x, y, xz, drawn);
				}
				else if ((LONGLONG)xe1 == 0 && coveredEdge(A1, B1)) {
					CALL_MEMBER_FN(*this, p)(x, y, xz, drawn);
				}
				else if ((LONGLONG)xe2 == 0 && coveredEdge(A2, B2)) {
					CALL_MEMBER_FN(*this, p)(x, y, xz, drawn);
				}
				xe0 += A0;
				xe1 += A1;
				xe2 += A2;
				xz += Ap;

			}

			e0 += B0;
			e1 += B1;
			e2 += B2;
			z += Bp;
		}
	}
}



void LEE::drawFlatPixel(int z, pf p) {
	int x = l, y = u;
	QType	A0 = edges[0]->getA(), A1 = edges[1]->getA(), A2 = edges[2]->getA(),
			B0 = edges[0]->getB(), B1 = edges[1]->getB(), B2 = edges[2]->getB();

	QType	e0 = edges[0]->evaluate(l, u),
			e1 = edges[1]->evaluate(l, u),
			e2 = edges[2]->evaluate(l, u);

	for (; y <= b; y++) {
		QType	xe0 = e0,
				xe1 = e1,
				xe2 = e2;
		bool drawn = false;
		for (x = l; x <= r; x++) {
			if (((LONGLONG)xe0 | (LONGLONG)xe1 | (LONGLONG)xe2) >= 0) {
				CALL_MEMBER_FN(*this, p)(x, y, z, drawn);
			}
			else if (((LONGLONG)xe0 | (LONGLONG)xe1 | (LONGLONG)xe2) < 0) {
				if (drawn) {
					break;
				}
			}
			else if ((LONGLONG)xe0 == 0 && coveredEdge(A0, B0)) {
				CALL_MEMBER_FN(*this, p)(x, y, z, drawn);
			}
			else if ((LONGLONG)xe1 == 0 && coveredEdge(A1, B1)) {
				CALL_MEMBER_FN(*this, p)(x, y, z, drawn);
			}
			else if ((LONGLONG)xe2 == 0 && coveredEdge(A2, B2)) {
				CALL_MEMBER_FN(*this, p)(x, y, z, drawn);
			}
			xe0 += A0;
			xe1 += A1;
			xe2 += A2;
		}

		e0 += B0;
		e1 += B1;
		e2 += B2;
	}
}

