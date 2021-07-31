#include "netvarmgr.h"

RazeNetvarManager::TableProps::TableProps() :
	props(0, 0)
{
}

RazeNetvarManager::RazeNetvarManager() :
	tables(0, 0)
{
}

RazeNetvarManager::TableProps* RazeNetvarManager::FindTable(const char* tbl)
{
	for (TablePropsNode* tblPropsNode : tables)
	{
		if (!strcmp(tblPropsNode->name, tbl))
		{
			return tblPropsNode->tableProps;
		}
	}

	return 0;
}

void RazeNetvarManager::AddTable(RecvTable* tbl)
{
	TableProps* tblProps = new TableProps;
	tblProps->table = tbl;
	
	TablePropsNode* tblPropsNode = new TablePropsNode;
	strcpy(tblPropsNode->name, tbl->m_pNetTableName);
	tblPropsNode->tableProps = tblProps;

	for (int32 i = 0; i < tbl->m_nProps; i++)
	{
		RecvProp* prop = &tbl->m_pProps[i];

		if (!prop || isdigit(prop->m_pVarName[0]))
		{
			continue;
		}

		if (!strcmp(prop->m_pVarName, "baseclass"))
		{
			continue;
		}

		if (prop->m_RecvType == DPT_DataTable && prop->m_pDataTable != nullptr && prop->m_pDataTable->m_pNetTableName[0] == 'D')
		{
			AddTable(prop->m_pDataTable);
		}
		
		tblProps->props.AddToTail(prop);
	}

	tables.AddToTail(tblPropsNode);
}

uint32 RazeNetvarManager::GetOffset(const char* tbl, const char* prop)
{
	TableProps* tblProps = FindTable(tbl);
	if (tblProps)
	{
		for (RecvProp* prp : tblProps->props)
		{
			if (!strcmp(prp->m_pVarName, prop))
			{
				return prp->m_Offset;
			}
		}
	}

	return 0;
}

#undef GetProp
RecvProp* RazeNetvarManager::GetProp(const char* tbl, const char* prop)
{
	TableProps* tblProps = FindTable(tbl);
	if (tblProps)
	{
		for (RecvProp* prp : tblProps->props)
		{
			if (!strcmp(prp->m_pVarName, prop))
			{
				return prp;
			}
		}
	}

	return 0;
}

void RazeNetvarManager::Dump()
{
	FILE* pFile = fopen("netvar_dump.txt", "w");
	if (!pFile)
	{
		return;
	}

	char szBuffer[1024];

	for(TablePropsNode* tblPropNode : tables)
	{
		sprintf(szBuffer, "%s\n", tblPropNode->name);
		fwrite(szBuffer, sizeof(char), strlen(szBuffer), pFile);

		for (RecvProp* prop : tblPropNode->tableProps->props)
		{
			sprintf(szBuffer, "\t%s [%04X]\n", prop->m_pVarName, prop->m_Offset);
			fwrite(szBuffer, sizeof(char), strlen(szBuffer), pFile);
		}
	}

	fclose(pFile);
}