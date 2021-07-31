#ifndef STROJAR_MEMORY_H
#define STROJAR_MEMORY_H
#pragma once

#include <raze/common.h>

#include <stdlib.h>
#include <string>

template<class T>
class StrojarMemory
{
public:
	StrojarMemory(int32 nGrowSize = 0, uint32 nInitSize = 0);
	StrojarMemory(T* pMemory, uint32 numElements);
	~StrojarMemory();
public:
	T & operator[](uint32 i);
	T* Base();

	uint32 NumAllocated();

	uint32 CalcNewAllocationCount(uint32 nNewSize);
	void Grow(uint32 num = 1);
	void EnsureCapacity(uint32 num);

	void Purge();
public:
	T* m_pMemory;
	uint32 m_nAllocationCount;
	int32 m_nGrowSize;
};



template<class T>
inline StrojarMemory<T>::StrojarMemory(int32 nGrowSize, uint32 nInitSize)
{
	m_pMemory = nullptr;
	m_nAllocationCount = nInitSize;
	m_nGrowSize = nGrowSize;

	if (m_nAllocationCount)
	{
		m_pMemory = (T*)malloc(m_nAllocationCount * sizeof(T));
	}
}

template<class T>
inline StrojarMemory<T>::StrojarMemory(T* pMemory, uint32 numElements)
{
	m_pMemory = pMemory;
	m_nAllocationCount = numElements;
	m_nGrowSize = -1;
}

template<class T>
inline StrojarMemory<T>::~StrojarMemory()
{
	Purge();
}

template<class T>
inline T& StrojarMemory<T>::operator[](uint32 i)
{
	return m_pMemory[i];
}

template<class T>
inline T* StrojarMemory<T>::Base()
{
	return m_pMemory;
}

template<class T>
inline uint32 StrojarMemory<T>::NumAllocated()
{
	return m_nAllocationCount;
}

template<class T>
inline uint32 StrojarMemory<T>::CalcNewAllocationCount(uint32 nNewSize)
{
	uint32 nAllocationCount = m_nAllocationCount;

	if (m_nGrowSize)
	{
		nAllocationCount = ((1 + ((nNewSize - 1) / m_nGrowSize)) * m_nGrowSize);
	}
	else
	{
		if (!nAllocationCount)
		{
			nAllocationCount = (31 + sizeof(T)) / sizeof(T);
		}

		while (nAllocationCount < nNewSize)
		{
			nAllocationCount *= 2;
		}
	}

	return nAllocationCount;
}

template<class T>
inline void StrojarMemory<T>::Grow(uint32 num)
{
	if (m_nGrowSize == -1)
	{
		return;
	}

	uint32 nAllocationRequested = m_nAllocationCount + num;
	uint32 nNewAllocationCount = CalcNewAllocationCount(nAllocationRequested);

	m_nAllocationCount = nNewAllocationCount;

	if (m_pMemory)
	{
		m_pMemory = (T*)realloc(m_pMemory, m_nAllocationCount * sizeof(T));
	}
	else
	{
		m_pMemory = (T*)malloc(m_nAllocationCount * sizeof(T));
	}
}

template<class T>
inline void StrojarMemory<T>::EnsureCapacity(uint32 num)
{
	if (m_nGrowSize == -1)
	{
		return;
	}

	m_nAllocationCount = num;
	
	if (m_pMemory)
	{
		m_pMemory = (T*)realloc(m_pMemory, m_nAllocationCount * sizeof(T));
	}
	else
	{
		m_pMemory = (T*)malloc(m_nAllocationCount * sizeof(T));
	}
}

template<class T>
inline void StrojarMemory<T>::Purge()
{
	if (m_nGrowSize == -1)
	{
		return;
	}

	if (m_pMemory)
	{
		free((void*)m_pMemory);
		m_pMemory = nullptr;
	}

	m_nAllocationCount = 0;
}

#endif