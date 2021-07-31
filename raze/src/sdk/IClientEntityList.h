#ifndef RAZE_SDK_ICLIENTENTITYLIST
#define RAZE_SDK_ICLIENTENTITYLIST
#pragma once

#include <sdk/interface.h>
#include <sdk/ehandle.h>
#include <sdk/IClientEntity.h>

class IClientEntityList
{
public:
	struct VTableFunctions
	{
		VTableContext* ctx = 0;

		typedef IClientEntity* (__thiscall* FnGetClientEntity)(void*, int32);
		uint32 indexGetClientEntity = 3;
		FnGetClientEntity GetClientEntity = 0;

		typedef IClientEntity* (__thiscall* FnGetClientEntityFromHandle)(void*, CBaseHandle);
		uint32 indexGetClientEntityFromHandle = 4;
		FnGetClientEntityFromHandle GetClientEntityFromHandle = 0;

		typedef int32 (__thiscall* FnGetHighestEntityIndex)(void*);
		uint32 indexGetHighestEntityIndex = 6;
		FnGetHighestEntityIndex GetHighestEntityIndex = 0;

		void Link(void* vt)
		{
			ctx = CreateVTableContext(vt);

			VFUNC_MACRO(GetClientEntity);
			VFUNC_MACRO(GetClientEntityFromHandle);
			VFUNC_MACRO(GetHighestEntityIndex);
		}

		void Cleanup()
		{
			delete ctx->vtableCopy;
			delete ctx;
		}
	};

	static VTableFunctions* vfuncs;
public:
	IClientEntity* GetClientEntity(int32 index)
	{
		return vfuncs->GetClientEntity(this, index);
	}

	IClientEntity* GetClientEntityFromHandle(CBaseHandle ent)
	{
		return vfuncs->GetClientEntityFromHandle(this, ent);
	}

	int32 GetHighestEntityIndex()
	{
		return vfuncs->GetHighestEntityIndex(this);
	}
};

#endif