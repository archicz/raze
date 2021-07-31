#include "buffer.h"

#define PUT_TYPE(type, val)			\
	if (CheckPut(sizeof(type)))		\
	{								\
		*(type*)PeekPut() = val;	\
		m_Put += sizeof(type);		\
	}

#define GET_TYPE(type, val)			\
	if (CheckGet(sizeof(type)))		\
	{								\
		val = *(type*)PeekGet();	\
		m_Get += sizeof(type);		\
	}								\
	else							\
	{								\
		val = 0;					\
	}								\

StrojarBuffer::StrojarBuffer(uint32 growSize, uint32 initSize, bool text) :
	m_Memory(growSize, initSize), m_Error(0)
{
	m_Get = 0;
	m_Put = 0;
	m_Flags = 0;

	if (text)
	{
		m_Flags |= TEXT_BUFFER;
	}
}

StrojarBuffer::StrojarBuffer(void const* pBuffer, uint32 size, bool text) :
	m_Memory((uint8*)pBuffer, size), m_Error(0)
{
	m_Get = 0;
	m_Put = 0;
	m_Flags = 0;

	if (text)
	{
		m_Flags |= TEXT_BUFFER;
	}
}

void StrojarBuffer::PutChar(int8 c)
{
	PUT_TYPE(int8, c);
}

void StrojarBuffer::PutUnsignedChar(uint8 uc)
{
	PUT_TYPE(uint8, uc);
}

void StrojarBuffer::PutShort(int16 s)
{
	PUT_TYPE(int16, s);
}

void StrojarBuffer::PutUnsignedShort(uint16 us)
{
	PUT_TYPE(uint16, us);
}

void StrojarBuffer::PutInt(int32 i)
{
	PUT_TYPE(int32, i);
}

void StrojarBuffer::PutUnsignedInt(uint32 u)
{
	PUT_TYPE(uint32, u);
}

void StrojarBuffer::PutFloat(float f)
{
	PUT_TYPE(float, f);
}

void StrojarBuffer::PutDouble(double d)
{
	PUT_TYPE(double, d);
}

void StrojarBuffer::PutString(const char* pString)
{
	uint32 len = strlen(pString);
	
	if (!IsText())
	{
		++len;
	}

	Put(pString, len);
}

void StrojarBuffer::Put(const void* pMem, uint32 size)
{
	if (CheckPut(size))
	{
		memcpy(&m_Memory[m_Put], pMem, size);
		m_Put += size;
	}
}

void* StrojarBuffer::PeekPut(uint32 offset)
{
	return &m_Memory[m_Put + offset];
}

void* StrojarBuffer::PeekGet(uint32 offset)
{
	return PeekPut(offset);
}

uint32 StrojarBuffer::TellPut() const
{
	return m_Put;
}

uint32 StrojarBuffer::TellGet() const
{
	return m_Get;
}

void StrojarBuffer::SeekPut(SeekType_t type, uint32 offset)
{
	switch (type)
	{
		case SEEK_HEAD:
			m_Put = offset;
		break;
		case SEEK_CURRENT:
			m_Put += offset;
		break;
		case SEEK_TAIL:
			m_Put = m_Memory.NumAllocated() - offset;
		break;
	}
}

void StrojarBuffer::SeekGet(SeekType_t type, uint32 offset)
{
	switch (type)
	{
		case SEEK_HEAD:
			m_Get = offset;
		break;
		case SEEK_CURRENT:
			m_Get += offset;
		break;
		case SEEK_TAIL:
			m_Get = m_Memory.NumAllocated() - offset;
		break;
	}
}

int8 StrojarBuffer::GetChar()
{
	int8 c;
	GET_TYPE(int8, c);
	return c;
}

uint8 StrojarBuffer::GetUnsignedChar()
{
	uint8 c;
	GET_TYPE(uint8, c);
	return c;
}

int16 StrojarBuffer::GetShort()
{
	int16 s;
	GET_TYPE(int16, s);
	return s;
}

uint16 StrojarBuffer::GetUnsignedShort()
{
	uint16 us;
	GET_TYPE(uint16, us);
	return us;
}

int32 StrojarBuffer::GetInt()
{
	int32 i;
	GET_TYPE(int32, i);
	return i;
}

uint32 StrojarBuffer::GetUnsignedInt()
{
	uint32 u;
	GET_TYPE(uint32, u);
	return u;
}

float StrojarBuffer::GetFloat()
{
	float f;
	GET_TYPE(float, f);
	return f;
}

double StrojarBuffer::GetDouble()
{
	double d;
	GET_TYPE(double, d);
	return d;
}

void StrojarBuffer::GetString(char* pString, uint32 nMaxLen)
{
	uint32 len = strlen((char*)&m_Memory[m_Get]) + 1;
	if (len <= nMaxLen)
	{
		Get(pString, len);
	}
	else
	{
		Get(pString, nMaxLen);
		pString[nMaxLen - 1] = 0;
		SeekGet(SEEK_CURRENT, len - nMaxLen);
	}
}

void StrojarBuffer::Get(void* pMem, uint32 size)
{
	memcpy(pMem, &m_Memory[m_Get], size);
	m_Get += size;
}

bool StrojarBuffer::CheckPut(uint32 size)
{
	while (m_Memory.NumAllocated() < m_Put + size)
	{
		m_Memory.Grow();
	}

	return true;
}

bool StrojarBuffer::CheckGet(uint32 size)
{
	if (m_Memory.NumAllocated() >= m_Get + size)
	{
		return true;
	}

	m_Error |= GET_OVERFLOW;
	return false;
}

void StrojarBuffer::EnsureCapacity(uint32 num)
{
	m_Memory.EnsureCapacity(num);
}

void* StrojarBuffer::Base()
{
	return m_Memory.Base();
}

uint32 StrojarBuffer::Size()
{
	return m_Memory.NumAllocated();
}
