#include <Windows.h>

#include "game.h"
#include "netvarmgr.h"

RazeNetvarManager* g_pNetvarMgr = new RazeNetvarManager;

ModuleContext* engineDll = 0;
ModuleContext* clientDll = 0;
ModuleContext* tier0Dll = 0;
ModuleContext* shaderapidx9Dll = 0;

FnCreateInterface engineFactory = 0;
FnCreateInterface clientFactory = 0;

IVEngineClient* g_pEngine = 0;
IVEngineClient::VTableFunctions* IVEngineClient::vfuncs = 0;

IBaseClientDLL* g_pClient = 0;
IBaseClientDLL::VTableFunctions* IBaseClientDLL::vfuncs = 0;

IClientMode* g_pClientMode = 0;
IClientMode::VTableFunctions* IClientMode::vfuncs = 0;

CGlobalVarsBase* g_pGlobals = 0;

IClientEntityList* g_pEntityList = 0;
IClientEntityList::VTableFunctions* IClientEntityList::vfuncs = 0;

typedef HRESULT(WINAPI* FnPresent)(IDirect3DDevice9*, const RECT*, const RECT*, HWND, const RGNDATA*);
FnPresent origPresent = 0;

typedef HRESULT(WINAPI* FnReset)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
FnReset origReset = 0;

HWND g_hWindow = 0;
WNDPROC origWinProc = 0;

IDirect3DDevice9* g_pDevice = 0;
VTableContext* g_pDeviceVTable = 0;

RazeRenderer* g_pRenderer = 0;
RazeGUIManager* g_pGUI = 0;

FnSendClanTag SendClanTag = 0;

FnMsg Msg = 0;

static void __fastcall FrameStageNotify(void*, int, ClientFrameStage_t stage)
{
	return g_pClient->FrameStageNotify(stage);
}

static bool __fastcall CreateMove(void*, int, float inputSampleTime, CUserCmd* cmd)
{
	uintp* stackFrame;
	__asm mov stackFrame, ebp;
	bool& sendPacket = *(bool*)(*stackFrame - 0x1C);
	
	return g_pClientMode->CreateMove(inputSampleTime, cmd);
}

HRESULT WINAPI HookPresent(IDirect3DDevice9* device, const RECT* source, const RECT* dest, HWND win, const RGNDATA* dirty)
{
	IDirect3DStateBlock9* stateBlock = 0;
	IDirect3DVertexDeclaration9* vertDec = 0;

	g_pDevice->GetVertexDeclaration(&vertDec);
	g_pDevice->CreateStateBlock(D3DSBT_PIXELSTATE, &stateBlock);

	g_pRenderer->Begin();
		g_pGUI->Render();
	g_pRenderer->End();
	
	stateBlock->Apply();
	stateBlock->Release();
	g_pDevice->SetVertexDeclaration(vertDec);

	return origPresent(device, source, dest, win, dirty);
}

HRESULT WINAPI HookReset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* params)
{
	return origReset(device, params);
}

LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (g_pGUI)
	{
		g_pGUI->WinProc(hWnd, message, wParam, lParam);
	}

	return CallWindowProcA(origWinProc, hWnd, message, wParam, lParam);
}

bool blblb = false;
float pitch = 0.f;
float yaw = 0.f;
float absol = 0.f;
float decim = 0.f;
int32 rating = 0;

void SetupMenu()
{
	RazeGUIFrame* loginFrame = g_pGUI->CreateFrame("loginFrame");
	loginFrame->SetSize(450, 500);
	loginFrame->SetHitTest(0, 0, 0, 30);
	loginFrame->ScreenCenter();

	RazeGUISection* topbar = loginFrame->CreateChildPanel<RazeGUISection>();
	topbar->SetSize(0, 30);
	topbar->SetPadding(0, 0, 0, 0);

	uint32 cw2 = loginFrame->GetContentsWidth() / 2;

	RazeGUISection* s_stars = loginFrame->CreateChildPanel<RazeGUISection>();
	s_stars->SetSize(cw2, 250);
	s_stars->SetPadding(-1, -1, -1, -1);
	s_stars->SetTitle(L"Stars");
	s_stars->SetCenterTitle(true);

	RazeGUIButton* b_resetStars = s_stars->CreateChildPanel<RazeGUIButton>();
	b_resetStars->SetSize(0, FONTSIZE_TAHOMANORMAL + RAZE_TEXTPADDING + RAZE_PADDING * 2); // text + sides + padding * 2
	b_resetStars->SetPadding(-1, -1, -1, -1);
	//b_resetStars->OnClick(ResetStars);
	b_resetStars->SetTitle(L"Reset Stars");

	RazeGUICheckBox* ch_redStars = s_stars->CreateChildPanel<RazeGUICheckBox>();
	ch_redStars->SetSize(0, FONTSIZE_TAHOMANORMAL + RAZE_TEXTPADDING + RAZE_PADDING); // text + sides + padding
	ch_redStars->SetPadding(-1, 0, -1, -1);
	ch_redStars->SetTitle(L"Red stars");
	ch_redStars->SetToggle(&blblb);

	RazeGUISlider* sl_pitch = s_stars->CreateChildPanel<RazeGUISlider>();
	sl_pitch->SetSize(0, FONTSIZE_TAHOMANORMAL + RAZE_TEXTPADDING + RAZE_PADDING);
	sl_pitch->SetPadding(-1, 0, -1, -1);
	sl_pitch->SetTitle(L"Pitch");
	sl_pitch->SetValue(&pitch);
	sl_pitch->SetMinMax(-90.f, 90.f);
	sl_pitch->SetDefaultValue(0.f);
	sl_pitch->SetValueType(VALUETYPE_ANGLE);

	RazeGUISlider* sl_yaw = s_stars->CreateChildPanel<RazeGUISlider>();
	sl_yaw->SetSize(0, FONTSIZE_TAHOMANORMAL + RAZE_TEXTPADDING + RAZE_PADDING);
	sl_yaw->SetPadding(-1, 0, -1, -1);
	sl_yaw->SetTitle(L"Yaw");
	sl_yaw->SetValue(&yaw);
	sl_yaw->SetMinMax(-360.f, 360.f);
	sl_yaw->SetDefaultValue(0.f);
	sl_yaw->SetValueType(VALUETYPE_ANGLE);

	RazeGUISlider* sl_abs = s_stars->CreateChildPanel<RazeGUISlider>();
	sl_abs->SetSize(0, FONTSIZE_TAHOMANORMAL + RAZE_TEXTPADDING + RAZE_PADDING);
	sl_abs->SetPadding(-1, 0, -1, -1);
	sl_abs->SetTitle(L"Absolute number");
	sl_abs->SetValue(&absol);
	sl_abs->SetMinMax(0.f, 1337.f);
	sl_abs->SetDefaultValue(0.f);
	sl_abs->SetValueType(VALUETYPE_ABSOLUTE);

	RazeGUISlider* sl_deci = s_stars->CreateChildPanel<RazeGUISlider>();
	sl_deci->SetSize(0, FONTSIZE_TAHOMANORMAL + RAZE_TEXTPADDING + RAZE_PADDING);
	sl_deci->SetPadding(-1, 0, -1, -1);
	sl_deci->SetTitle(L"Decimal number");
	sl_deci->SetValue(&decim);
	sl_deci->SetMinMax(0.f, 1337.f);
	sl_deci->SetDefaultValue(0.f);
	sl_deci->SetValueType(VALUETYPE_DECIMAL);

	RazeGUIComboBox* cb_test = s_stars->CreateChildPanel<RazeGUIComboBox>();
	cb_test->SetSize(0, FONTSIZE_TAHOMANORMAL * 2 + RAZE_TEXTPADDING + RAZE_PADDING);
	cb_test->SetPadding(-1, 0, -1, -1);
	cb_test->SetTitle(L"How do you rate RazeGUI?");
	cb_test->SetValue(&rating);

	cb_test->AddItem(L"Very nice", 1);
	cb_test->AddItem(L"Good", 2);
	cb_test->AddItem(L"Meh", 3);
	cb_test->AddItem(L"ImGui better", 4);
	cb_test->AddItem(L"Bad", 5);

	RazeGUIButton* b_tester = s_stars->CreateChildPanel<RazeGUIButton>();
	b_tester->SetSize(0, FONTSIZE_TAHOMANORMAL + RAZE_TEXTPADDING + RAZE_PADDING);
	b_tester->SetPadding(-1, 0, -1, -1);
	//b_tester->OnClick(ResetStars);
	b_tester->SetTitle(L"Padding tester");
}

bool LinkGame()
{
	while (!(g_hWindow = FindWindow(L"Valve001", 0)))
	{
		Sleep(50);
	}

	origWinProc = (WNDPROC)SetWindowLongPtr(g_hWindow, GWLP_WNDPROC, (LONG)WinProc);

	// tier0.dll
	if (tier0Dll = CreateModuleContext("tier0.dll"))
	{
		Msg = (FnMsg)GetProcAddress(tier0Dll->handle, "Msg");
		if (!Msg)
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	// shaderapidx9.dll
	if (shaderapidx9Dll = CreateModuleContext("shaderapidx9.dll"))
	{
		char* deviceString = (char*)StringScan(shaderapidx9Dll->addr, shaderapidx9Dll->size, "Video driver has crashed and been reset", false);
		if (deviceString)
		{
			uint8* deviceStringxref = PointerXRef(shaderapidx9Dll->addr, shaderapidx9Dll->size, deviceString);
			if (deviceStringxref)
			{
				uint8* movDeviceInstr = PatternScanReverse(deviceStringxref, 512, "A1 ?? ?? ?? ??");
				if (movDeviceInstr)
				{
					++movDeviceInstr;

					g_pDevice = **(IDirect3DDevice9***)movDeviceInstr;

					g_pDeviceVTable = CreateVTableContext(g_pDevice);
					origPresent = (FnPresent)GetOriginalVTableMethod(g_pDeviceVTable, PRESENT_INDEX);
					origReset = (FnReset)GetOriginalVTableMethod(g_pDeviceVTable, RESET_INDEX);
					HookVTableMethod(g_pDeviceVTable, HookPresent, PRESENT_INDEX);
					HookVTableMethod(g_pDeviceVTable, HookReset, RESET_INDEX);
					SwapVTableCopy(g_pDeviceVTable);

					g_pRenderer = new RazeRenderer(g_pDevice);
					g_pGUI = new RazeGUIManager(g_pRenderer);
					
					SetupMenu();
				}
			}
		}
	}

	// engine.dll
	if (engineDll = CreateModuleContext("engine.dll"))
	{
		engineFactory = (FnCreateInterface)GetProcAddress(engineDll->handle, "CreateInterface");
		if (engineFactory)
		{
			// IVEngineClient -> g_pEngine
			char* engineClientVersion = (char*)StringScan(engineDll->addr, engineDll->size, "VEngineClient0??", true);
			if (engineClientVersion)
			{
				g_pEngine = (IVEngineClient*)engineFactory(engineClientVersion, 0);
				if (g_pEngine)
				{
					g_pEngine->vfuncs = new IVEngineClient::VTableFunctions;
					g_pEngine->vfuncs->Link(g_pEngine);
				}
				else
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	// client.dll
	if (clientDll = CreateModuleContext("client.dll"))
	{
		clientFactory = (FnCreateInterface)GetProcAddress(clientDll->handle, "CreateInterface");
		if (clientFactory)
		{
			// IBaseClientDLL -> g_pClient
			char* clientVersion = (char*)StringScan(clientDll->addr, clientDll->size, "VClient0??", true);
			if (clientVersion)
			{
				g_pClient = (IBaseClientDLL*)clientFactory(clientVersion, 0);
				if (g_pClient)
				{
					g_pClient->vfuncs = new IBaseClientDLL::VTableFunctions;
					g_pClient->vfuncs->Link(g_pClient);
					g_pClient->vfuncs->Hook(FrameStageNotify, g_pClient->vfuncs->indexFrameStageNotify);
				}
				else
				{
					return false;
				}
			}
			else
			{
				return false;
			}

			// IClientMode -> g_pClientMode
			g_pClientMode = **(IClientMode***)((uint8*)GetOriginalVTableMethod(g_pClient->vfuncs->ctx, 10) + 0x5);
			if (g_pClientMode)
			{
				g_pClientMode->vfuncs = new IClientMode::VTableFunctions;
				g_pClientMode->vfuncs->Link(g_pClientMode);
				g_pClientMode->vfuncs->Hook(CreateMove, g_pClientMode->vfuncs->indexCreateMove);
			}
			else
			{
				return false;
			}

			// CGlobalVarsBase -> g_pGlobals
			uint8* moveGlobalsInstr = PatternScan(GetOriginalVTableMethod(g_pClient->vfuncs->ctx, 0), 32, "A3 ?? ?? ?? ??");
			if (moveGlobalsInstr)
			{
				moveGlobalsInstr++;
				g_pGlobals = **(CGlobalVarsBase***)moveGlobalsInstr;
				if (!g_pGlobals)
				{
					return false;
				}
			}
			else
			{
				return false;
			}

			// IClientEntityList -> g_pEntityList
			char* entityListVersion = (char*)StringScan(clientDll->addr, clientDll->size, "VClientEntityList0??", true);
			if (entityListVersion)
			{
				g_pEntityList = (IClientEntityList*)clientFactory(entityListVersion, 0);
				if (g_pEntityList)
				{
					g_pEntityList->vfuncs = new IClientEntityList::VTableFunctions;
					g_pEntityList->vfuncs->Link(g_pEntityList);
				}
				else
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
	
	for (ClientClass* clClass = g_pClient->GetAllClasses(); clClass != 0; clClass = clClass->m_pNext)
	{
		if (clClass->m_pRecvTable)
		{
			g_pNetvarMgr->AddTable(clClass->m_pRecvTable);
		}
	}

	// For dumping netvars
	//g_pNetvarMgr->Dump();

	g_pClient->vfuncs->ApplyHooks();
	g_pClientMode->vfuncs->ApplyHooks();

	return true;
}

void UnlinkGame()
{
	g_pClient->vfuncs->RestoreHooks();
	g_pClientMode->vfuncs->RestoreHooks();

	g_pEngine->vfuncs->Cleanup();
	g_pClient->vfuncs->Cleanup();
	g_pClientMode->vfuncs->Cleanup();
	g_pEntityList->vfuncs->Cleanup();
}

// g_pClient[16] (IN_ActivateMouse) + 0x2 = input [client.dll]
// xref string (Player.Swim), reverse pattern search "8D 0D ?? ?? ?? ??" (mov ecx, movehelper) + 0x2 = movehelper [client.dll]
// find function by string xref (#int#) = KeyValues::FromString [client.dll]

inline IHandleEntity* CBaseHandle::Get() const
{
	return g_pEntityList->GetClientEntityFromHandle(*this);
}