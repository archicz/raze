#ifndef STROJAR_VECTOR_H
#define STROJAR_VECTOR_H
#pragma once

#include <strojar/memory.h>

template <class T>
inline T* Construct(T* pMemory)
{
	return ::new(pMemory) T;
}

template <class T>
inline void Destruct(T* pMemory)
{
	pMemory->~T();
}

template <class T>
inline T* CopyConstruct(T* pMemory, T const& src)
{
	return ::new(pMemory) T(src);
}



template<class T, class A = StrojarMemory<T>>
class StrojarVector
{
public:
	StrojarVector(int32 growSize = 0, uint32 initSize = 0);
	StrojarVector(T* pMemory, uint32 allocationCount, uint32 numElements = 0);
	~StrojarVector();
public:
	T& operator[](uint32 i);
	T& Element(uint32 i);
	T& Head();
	T& Tail();

	T* Base();
	uint32 Count();
public: // C++11 range support
	T* begin()
	{
		return Base();
	}

	const T* begin() const
	{
		return Base();
	}

	T* end()
	{
		return Base() + Count();
	}

	const T* end() const
	{
		return Base() + Count();
	}
public:
	uint32 AddToHead();
	uint32 AddToTail();
	uint32 InsertBefore(uint32 elem);
	uint32 InsertAfter(uint32 elem);

	uint32 AddToHead(const T& src);
	uint32 AddToTail(const T& src);
	uint32 InsertBefore(uint32 elem, const T& src);
	uint32 InsertAfter(uint32 elem, const T& src);

	int32 Find(const T& src);
	
	void Clear();
	void Remove(uint32 elem);
	bool FindAndRemove(const T& src);
	void RemoveAll();

	void RemoveFromHead();
	void RemoveFromTail();
	
	void Purge();
public:
	void GrowVector(uint32 num = 1);
	void ShiftElementsRight(uint32 elem, uint32 num = 1);
	void ShiftElementsLeft(uint32 elem, uint32 num = 1);
public:
	A m_Memory;
	uint32 m_Size;
	T* m_pElements;
};



template<class T, class A>
inline StrojarVector<T, A>::StrojarVector(int32 growSize, uint32 initSize)
{
	m_Memory = A(growSize, initSize);
}

template<class T, class A>
inline StrojarVector<T, A>::StrojarVector(T* pMemory, uint32 allocationCount, uint32 numElements)
{
	m_Memory(pMemory, allocationCount);
	m_Size = numElements;
}

template<class T, class A>
inline StrojarVector<T, A>::~StrojarVector()
{
	Purge();
}

template<class T, class A>
inline T& StrojarVector<T, A>::operator[](uint32 i)
{
	return m_Memory[i];
}

template<class T, class A>
inline T& StrojarVector<T, A>::Element(uint32 i)
{
	return m_Memory[i];
}

template<class T, class A>
inline T& StrojarVector<T, A>::Head()
{
	return m_Memory[0];
}

template<class T, class A>
inline T& StrojarVector<T, A>::Tail()
{
	return m_Memory[m_Size - 1];
}

template<class T, class A>
inline T* StrojarVector<T, A>::Base()
{
	return m_Memory.Base();
}

template<class T, class A>
inline uint32 StrojarVector<T, A>::Count()
{
	return m_Size;
}


template<class T, class A>
inline uint32 StrojarVector<T, A>::AddToHead()
{
	return InsertBefore(0);
}

template<class T, class A>
inline uint32 StrojarVector<T, A>::AddToTail()
{
	return InsertBefore(m_Size);
}

template<class T, class A>
inline uint32 StrojarVector<T, A>::InsertBefore(uint32 elem)
{
	GrowVector();
	ShiftElementsRight(elem);
	Construct(&Element(elem));

	return elem;
}

template<class T, class A>
inline uint32 StrojarVector<T, A>::InsertAfter(uint32 elem)
{
	return InsertBefore(elem + 1);
}

template<class T, class A>
inline uint32 StrojarVector<T, A>::AddToHead(const T& src)
{
	return InsertBefore(0, src);
}

template<class T, class A>
inline uint32 StrojarVector<T, A>::AddToTail(const T& src)
{
	return InsertBefore(m_Size, src);
}

template<class T, class A>
inline uint32 StrojarVector<T, A>::InsertBefore(uint32 elem, const T& src)
{
	GrowVector();
	ShiftElementsRight(elem);
	CopyConstruct(&Element(elem), src);

	return elem;
}

template<class T, class A>
inline uint32 StrojarVector<T, A>::InsertAfter(uint32 elem, const T& src)
{
	return InsertBefore(elem + 1, src);
}

template<class T, class A>
inline int32 StrojarVector<T, A>::Find(const T& src)
{
	uint32 nCount = Count();

	for (uint32 i = 0; i < nCount; ++i)
	{
		if (Element(i) == src)
		{
			return i;
		}
	}

	return -1;
}

template<class T, class A>
inline void StrojarVector<T, A>::Clear()
{
	m_Size = 0;
}

template<class T, class A>
inline void StrojarVector<T, A>::Remove(uint32 elem)
{
	Destruct(&Element(elem));
	ShiftElementsLeft(elem);
	--m_Size;
}

template<class T, class A>
inline bool StrojarVector<T, A>::FindAndRemove(const T& src)
{
	int32 elem = Find(src);

	if (elem != -1)
	{
		Remove(elem);
		return true;
	}

	return false;
}

template<class T, class A>
inline void StrojarVector<T, A>::RemoveAll()
{
	for (int i = m_Size; --i >= 0; )
	{
		Destruct(&Element(i));
	}

	m_Size = 0;
}

template<class T, class A>
inline void StrojarVector<T, A>::RemoveFromHead()
{
	Remove(1);
}

template<class T, class A>
inline void StrojarVector<T, A>::RemoveFromTail()
{
	Remove(Count());
}

template<class T, class A>
inline void StrojarVector<T, A>::Purge()
{
	RemoveAll();
	m_Memory.Purge();
}



template<class T, class A>
inline void StrojarVector<T, A>::GrowVector(uint32 num)
{
	if (m_Size + num > m_Memory.NumAllocated())
	{
		m_Memory.Grow(m_Size + num - m_Memory.NumAllocated());
	}

	m_Size += num;
}

template<class T, class A>
inline void StrojarVector<T, A>::ShiftElementsRight(uint32 elem, uint32 num)
{
	uint32 numToMove = m_Size - elem - num;

	if ((numToMove > 0) && (num > 0))
	{
		memmove(&Element(elem + num), &Element(elem), numToMove * sizeof(T));
	}
}

template<class T, class A>
inline void StrojarVector<T, A>::ShiftElementsLeft(uint32 elem, uint32 num)
{
	uint32 numToMove = m_Size - elem - num;

	if ((numToMove > 0) && (num > 0))
	{
		memmove(&Element(elem), &Element(elem + num), numToMove * sizeof(T));
	}
}

#endif