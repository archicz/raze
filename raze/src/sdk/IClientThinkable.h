#ifndef RAZE_SDK_ICLIENTTHINKABLE_H
#define RAZE_SDK_ICLIENTTHINKABLE_H
#pragma once

#include <sdk/IClientUnknown.h>

typedef void* ClientThinkHandle_t;

class IClientThinkable
{
public:
	virtual IClientUnknown* GetIClientUnknown() = 0;
	virtual void ClientThink() = 0;
	virtual ClientThinkHandle_t	GetThinkHandle() = 0;
	virtual void SetThinkHandle(ClientThinkHandle_t hThink) = 0;
	virtual void Release() = 0;
};

#endif