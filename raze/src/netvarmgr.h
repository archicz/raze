#ifndef RAZE_NETVARMGR_H
#define RAZE_NETVARMGR_H
#pragma once

#include <strojar/vector.h>
#include <sdk/client_class.h>

class RazeNetvarManager
{
public:
	class TableProps
	{
	public:
		TableProps();

		RecvTable* table;
		StrojarVector<RecvProp*> props;
	};

	struct TablePropsNode
	{
		char name[128];
		TableProps* tableProps;
	};

	StrojarVector<TablePropsNode*> tables;
public:
	RazeNetvarManager();

	TableProps* FindTable(const char* tbl);
	void AddTable(RecvTable* tbl);
	
	uint32 GetOffset(const char* tbl, const char* prop);
	RecvProp* GetProp(const char* tbl, const char* prop);

	void Dump();
};

extern RazeNetvarManager* g_pNetvarMgr;

#endif