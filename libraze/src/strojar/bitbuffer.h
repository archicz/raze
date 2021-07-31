#ifndef STROJAR_BITBUFFER_H
#define STROJAR_BITBUFFER_H
#pragma once

#include <raze/common.h>

class StrojarBitbuffer
{
public:
	StrojarBitbuffer();
	StrojarBitbuffer(void* pData, uint32 nBytes, int32 nMaxBits = -1);

	uint32 GetNumBitsLeft();
	uint32 GetNumBytesLeft();

	uint32 GetNumBitsProcessed();
	uint32 GetNumBytesProcessed();

	void Reset();
	void Seek(uint32 bitpos);
	void SeekRelative(uint32 bitpos);
public: // write functions
	void WriteOneBit(uint8 bit);

	void WriteUBitLong(uint32 data, uint32 numbits);
	void WriteSBitLong(int32 data, uint32 numbits);
	void WriteVarInt32(uint32 data);
	void WriteFloat(float data);

	bool WriteBits(void* pIn, uint32 nBits);
	bool WriteBytes(void* pIn, uint32 nBytes);

	void WriteChar(char val);
	void WriteByte(uint8 val);
	void WriteShort(int16 val);
	void WriteWord(uint16 val);
	void WriteLong(int32 val);
	void WriteUnsignedLong(uint32 val);

	bool WriteString(const char* pStr);
public: // read functions
	uint8 ReadOneBit();

	uint32 ReadUBitLong(uint32 numbits);
	int32 ReadSBitLong(uint32 numbits);
	uint32 ReadVarInt32();
	float ReadFloat();

	void ReadBits(void* pOut, uint32 nBits);
	void ReadBytes(void* pOut, uint32 nBytes);

	char ReadChar();
	uint8 ReadByte();
	int16 ReadShort();
	uint16 ReadWord();
	int32 ReadLong();
	uint32 ReadUnsignedLong();

	bool ReadString(char* pStr, uint32 bufLen);
public:
	uintp* __restrict m_pData;
	uint32 m_nDataBytes;
	uint32 m_nDataBits;
	uint32 m_nCurBit;
};

#endif