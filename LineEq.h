#pragma once
#include "gz.h"

#define SCALEFACTOR (1 << 24)

typedef LONGLONG QType;

template<class T>
class LineEq
{
public:
	LineEq(T scale);
	~LineEq();
	void calLineEq(GzCoord v0, GzCoord v1);
	double evaluate(int x, int y);

	T getA() {
		return A;
	}

	T getB() {
		return B;
	}

	T getC() {
		return C;
	}
	// reverse line eq in case vertices are not given in clock wise
	void reverse() {
		A = -A;
		B = -B;
		C = -C;
	}
	T INTSCALE;
	double DSCALE;
private:
	T A, B, C;
};

template <class T>
LineEq<T>::LineEq(T scale)
{
	INTSCALE = scale;
	DSCALE = scale;
}

template <class T>
LineEq<T>::~LineEq()
{
}

template <class T>
void LineEq<T>::calLineEq(GzCoord v0, GzCoord v1) {
	/*
	A = y0 - y1;
	B = x0 - x1;
	C = -(A(x0+x1)+B(y0+y1))/2

	generate int type Ax+By+C=0
	*/
	float a = v0[Y] - v1[Y];
	float b = v1[X] - v0[X];
	float c = -(a*(v0[X] + v1[X]) + b*(v0[Y] + v1[Y])) * 0.5f;

	A = (T)(a*INTSCALE);
	B = (T)(b*INTSCALE);
	C = (T)(c*INTSCALE);
}

template <class T>
double LineEq<T>::evaluate(int x, int y) {
	return A*x + B*y + C;
}