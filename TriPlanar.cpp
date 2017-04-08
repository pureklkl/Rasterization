#include "stdafx.h"
#include "TriPlanar.h"


TriPlanar::~TriPlanar()
{
}

/*
	Planar eq: Ap*x+Bp*y+C=z

			|A1 A2 A0| |z0| |Ap|
1/(2*area)*	|B1 B2 B0|*|z1|=|Bp|
			|C1 C2 C0| |z2| |Cp|
*/

void TriPlanar::calPlanar(LineEq<QType>* edges[], int z0, int z1, int z2, double area) {
	double scale = edges[0]->DSCALE/ (double)area;
	double sz0 = z0*scale,
		sz1 = z1*scale,
		sz2 = z2*scale;

	Ap = edges[1]->getA()*sz0 + edges[2]->getA()*sz1 + edges[0]->getA()*sz2;
	Bp = edges[1]->getB()*sz0 + edges[2]->getB()*sz1 + edges[0]->getB()*sz2;
	Cp = edges[1]->getC()*sz0 + edges[2]->getC()*sz1 + edges[0]->getC()*sz2;

	Ap /= (edges[0]->DSCALE);
	Bp /= (edges[0]->DSCALE);
	Cp /= (edges[0]->DSCALE);
}

void TriPlanar::buildFromPoint(GzCoord point[3]) {
	LineEq<float> edges[3] = {LineEq<float>(1.f), LineEq<float>(1.f), LineEq<float>(1.f)};

	edges[0].calLineEq(point[0], point[1]);
	edges[1].calLineEq(point[1], point[2]);
	edges[2].calLineEq(point[2], point[0]);
	/*
	compute area of the triangle, make sure the vertices is given in clockwise
	*/
	float area = edges[0].getC() + edges[1].getC() + edges[2].getC();

	double scale = 1. / (double)area;
	double sz0 = point[0][2] *scale,
		sz1 = point[1][2] *scale,
		sz2 = point[2][2] *scale;

	//TODO Refactoring
	Ap = edges[1].getA()*sz0 + edges[2].getA()*sz1 + edges[0].getA()*sz2;
	Bp = edges[1].getB()*sz0 + edges[2].getB()*sz1 + edges[0].getB()*sz2;
	Cp = edges[1].getC()*sz0 + edges[2].getC()*sz1 + edges[0].getC()*sz2;

}
