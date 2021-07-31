#ifndef RAZE_MEMORY_H
#define RAZE_MEMORY_H
#pragma once

#include <Windows.h>
#include <raze/common.h>

struct ModuleContext
{
	HMODULE handle;
	void* addr;
	uint32 size;
};

struct VTableContext
{
	uintp** vtable;
	uintp* vtableOrig;
	uintp* vtableCopy;
	uint32 methodCount;
};

int32* PatternToByteArray(const char* pattern, uint32* byteCount);
int32* StringToByteArray(const char* string, bool wildcard);

uint8* FindInMemory(void* addr, uint32 size, int32* bytes, uint32 byteCount);
uint8* FindInMemoryReverse(void* addr, uint32 size, int32* bytes, uint32 byteCount);

uint8* PatternScan(void* addr, uint32 size, const char* pattern);
uint8* PatternScanReverse(void* addr, uint32 size, const char* pattern);
uint8* StringScan(void* addr, uint32 size, const char* string, bool wilrdcard);

uint8* PointerXRef(void* addr, uint32 size, void* ptr);

uint8* FindFunctionStart(void* addr, uint32 size);
uint8* FindFunctionByString(void* addr, uint32 size, const char* string, uint32 findSize);

ModuleContext* CreateModuleContext(const char* fileName);

VTableContext* CreateVTableContext(void* vtb);

void SwapVTableOrig(VTableContext* ctx);
void SwapVTableCopy(VTableContext* ctx);

void HookVTableMethod(VTableContext* ctx, void* fn, uint32 index);
void* GetOriginalVTableMethod(VTableContext* ctx, uint32 index);

#endif