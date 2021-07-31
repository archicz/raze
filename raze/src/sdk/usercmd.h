#ifndef RAZE_SDK_CUSERCMD_H
#define RAZE_SDK_CUSERCMD_H
#pragma once

#include <raze/math.h>

struct CUserCmd
{
	int32 command_number;
	int32 tick_count;
	QAngle viewangles;
	Vector aimdirection;
	float forwardmove;
	float sidemove;
	float upmove;
	int32 buttons;
	uint8 impulse;
	int32 weaponselect;
	int32 weaponsubtype;
	int32 random_seed;
	int16 mousedx;
	int16 mousedy;
	bool hasbeenpredicted;
};

#endif