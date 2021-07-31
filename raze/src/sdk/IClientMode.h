#ifndef RAZE_SDK_ICLIENTMODE_H
#define RAZE_SDK_ICLIENTMODE_H
#pragma once

#include <sdk/interface.h>
#include <sdk/usercmd.h>

class IClientMode
{
public:
	struct VTableFunctions
	{
		VTableContext* ctx = 0;

		typedef bool(__thiscall* FnCreateMove)(void*, float, CUserCmd*);
		uint32 indexCreateMove = 24;
		FnCreateMove CreateMove = 0;

		void Link(void* vt)
		{
			ctx = CreateVTableContext(vt);

			VFUNC_MACRO(CreateMove);
		}

		void Cleanup()
		{
			delete ctx->vtableCopy;
			delete ctx;
		}

		void Hook(void* hook, uint32 index)
		{
			HookVTableMethod(ctx, hook, index);
		}

		void ApplyHooks()
		{
			SwapVTableCopy(ctx);
		}

		void RestoreHooks()
		{
			SwapVTableOrig(ctx);
		}
	};

	static VTableFunctions* vfuncs;
public:
	bool CreateMove(float inputSampleTime, CUserCmd* cmd)
	{
		return vfuncs->CreateMove(this, inputSampleTime, cmd);
	}
};

#endif