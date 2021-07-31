#ifndef RAZE_COMMON_H
#define RAZE_COMMON_H
#pragma once

typedef unsigned long long uint64;
typedef signed long long int64;

typedef unsigned long uintp;
typedef unsigned long ulong;
typedef unsigned int uint32;
typedef signed int int32;

typedef unsigned short uint16;
typedef signed short int16;

typedef unsigned char uint8;
typedef signed char int8;

#define Bits2Bytes(b) ((b+7) >> 3)
#define Bytes2Bits(b) (b << 3)

inline int32 GetBitForBitnum(int32 bitNum)
{
	static int bitsForBitnum[] =
	{
		(1 << 0),
		(1 << 1),
		(1 << 2),
		(1 << 3),
		(1 << 4),
		(1 << 5),
		(1 << 6),
		(1 << 7),
		(1 << 8),
		(1 << 9),
		(1 << 10),
		(1 << 11),
		(1 << 12),
		(1 << 13),
		(1 << 14),
		(1 << 15),
		(1 << 16),
		(1 << 17),
		(1 << 18),
		(1 << 19),
		(1 << 20),
		(1 << 21),
		(1 << 22),
		(1 << 23),
		(1 << 24),
		(1 << 25),
		(1 << 26),
		(1 << 27),
		(1 << 28),
		(1 << 29),
		(1 << 30),
		(1 << 31),
	};

	return bitsForBitnum[(bitNum) & (32 - 1)];
}

#endif