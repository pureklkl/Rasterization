#pragma once
#include "gz.h"
/* NOT part of API - just for general assistance */
short	ctoi(float color);

template <class vt>
void clampVal(vt *val, vt min, vt max) {
	*val = *val > max ? max : *val;
	*val = *val < min ? min : *val;
}


template <class vt>
void swap(vt& v0, vt& v1) {
	vt t = v0;
	v0 = v1;
	v1 = t;
}

template <class vt>
void minMax3(vt v0, vt v1, vt v2, vt& min, vt& max) {
	if (v0 < v1) {
		if (v0 < v2) {
			min = v0;
			if (v1 < v2) {
				max = v2;
			}
			else {
				max = v1;
			}
		}
		else {
			min = v2;
			max = v1;
		}
	}
	else {
		if (v0 > v2) {
			max = v0;
			if (v1 > v2) {
				min = v2;
			}
			else {
				min = v1;
			}
		}
		else {
			min = v1;
			max = v2;
		}
	}
}


/*
template <class vt>
void findMinMax4(vt v0, vt v1, vt v2, vt v3, vt& min, vt& max) {
	if (v0 > v1) {
		swap<vt>(v0, v1);
	}
	if (v2 > v3) {
		swap<vt>(v2, v3);
	}
	min = v0 > v2 ? v2 : v0;
	max = v1 > v3 ? v1 : v3;
}
*/

