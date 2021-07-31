#ifndef RAZE_SDK_CGLOBALVARSBASE_H
#define RAZE_SDK_CGLOBALVARSBASE_H
#pragma once

#include <raze/common.h>

struct CGlobalVarsBase
{
	float realtime;
	int32 framecount;
	float absoluteframetime;
	float absoluteframestarttimestddev;
	float curtime;
	float frametime;
	int32 maxClients;
	int32 tickcount;
	float interval_per_tick;
	float interpolation_amount;
	int32 simTicksThisFrame;
	int32 network_protocol;
};

#endif