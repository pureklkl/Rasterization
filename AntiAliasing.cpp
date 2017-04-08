#include "stdafx.h"
#include "AntiAliasing.h"


AntiAliasing::AntiAliasing(GzDisplay* display)
{
	for (int i = 0; i < AAKERNEL_SIZE; i++) {
		GzNewDisplay(&samples[i], display->xres, display->yres);
		GzInitDisplay(samples[i]);
	}
}


AntiAliasing::~AntiAliasing(){
	for (int i = 0; i < AAKERNEL_SIZE; i++) {
		GzFreeDisplay(samples[i]);
	}
}
