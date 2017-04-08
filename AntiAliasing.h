#pragma once
#include "gz.h"
#include "disp.h"
#include "rend.h"
#include "help.h"
class AntiAliasing
{
public:

	void setFilters(AAFilter filters[]) {
		memcpy(this->filters, filters, AAKERNEL_SIZE * 3 * sizeof(filters[0][0]));
		GzCoord tx;
		for (int i = 0; i < AAKERNEL_SIZE; i++) {
			tx[0] = filters[i][0];
			tx[1] = filters[i][1];
			tx[2] = 0;
			GzTrxMat(tx, filterTx[i]);
		}
	}

	GzDisplay* getBuffer(int index) {
		return samples[index];
	}

	GzMatrix* getTxMat(int index) {
		return &filterTx[index];
	}

	void setOutput(GzDisplay *display) {
		short scolor[3];
		for(int i=0;i<display->xres;i++)
			for (int j = 0; j < display->yres; j++) {
				weightSum(i, j, scolor);
				putBuffer(display, i, j, scolor);
			}
	}

	AntiAliasing(GzDisplay* display);
	~AntiAliasing();

private:

	void weightSum(int i, int j, short scolor[]) {
		GzColor color = {0.f, 0.f, 0.f};
		/*for (int k = 1; k < 2; k++) {
			GzDisplay *display = samples[k];
			color[BLUE] += (float)((display->fbuf + ARRAY(i, j))->blue);
			color[RED] += (float)((display->fbuf + ARRAY(i, j))->red);
			color[GREEN] += (float)((display->fbuf + ARRAY(i, j))->green);
		}*/
		for (int k = 0; k < AAKERNEL_SIZE; k++) {
			GzDisplay *display = samples[k];
			color[BLUE] += (float)((display->fbuf + ARRAY(i, j))->blue)*filters[k][2];
			color[RED] += (float)((display->fbuf + ARRAY(i, j))->red)*filters[k][2];
			color[GREEN] += (float)((display->fbuf + ARRAY(i, j))->green)*filters[k][2];
		}
		for (int c = 0; c < 3; c++) {
			scolor[c] = (short)color[c];
			clampVal<short>(&scolor[c], 0, MAXRGBA);
		}
	}

	void putBuffer(GzDisplay *display, int i, int j, short color[]) {

		(display->fbuf + ARRAY(i, j))->blue = color[BLUE];
		(display->fbuf + ARRAY(i, j))->green = color[GREEN];
		(display->fbuf + ARRAY(i, j))->red = color[RED];

	}

	GzMatrix filterTx[AAKERNEL_SIZE];
	GzDisplay *samples[AAKERNEL_SIZE];
	AAFilter filters[AAKERNEL_SIZE];
};

