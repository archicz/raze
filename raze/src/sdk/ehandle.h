#ifndef RAZE_SDK_EHANDLE_H
#define RAZE_SDK_EHANDLE_H
#pragma once

#include <sdk/basehandle.h>

template< class T >
class CHandle : public CBaseHandle
{
public:
	T* Get() const;
	void Set(const T* pVal);

	bool operator !() const;
	bool operator==(T* val) const;
	bool operator!=(T* val) const;
	CHandle& operator=(const T* val);

	T* operator->() const;
};

template<class T>
inline T* CHandle<T>::Get() const
{
	return (T*)CBaseHandle::Get();
}

template<class T>
inline bool CHandle<T>::operator !() const
{
	return !Get();
}

template<class T>
inline bool CHandle<T>::operator==(T* val) const
{
	return Get() == val;
}

template<class T>
inline bool CHandle<T>::operator!=(T* val) const
{
	return Get() != val;
}

template<class T>
void CHandle<T>::Set(const T* pVal)
{
	const IHandleEntity* pValInterface = (IHandleEntity*)pVal;
	CBaseHandle::Set(pValInterface);
}

template<class T>
inline CHandle<T>& CHandle<T>::operator=(const T* val)
{
	Set(val);
	return *this;
}

template<class T>
T* CHandle<T>::operator->() const
{
	return Get();
}

#endif