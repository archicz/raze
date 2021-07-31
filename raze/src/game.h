#ifndef RAZE_GAME_H
#define RAZE_GAME_H
#pragma once

#include <raze/memory.h>

extern ModuleContext* engineDll;
extern ModuleContext* clientDll;
extern ModuleContext* tier0Dll;
extern ModuleContext* shaderapidx9Dll;

#include <sdk/interface.h>
extern FnCreateInterface engineFactory;
extern FnCreateInterface clientFactory;

#include <sdk/IVEngineClient.h>
extern IVEngineClient* g_pEngine;

#include <sdk/IBaseClientDLL.h>
extern IBaseClientDLL* g_pClient;

#include <sdk/IClientMode.h>
extern IClientMode* g_pClientMode;

#include <sdk/CGlobalVarsBase.h>
extern CGlobalVarsBase* g_pGlobals;

#include <sdk/IClientEntityList.h>
extern IClientEntityList* g_pEntityList;

#include <raze/renderer.h>
#include <raze/gui.h>
#define PRESENT_INDEX 17
#define RESET_INDEX 16
extern HWND g_hWindow;
extern IDirect3DDevice9* g_pDevice;
extern RazeRenderer* g_pRenderer;
extern RazeGUIManager* g_pGUI;

typedef int32(__fastcall* FnSendClanTag)(const char*, const char*);
extern FnSendClanTag SendClanTag;

typedef void(*FnMsg)(const char*, ...);
extern FnMsg Msg;

// link/unlink all
bool LinkGame();
void UnlinkGame();

#endif