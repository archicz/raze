#ifndef RAZE_COLOR_H
#define RAZE_COLOR_H
#pragma once

#include <raze/common.h>

struct Color
{
	uint8 r = 0;
	uint8 g = 0;
	uint8 b = 0;
	uint8 a = 0;

	Color(uint8 _r, uint8 _g, uint8 _b, uint8 _a) :
		r(_r), g(_g), b(_b), a(_a)
	{
	}

	Color(uint8 _r, uint8 _g, uint8 _b) :
		r(_r), g(_g), b(_b), a(255)
	{
	}

	Color(uint8 rgb, uint8 alpha) :
		r(rgb), g(rgb), b(rgb), a(alpha)
	{
	}
};

#endif