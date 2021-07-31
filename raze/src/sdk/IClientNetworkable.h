#ifndef RAZE_SDK_ICLIENTNETWORKABLE_H
#define RAZE_SDK_ICLIENTNETWORKABLE_H
#pragma once

#include <raze/common.h>
#include <sdk/IClientUnknown.h>

class ClientClass;
class bf_read;

enum ShouldTransmitState_t
{
	SHOULDTRANSMIT_START = 0,
	SHOULDTRANSMIT_END
};

enum DataUpdateType_t
{
	DATA_UPDATE_CREATED = 0,
	DATA_UPDATE_DATATABLE_CHANGED,
	DATA_UPDATE_POST_UPDATE,
};

class IClientNetworkable
{
public:
	virtual IClientUnknown* GetIClientUnknown() = 0;
	virtual void Release() = 0;
	virtual ClientClass* GetClientClass() = 0;
	virtual void NotifyShouldTransmit(ShouldTransmitState_t state) = 0;
	virtual void OnPreDataChanged(DataUpdateType_t updateType) = 0;
	virtual void OnDataChanged(DataUpdateType_t updateType) = 0;
	virtual void PreDataUpdate(DataUpdateType_t updateType) = 0;
	virtual void PostDataUpdate(DataUpdateType_t updateType) = 0;
	virtual void OnDataUnchangedInPVS() = 0;
	virtual bool IsDormant() const = 0;
	virtual int32 EntIndex() const = 0;
	virtual void ReceiveMessage(int32 classID, bf_read& msg) = 0;
	virtual void* GetDataTableBasePtr() = 0;
	virtual void SetDestroyedOnRecreateEntities() = 0;
};

#endif