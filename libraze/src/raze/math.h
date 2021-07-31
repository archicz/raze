#ifndef RAZE_MATH_H
#define RAZE_MATH_H
#pragma once

#include <raze/common.h>

// normal math functions
template<class T>
T clamp(T n, T lower, T upper)
{
	n = (n > lower) * n + !(n > lower) * lower;
	return (n < upper) * n + !(n < upper) * upper;
}

template<class T>
T remap(T value, T inMin, T inMax, T outMin, T outMax)
{
	return outMin + (((value - inMin) / (inMax - inMin)) * (outMax - outMin));
}


// source engine specific structs
struct Vector
{
	float x, y, z;
};

struct QAngle
{
	float p, y, r;
};

struct matrix3x4
{
	float mat[3][4];
};

struct VMatrix
{
	float mat[4][4];
};

#endif