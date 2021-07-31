#ifndef RAZE_RENDERER_H
#define RAZE_RENDERER_H
#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#include <raze/common.h>
#include <raze/color.h>
#include <strojar/vector.h>

#define RAZE_FVF (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)
#define RAZE_MAXVBOLEN 2048

struct RazeVertex
{
	float x, y, z, rhw;
	DWORD color;
	float tx, ty;

	RazeVertex(float _x, float _y, DWORD _color) :
		x(_x), y(_y), z(0.f), rhw(1.f), color(_color), tx(0.f), ty(0.f)
	{
	}

	RazeVertex(int32 _x, int32 _y, DWORD _color) :
		x((float)_x), y((float)_y), z(0.f), rhw(1.f), color(_color), tx(0.f), ty(0.f)
	{
	}

	RazeVertex(int32 _x, int32 _y, DWORD _color, float _tx, float _ty) :
		x((float)_x), y((float)_y), z(0.f), rhw(1.f), color(_color), tx(_tx), ty(_ty)
	{
	}
};

struct RazePoint
{
	int32 x;
	int32 y;

	RazePoint() :
		x(0), y(0)
	{
	}

	RazePoint(int32 _x, int32 _y) :
		x(_x), y(_y)
	{
	}

	bool IsZeroed()
	{
		return (x == 0 && y == 0);
	}
};

struct RazeRect
{
	int32 x;
	int32 y;
	int32 w;
	int32 h;

	RazeRect() :
		x(0), y(0), w(0), h(0)
	{
	}

	RazeRect(int32 _x, int32 _y) :
		x(_x), y(_y), w(0), h(0)
	{
	}

	RazeRect(int32 _x, int32 _y, int32 _w, int32 _h) :
		x(_x), y(_y), w(_w), h(_h)
	{
	}
	
	bool PointInside(int32 _x, int32 _y)
	{
		if (_x > (x + w)) return false;
		if (_x < x) return false;

		if (_y > (y + h)) return false;
		if (_y < y) return false;

		return true;
	}

	bool IsZeroed()
	{
		return (x == 0 && y == 0 && w == 0 && h == 0);
	}
};

class RazeRenderer
{
public:
	enum BatchType : uint8
	{
		BATCHTYPE_PRIMITIVE,
		BATCHTYPE_SPRITE
	};

	struct Batch
	{
		BatchType type;
		D3DPRIMITIVETYPE primType;
		LPDIRECT3DVERTEXBUFFER9 vbo;
		LPD3DXSPRITE sprite;
		LPDIRECT3DTEXTURE9 texture;
		uint32 count;
	};
public:
	IDirect3DDevice9* device;
	D3DVIEWPORT9 viewPort;
	RazeRect screenSize;

	StrojarVector<Batch*> batches;
	Batch* curBatch;

	StrojarVector<ID3DXFont*> fonts;
public:
	RazeRenderer(IDirect3DDevice9* dev);

	void Init(IDirect3DDevice9* dev);
	void SetRenderStates();
	void Reset(IDirect3DDevice9* dev);

	void Begin();
	void End();
public:
	void PushCurrentBatch();
	void SetCurrentBatch(Batch* btch);
	
	void MakePrimitiveBatch(D3DPRIMITIVETYPE type, IDirect3DTexture9* tx);
	void MakeSpriteBatch();

	void CheckPrimitiveBatch(D3DPRIMITIVETYPE desiredType, IDirect3DTexture9* tx);
	void CheckSpriteBatch();

	void AddPrimitivesToBatch(RazeVertex* vtx, uint32 byteLen);
public:
	uint32 GetScreenWidth();
	uint32 GetScreenHeight();
public:
	void DrawLine(int32 x0, int32 y0, int32 x1, int32 y1, Color clr);
	void DrawGradientLine(int32 x0, int32 y0, int32 x1, int32 y1, Color startClr, Color endClr);

	void DrawRect(int32 x, int32 y, int32 w, int32 h, Color clr);
	void DrawGradientRect(int32 x, int32 y, int32 w, int32 h, Color startClr, Color endClr);
	void DrawOutlinedRect(int32 x, int32 y, int32 w, int32 h, Color clr);
	void DrawTexturedRect(int32 x, int32 y, int32 w, int32 h, Color clr, IDirect3DTexture9* texture);
	void DrawTexturedGradientRect(int32 x, int32 y, int32 w, int32 h, Color startClr, Color endClr, IDirect3DTexture9* texture);

	uint32 AddFont(const wchar_t* fontName, uint32 height);
	RazeRect& FontTextSize(uint32 fontID, const wchar_t* str);
	void DrawString(uint32 fontID, int32 x, int32 y, const wchar_t* str, bool outline, DWORD flags, Color clr);
};

#endif