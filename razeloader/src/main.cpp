// include the basic windows header files and the Direct3D header file
#include <windows.h>
#include <windowsx.h>
#include <raze/renderer.h>
#include <raze/gui.h>

// global declarations
LPDIRECT3D9 d3d;    // the pointer to our Direct3D interface
LPDIRECT3DDEVICE9 d3ddev;    // the pointer to the device class

// function prototypes
void initD3D(HWND hWnd);    // sets up and initializes Direct3D
void render_frame(void);    // renders a single frame
void cleanD3D(void);    // closes Direct3D and releases memory

// the WindowProc function prototype
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

RazeRenderer* g_pRenderer = 0;
RazeGUIManager* g_pGUIMgr = 0;

#define WINDOW_W 1280
#define WINDOW_H 720

// the entry point for any Windows program
int WINAPI WinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow)
{
    HWND hWnd;
    WNDCLASSEX wc;

    ZeroMemory(&wc, sizeof(WNDCLASSEX));

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.lpszClassName = L"PojdMrdatVokno";

    RegisterClassEx(&wc);

    AllocConsole();
    freopen("CONOUT$", "wb", stdout);

    hWnd = CreateWindowExA(NULL,
        "PojdMrdatVokno",
        "RAZE LOADER",
        WS_OVERLAPPEDWINDOW,
        GetSystemMetrics(SM_CXSCREEN) / 2 - WINDOW_W / 2, GetSystemMetrics(SM_CYSCREEN) / 2 - WINDOW_H / 2,
        WINDOW_W, WINDOW_H,
        NULL,
        NULL,
        hInstance,
        NULL);

    ShowWindow(hWnd, nCmdShow);

    // set up and initialize Direct3D
    initD3D(hWnd);

    // enter the main loop:

    MSG msg;

    while (TRUE)
    {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (msg.message == WM_QUIT)
            break;

        render_frame();
    }

    // clean up DirectX and COM
    cleanD3D();

    return msg.wParam;
}


// this is the main message handler for the program
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        break;
    }

    if (g_pGUIMgr)
    {
        g_pGUIMgr->WinProc(hWnd, message, wParam, lParam);
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

#include <ctime>
#include <stdlib.h>

struct Star
{
    int32 x;
    int32 y;
    uint32 speed;
};

#define NUMSTARS 76
Star stars[NUMSTARS];

void InitRandom()
{
    for (uint32 i = 0; i < 20; i++)
    {
        srand((uint32)time(0) * rand());
    }
}

uint32 RandomSpeed()
{
    srand((uint32)time(0) * rand());
    return 1 + (rand() % 20);
}

void InitStars()
{
    for (uint32 i = 0; i < NUMSTARS; i++)
    {
        uint32 rSpeed = RandomSpeed();

        stars[i].x = rSpeed;
        stars[i].y = 5 + (i * 10);
        stars[i].speed = rSpeed;
    }
}

void UpdateStars()
{
    for (uint32 i = 0; i < NUMSTARS; i++)
    {
        if (stars[i].x > g_pRenderer->GetScreenWidth())
        {
            uint32 rSpeed = RandomSpeed();

            stars[i].x = rSpeed;
            stars[i].speed = rSpeed;
        }

        stars[i].x += stars[i].speed;

        uint32 sx = stars[i].x;
        uint32 sy = stars[i].y;
        uint32 sp = stars[i].speed;
        g_pRenderer->DrawGradientLine(sx - sp, sy, sx, sy, Color(0, 255), Color(255, 255));
    }
}

void ResetStars(RazeGUIButton* btn)
{
    puts("ResetStars");

    for (uint32 i = 0; i < NUMSTARS; i++)
    {
        stars[i].x = 0;
    }
}

uint32 mujFont = 0;
IDirect3DTexture9* textura = 0;

bool blblb = false;
float pitch = 0.f;
float yaw = 0.f;
float absol = 0.f;
float decim = 0.f;

int32 rating = 0;

#include "hackerjpg.h"

// this function initializes and prepares Direct3D for use
void initD3D(HWND hWnd)
{
    d3d = Direct3DCreate9(D3D_SDK_VERSION);    // create the Direct3D interface

    D3DPRESENT_PARAMETERS d3dpp;    // create a struct to hold various device information

    ZeroMemory(&d3dpp, sizeof(d3dpp));    // clear out the struct for use
    d3dpp.Windowed = TRUE;    // program windowed, not fullscreen
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;    // discard old frames
    d3dpp.hDeviceWindow = hWnd;    // set the window to be used by Direct3D


    // create a device class using this information and the info from the d3dpp stuct
    d3d->CreateDevice(D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        hWnd,
        D3DCREATE_SOFTWARE_VERTEXPROCESSING,
        &d3dpp,
        &d3ddev);

    g_pRenderer = new RazeRenderer(d3ddev);

    InitRandom();
    InitStars();
    
    mujFont = g_pRenderer->AddFont(L"Tahoma", 40);
    D3DXCreateTextureFromFileInMemory(d3ddev, hackerjpg, hackerjpg_length, &textura);

    g_pGUIMgr = new RazeGUIManager(g_pRenderer);

    RazeGUIFrame* loginFrame = g_pGUIMgr->CreateFrame("loginFrame");
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
    b_resetStars->OnClick(ResetStars);
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

// this is the function used to render a single frame
void render_frame(void)
{
    // clear the window to a deep blue
    d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_RGBA(0, 0, 0, 255), 1.0f, 0);

    d3ddev->BeginScene();    // begins the 3D scene

    g_pRenderer->Begin();
        UpdateStars();

        g_pGUIMgr->Render();
    g_pRenderer->End();

    d3ddev->EndScene();    // ends the 3D scene

    d3ddev->Present(NULL, NULL, NULL, NULL);   // displays the created frame on the screen
}

// this is the function that cleans up Direct3D and COM
void cleanD3D(void)
{
    d3ddev->Release();    // close and release the 3D device
    d3d->Release();    // close and release Direct3D
}