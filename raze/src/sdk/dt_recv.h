#ifndef RAZE_SDK_DTRECV_H
#define RAZE_SDK_DTRECV_H
#pragma once

#include <raze/common.h>
#include <string>

class RecvTable;
class RecvProp;

enum SendPropType
{
	DPT_Int = 0,
	DPT_Float,
	DPT_Vector,
	DPT_VectorXY,
	DPT_String,
	DPT_Array,
	DPT_DataTable,
	DPT_Int64,
	DPT_NUMSendPropTypes
};

class DVariant
{
public:
	union
	{
		float m_Float;
		long m_Int;
		char* m_pString;
		void* m_pData;
		float m_Vector[3];
		int64 m_Int64;
	};
	SendPropType  m_Type;
};

class CRecvProxyData
{
public:
	RecvProp* m_pRecvProp;
	DVariant m_Value;
	int32 m_iElement;
	int32 m_ObjectID;
};

typedef void(*RecvVarProxyFn)(CRecvProxyData* pData, void* pStruct, void* pOut);
typedef void(*ArrayLengthRecvProxyFn)(void* pStruct, int32 objectID, int32 currentArrayLength);
typedef void(*DataTableRecvVarProxyFn)(RecvProp* pProp, void** pOut, void* pData, int32 objectID);

class RecvProp
{
public:
	char* m_pVarName;
	SendPropType m_RecvType;
	int32 m_Flags;
	int32 m_StringBufferSize;
	bool m_bInsideArray;
	const void* m_pExtraData;
	RecvProp* m_pArrayProp;
	ArrayLengthRecvProxyFn m_ArrayLengthProxy;
	RecvVarProxyFn m_ProxyFn;
	DataTableRecvVarProxyFn	m_DataTableProxyFn;
	RecvTable* m_pDataTable;
	int32 m_Offset;
	int32 m_ElementStride;
	int32 m_nElements;
	const char* m_pParentArrayPropName;
public:
	RecvVarProxyFn GetProxyFn() const;
	void SetProxyFn(RecvVarProxyFn fn);
	DataTableRecvVarProxyFn	GetDataTableProxyFn() const;
	void SetDataTableProxyFn(DataTableRecvVarProxyFn fn);
};

class RecvTable
{
public:
	RecvProp* m_pProps;
	int32 m_nProps;
	void* m_pDecoder;
	char* m_pNetTableName;
	bool m_bInitialized;
	bool m_bInMainList;
};

inline RecvVarProxyFn RecvProp::GetProxyFn() const
{
	return m_ProxyFn;
}

inline void RecvProp::SetProxyFn(RecvVarProxyFn fn)
{
	m_ProxyFn = fn;
}

inline DataTableRecvVarProxyFn RecvProp::GetDataTableProxyFn() const
{
	return m_DataTableProxyFn;
}

inline void RecvProp::SetDataTableProxyFn(DataTableRecvVarProxyFn fn)
{
	m_DataTableProxyFn = fn;
}

#endif