#ifndef RAZE_SDK_BASEHANDLE_H
#define RAZE_SDK_BASEHANDLE_H
#pragma once

#include <raze/common.h>
#include <sdk/const.h>
#include <sdk/IHandleEntity.h>

class CBaseHandle
{
public:
	uint32 m_Index;
public:
	bool operator !=(const CBaseHandle& other) const;
	bool operator ==(const CBaseHandle& other) const;
	bool operator ==(const IHandleEntity* pEnt) const;
	bool operator !=(const IHandleEntity* pEnt) const;
	bool operator <(const CBaseHandle& other) const;
	bool operator <(const IHandleEntity* pEnt) const;
	const CBaseHandle& operator=(const IHandleEntity* pEntity);

	const CBaseHandle& Set(const IHandleEntity* pEntity);
	IHandleEntity* Get() const;
};

inline bool CBaseHandle::operator !=(const CBaseHandle& other) const
{
	return m_Index != other.m_Index;
}

inline bool CBaseHandle::operator ==(const CBaseHandle& other) const
{
	return m_Index == other.m_Index;
}

inline bool CBaseHandle::operator ==(const IHandleEntity* pEnt) const
{
	return Get() == pEnt;
}

inline bool CBaseHandle::operator !=(const IHandleEntity* pEnt) const
{
	return Get() != pEnt;
}

inline bool CBaseHandle::operator <(const CBaseHandle& other) const
{
	return m_Index < other.m_Index;
}

inline bool CBaseHandle::operator <(const IHandleEntity* pEntity) const
{
	uint32 otherIndex = (pEntity) ? pEntity->GetRefEHandle().m_Index : INVALID_EHANDLE_INDEX;
	return m_Index < otherIndex;
}

inline const CBaseHandle& CBaseHandle::operator=(const IHandleEntity* pEntity)
{
	return Set(pEntity);
}

inline const CBaseHandle& CBaseHandle::Set(const IHandleEntity* pEntity)
{
	if (pEntity)
	{
		*this = pEntity->GetRefEHandle();
	}
	else
	{
		m_Index = INVALID_EHANDLE_INDEX;
	}

	return *this;
}

#endif