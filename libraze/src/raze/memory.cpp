#include <raze/memory.h>
#include <string.h>
#include <stdlib.h>

int32* PatternToByteArray(const char* pattern, uint32* byteCount)
{
	uint32 curByte = 0;
	int32* bytes = (int32*)malloc(strlen(pattern) * sizeof(int32));

	char* patternCopy = (char*)malloc(strlen(pattern));
	strcpy(patternCopy, pattern);

	char* token = strtok((char*)patternCopy, " ");

	while (token != 0)
	{
		bool wildcard = false;

		if (strcmp(token, "??") == 0)
		{
			wildcard = true;
		}

		bytes[curByte] = wildcard ? -1 : strtoul(token, &token, 16);
		curByte++;

		token = strtok(0, " ");
	}

	free(patternCopy);

	*byteCount = curByte;
	int32* bytesFinal = (int32*)realloc(bytes, curByte * sizeof(int32));
	return bytesFinal;
}

int32* StringToByteArray(const char* string, bool wildcard)
{
	int32* bytes = (int32*)malloc(strlen(string) * sizeof(int32));

	for (uint32 i = 0; i < strlen(string) + 1; i++)
	{
		bytes[i] = wildcard ? (string[i] == '?' ? -1 : string[i]) : string[i];
	}

	return bytes;
}

uint8* FindInMemory(void* addr, uint32 size, int32* bytes, uint32 byteCount)
{
	uint8* scan = (uint8*)addr;

	for (uint32 i = 0; i < size - byteCount; i++)
	{
		bool found = true;

		for (uint32 j = 0; j < byteCount; j++)
		{
			if (scan[i + j] != bytes[j] && bytes[j] != -1)
			{
				found = false;
				break;
			}
		}

		if (found)
		{
			return &scan[i];
		}
	}

	return 0;
}

uint8* FindInMemoryReverse(void* addr, uint32 size, int32* bytes, uint32 byteCount)
{
	uint8* scan = (uint8*)addr;

	for (uint32 i = 0; i < size - byteCount; i++)
	{
		bool found = true;

		for (uint32 j = 0; j < byteCount; j++)
		{
			if ((scan - i)[j] != bytes[j] && bytes[j] != -1)
			{
				found = false;
				break;
			}
		}

		if (found)
		{
			return scan - i;
		}
	}

	return 0;
}

uint8* PatternScan(void* addr, uint32 size, const char* pattern)
{
	uint32 byteCount = 0;
	int32* bytes = PatternToByteArray(pattern, &byteCount);

	uint8* found = FindInMemory(addr, size, bytes, byteCount);
	free(bytes);

	return found;
}

uint8* PatternScanReverse(void* addr, uint32 size, const char* pattern)
{
	uint32 byteCount = 0;
	int32* bytes = PatternToByteArray(pattern, &byteCount);

	uint8* found = FindInMemoryReverse(addr, size, bytes, byteCount);
	free(bytes);

	return found;
}

uint8* StringScan(void* addr, uint32 size, const char* string, bool wilrdcard)
{
	int32* bytes = StringToByteArray(string, wilrdcard);

	uint8* found = FindInMemory(addr, size, bytes, strlen(string));
	free(bytes);

	return found;
}

uint8* PointerXRef(void* addr, uint32 size, void* ptr)
{
	uint8* scan = (uint8*)addr;

	for (uint32 i = 0; i < size - sizeof(uintp); i++)
	{
		uint8* xrefAddr = *(uint8**)(scan + i);
		if (xrefAddr == ptr)
		{
			return scan + i;
		}
	}

	return 0;
}

static uint8* FindThisCallFunctionStart(void* addr, uint32 size)
{
	uint8* found = 0;
	int32 instr1 = 0x55;            // push ebp
	int32 instr2[] = {0x8B, 0xEC};  // mov ebp, esp

	found = FindInMemoryReverse(addr, size, instr2, sizeof(instr2) / sizeof(int32));
	if (found)
	{
		found = FindInMemoryReverse(found, size, &instr1, 1);
	}

	return found;
}

static uint8* FindFastCallFunctionStart(void* addr, uint32 size)
{
	uint8* found = 0;
	int32 instr1 = 0x53;            // push ebx
	int32 instr2[] = {0x8B, 0xDA};  // mov ebx, edx

	found = FindInMemoryReverse(addr, size, instr2, sizeof(instr2) / sizeof(int32));
	if (found)
	{
		found = FindInMemoryReverse(found, size, &instr1, 1);
	}

	return found;
}

uint8* FindFunctionStart(void* addr, uint32 size)
{
	uint8* found = 0;

	found = FindThisCallFunctionStart(addr, size);
	if (!found)
	{
		found = FindFastCallFunctionStart(addr, size);
	}
	
	return found;
}

uint8* FindFunctionByString(void* addr, uint32 size, const char* string, uint32 findSize)
{
	uint8* findString = StringScan(addr, size, string, false);
	if (findString)
	{
		void* xref = PointerXRef(addr, size, findString);
		if (xref)
		{
			return FindFunctionStart(xref, findSize);
		}
	}

	return 0;
}

ModuleContext* CreateModuleContext(const char* fileName)
{
	HMODULE moduleHandle = GetModuleHandleA(fileName);
	if (moduleHandle)
	{
		PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)moduleHandle;
		PIMAGE_NT_HEADERS ntHeaders = (PIMAGE_NT_HEADERS)((uint8*)moduleHandle + dosHeader->e_lfanew);
		
		ModuleContext* ctx = new ModuleContext;
		ctx->handle = moduleHandle;
		ctx->addr = moduleHandle;
		ctx->size = ntHeaders->OptionalHeader.SizeOfImage;
		
		return ctx;
	}

	return 0;
}

VTableContext* CreateVTableContext(void* vtb)
{
	VTableContext* ctx = new VTableContext;

	ctx->vtable = (uintp**)vtb;
	ctx->vtableOrig = *(uintp**)vtb;
	ctx->methodCount = 0;

	while (ctx->vtableOrig[ctx->methodCount])
	{
		ctx->methodCount++;
	}
	
	ctx->vtableCopy = new uintp[ctx->methodCount];
	memcpy(ctx->vtableCopy, ctx->vtableOrig, sizeof(uintp) * ctx->methodCount);

	return ctx;
}

void SwapVTableOrig(VTableContext* ctx)
{
	*ctx->vtable = ctx->vtableOrig;
}

void SwapVTableCopy(VTableContext* ctx)
{
	*ctx->vtable = ctx->vtableCopy;
}

void HookVTableMethod(VTableContext* ctx, void* fn, uint32 index)
{
	ctx->vtableCopy[index] = (uintp)fn;
}

void* GetOriginalVTableMethod(VTableContext* ctx, uint32 index)
{
	return (void*)ctx->vtableOrig[index];
}
