#ifndef RAZE_SDK_IVENGINECLIENT_H
#define RAZE_SDK_IVENGINECLIENT_H
#pragma once

#include <sdk/const.h>
#include <sdk/interface.h>

struct player_info_t
{
	uint64 version;
	uint64 xuid;
	char name[MAX_PLAYER_NAME_LENGTH];
	int32 userID;
	char guid[SIGNED_GUID_LEN + 1];
	uint32 friendsID;
	char friendsName[MAX_PLAYER_NAME_LENGTH];
	bool fakeplayer;
	bool ishltv;
};

class INetChannelInfo;

class IVEngineClient
{
public:
	struct VTableFunctions
	{
		VTableContext* ctx = 0;

		typedef bool(__thiscall* FnGetPlayerInfo)(void*, int32, player_info_t*);
		uint32 indexGetPlayerInfo = 8;
		FnGetPlayerInfo GetPlayerInfo = 0;

		typedef int32(__thiscall* FnGetLocalPlayer)(void*);
		uint32 indexGetLocalPlayer = 12;
		FnGetLocalPlayer GetLocalPlayer = 0;

		typedef bool(__thiscall* FnIsInGame)(void*);
		uint32 indexIsInGame = 26;
		FnIsInGame IsInGame = 0;

		typedef bool(__thiscall* FnIsConnected)(void*);
		uint32 indexIsConnected = 27;
		FnIsConnected IsConnected = 0;

		typedef INetChannelInfo* (__thiscall* FnGetNetChannelInfo)(void*);
		uint32 indexGetNetChannelInfo = 78;
		FnGetNetChannelInfo GetNetChannelInfo = 0;

		void Link(void* vt)
		{
			ctx = CreateVTableContext(vt);

			VFUNC_MACRO(GetPlayerInfo);
			VFUNC_MACRO(GetLocalPlayer);
			VFUNC_MACRO(IsInGame);
			VFUNC_MACRO(IsConnected);
			VFUNC_MACRO(GetNetChannelInfo);
		}

		void Cleanup()
		{
			delete ctx->vtableCopy;
			delete ctx;
		}
	};

	static VTableFunctions* vfuncs;
public:
	bool GetPlayerInfo(int32 index, player_info_t* info)
	{
		return vfuncs->GetPlayerInfo(this, index, info);
	}

	int32 GetLocalPlayer()
	{
		return vfuncs->GetLocalPlayer(this);
	}

	bool IsInGame()
	{
		return vfuncs->IsInGame(this);
	}

	bool IsConnected()
	{
		return vfuncs->IsConnected(this);
	}

	INetChannelInfo* GetNetChannelInfo()
	{
		return vfuncs->GetNetChannelInfo(this);
	}
};

#endif