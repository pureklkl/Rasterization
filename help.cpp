#include "stdafx.h"

#include "help.h"

short	ctoi(float color)		/* convert float color to GzIntensity short */
{
	return(short)((int)(color * ((1 << 12) - 1)));
}

