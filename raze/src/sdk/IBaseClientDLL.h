#ifndef RAZE_SDK_IBASECLIENTDLL_H
#define RAZE_SDK_IBASECLIENTDLL_H
#pragma once

#include <sdk/interface.h>

class ClientClass;

enum ClientFrameStage_t
{
	FRAME_UNDEFINED = -1,
	FRAME_START,
	FRAME_NET_UPDATE_START,
	FRAME_NET_UPDATE_POSTDATAUPDATE_START,
	FRAME_NET_UPDATE_POSTDATAUPDATE_END,
	FRAME_NET_UPDATE_END,
	FRAME_RENDER_START,
	FRAME_RENDER_END,
	FRAME_NET_FULL_FRAME_UPDATE_ON_REMOVE
};

class IBaseClientDLL
{
public:
	struct VTableFunctions
	{
		VTableContext* ctx = 0;

		typedef ClientClass*(__thiscall* FnGetAllClasses)(void*);
		uint32 indexGetAllClasses = 8;
		FnGetAllClasses GetAllClasses = 0;

		typedef void(__thiscall* FnFrameStageNotify)(void*, ClientFrameStage_t);
		uint32 indexFrameStageNotify = 37;
		FnFrameStageNotify FrameStageNotify = 0;

		typedef bool(__thiscall* FnDispatchUserMessage)(void*, int32, int32, int32, void*);
		uint32 indexDispatchUserMessage = 38;
		FnDispatchUserMessage DispatchUserMessage = 0;

		void Link(void* vt)
		{
			ctx = CreateVTableContext(vt);

			VFUNC_MACRO(GetAllClasses);
			VFUNC_MACRO(FrameStageNotify);
			VFUNC_MACRO(DispatchUserMessage);
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
	ClientClass* GetAllClasses()
	{
		return vfuncs->GetAllClasses(this);
	}

	void FrameStageNotify(ClientFrameStage_t stage)
	{
		return vfuncs->FrameStageNotify(this, stage);
	}

	bool DispatchUserMessage(int32 msgType, int32 flags, int32 size, void* msg)
	{
		return vfuncs->DispatchUserMessage(this, msgType, flags, size, msg);
	}
};

#endif