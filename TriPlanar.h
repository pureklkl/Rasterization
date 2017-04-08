#pragma once
#include "LineEq.h"
class TriPlanar
{
public:

	TriPlanar() {
	}

	double evaluate(int x, int y) {
		return (Ap*(double)x + Bp*(double)y + Cp);
	}

	double getAp() {
		return Ap;
	}

	double getBp() {
		return Bp;
	}

	double getCp() {
		return Cp;
	}
	
	void calPlanar(LineEq<QType>* edges[], int z0, int z1, int z2, double area);
	void buildFromPoint(GzCoord vertices[3]);
	~TriPlanar();
private:
	double Ap, Bp, Cp;
	
};

