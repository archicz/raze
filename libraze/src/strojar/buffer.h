#ifndef STROJAR_BUFFER_H
#define STROJAR_BUFFER_H
#pragma once

#include <raze/common.h>
#include <strojar/memory.h>

class StrojarBuffer
{
public:
	enum SeekType_t
	{
		SEEK_HEAD = 0,
		SEEK_CURRENT,
		SEEK_TAIL
	};

	StrojarBuffer(uint32 growSize = 0, uint32 initSize = 0, bool text = false);
	StrojarBuffer(void const* pBuffer, uint32 size, bool text = false);
public: // write functions
	void PutChar(int8 c);
	void PutUnsignedChar(uint8 uc);
	void PutShort(int16 s);
	void PutUnsignedShort(uint16 us);
	void PutInt(int32 i);
	void PutUnsignedInt(uint32 u);
	void PutFloat(float f);
	void PutDouble(double d);
	void PutString(const char* pString);
	void Put(const void* pMem, uint32 size);

	void* PeekPut(uint32 offset = 0);
	void* PeekGet(uint32 offset = 0);

	uint32 TellPut() const;
	uint32 TellGet() const;

	void SeekPut(SeekType_t type, uint32 offset);
	void SeekGet(SeekType_t type, uint32 offset);
public: // read functions
	int8 GetChar();
	uint8 GetUnsignedChar();
	int16 GetShort();
	uint16 GetUnsignedShort();
	int32 GetInt();
	uint32 GetUnsignedInt();
	float GetFloat();
	double GetDouble();
	void GetString(char* pString, uint32 nMaxLen = 0);
	void Get(void* pMem, uint32 size);
public:
	bool CheckPut(uint32 size);
	bool CheckGet(uint32 size);
	void EnsureCapacity(uint32 num);
public:
	void* Base();
	uint32 Size();

	inline bool IsText() const { return (m_Flags & TEXT_BUFFER) != 0; }
private:
	enum
	{
		PUT_OVERFLOW = 0x1,
		GET_OVERFLOW = 0x2,
	};

	enum
	{
		TEXT_BUFFER = 0x1,
	};
public:
	StrojarMemory<uint8> m_Memory;
	uint32 m_Get;
	uint32 m_Put;
	uint8 m_Error;
	uint8 m_Flags;
};

#endif