#ifndef RAZE_SDK_INTERFACE_H
#define RAZE_SDK_INTERFACE_H
#pragma once

#include <raze/memory.h>

typedef void*(*FnCreateInterface)(const char*, int*);

#define VFUNC_MACRO(name) \
	name = (Fn##name)GetOriginalVTableMethod(ctx, index##name); \

/*
public:
	struct VTableFunctions
	{
		VTableContext* ctx = 0;

		// typedef
		// uint32 index
		// Fn

		void Link(void* vt)
		{
			ctx = CreateVTableContext(vt);
			
			// VFUNC_MACRO
		}

		void Cleanup()
		{
			delete ctx->vtableCopy;
			delete ctx;
		}

		// OPTIONAL
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
*/

#endif