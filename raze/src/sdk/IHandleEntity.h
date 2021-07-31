#ifndef RAZE_SDK_IHANDLEENTITY_H
#define RAZE_SDK_IHANDLEENTITY_H
#pragma once

class CBaseHandle;

class IHandleEntity
{
public:
	virtual ~IHandleEntity() {}
	virtual void SetRefEHandle(const CBaseHandle& handle) = 0;
	virtual const CBaseHandle& GetRefEHandle() const = 0;
};

#endif