#ifndef RAZE_SDK_CLIENTCLASS_H
#define RAZE_SDK_CLIENTCLASS_H
#pragma once

#include <sdk/dt_recv.h>
#include <sdk/IClientEntity.h>

typedef IClientNetworkable*(*CreateClientClassFn)(int32 entnum, int32 serialNum);
typedef IClientNetworkable*(*CreateEventFn)();

class ClientClass
{
public:
	CreateClientClassFn	 m_pCreateFn;
	CreateEventFn m_pCreateEventFn;
	char* m_pNetworkName;
	RecvTable* m_pRecvTable;
	ClientClass* m_pNext;
	int32 m_ClassID;
	const char* m_pMapClassname;
};

#endif