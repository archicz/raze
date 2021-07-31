#ifndef RAZE_SDK_ICLIENTENTITY_H
#define RAZE_SDK_ICLIENTENTITY_H
#pragma once

#include <sdk/IClientRenderable.h>
#include <sdk/IClientNetworkable.h>
#include <sdk/IClientThinkable.h>

class CMouthInfo;
struct SpatializationInfo_t;

class IClientEntity : public IClientUnknown, public IClientRenderable, public IClientNetworkable, public IClientThinkable
{
public:
	virtual void Release() = 0;
	virtual const Vector& GetAbsOrigin() const = 0;
	virtual const QAngle& GetAbsAngles() const = 0;
	virtual CMouthInfo* GetMouth() = 0;
	virtual bool GetSoundSpatialization(SpatializationInfo_t& info) = 0;
	virtual bool IsBlurred() = 0;
};

#endif