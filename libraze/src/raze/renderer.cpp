#include "renderer.h"

RazeRenderer::RazeRenderer(IDirect3DDevice9* dev) :
	device(0), batches(0, 0), curBatch(0), fonts(0, 0)
{
	Init(dev);
}

void RazeRenderer::Init(IDirect3DDevice9* dev)
{
	device = dev;
	dev->GetViewport(&viewPort);

	IDirect3DSurface9* surface;
	dev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &surface);

	D3DSURFACE_DESC surfaceDesc;
	surface->GetDesc(&surfaceDesc);

	screenSize.x = 0;
	screenSize.y = 0;
	screenSize.w = surfaceDesc.Width;
	screenSize.h = surfaceDesc.Height;
}

void RazeRenderer::SetRenderStates()
{
	device->SetVertexShader(0);
	device->SetPixelShader(0);

	device->SetRenderState(D3DRS_LIGHTING, FALSE);
	device->SetRenderState(D3DRS_FOGENABLE, FALSE);
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	device->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	device->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
	device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	device->SetRenderState(D3DRS_STENCILENABLE, FALSE);

	device->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
	device->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, TRUE);

	device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	device->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, TRUE);
	device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	device->SetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_INVDESTALPHA);
	device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	device->SetRenderState(D3DRS_DESTBLENDALPHA, D3DBLEND_ONE);

	device->SetRenderState(D3DRS_SRGBWRITEENABLE, FALSE);
	device->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA);
}

void RazeRenderer::Reset(IDirect3DDevice9* dev)
{
	Init(dev);
}

void RazeRenderer::Begin()
{
	SetRenderStates();
	SetCurrentBatch(0);
}

void RazeRenderer::End()
{
	// push the last batch lol
	PushCurrentBatch();

	for (Batch* btch : batches)
	{
		if (btch->type == BATCHTYPE_PRIMITIVE)
		{
			uint32 primCount = 0;

			switch (btch->primType)
			{
				case D3DPT_LINELIST:
					primCount = btch->count / 2;
				break;
				case D3DPT_TRIANGLELIST:
					primCount = btch->count / 3;
				break;
			}
			
			device->SetFVF(RAZE_FVF);
			device->SetStreamSource(0, btch->vbo, 0, sizeof(RazeVertex));
			device->SetTexture(0, btch->texture);

			device->DrawPrimitive(btch->primType, 0, primCount);
			btch->vbo->Release();
		}

		if (btch->type == BATCHTYPE_SPRITE)
		{
			btch->sprite->End();
			btch->sprite->Release();
		}
	}

	SetCurrentBatch(0);
	batches.RemoveAll();
}

void RazeRenderer::PushCurrentBatch()
{
	batches.AddToTail(curBatch);
	SetCurrentBatch(0);
}

void RazeRenderer::SetCurrentBatch(Batch* btch)
{
	curBatch = btch;
}

void RazeRenderer::MakePrimitiveBatch(D3DPRIMITIVETYPE type, IDirect3DTexture9* tx)
{
	Batch* btch = new Batch;
	btch->type = BATCHTYPE_PRIMITIVE;
	btch->primType = type;
	btch->sprite = 0;
	device->CreateVertexBuffer(RAZE_MAXVBOLEN * sizeof(RazeVertex), D3DUSAGE_WRITEONLY, RAZE_FVF, D3DPOOL_DEFAULT, &btch->vbo, 0);
	btch->texture = tx;
	btch->count = 0;

	SetCurrentBatch(btch);
}

void RazeRenderer::MakeSpriteBatch()
{
	Batch* btch = new Batch;
	btch->type = BATCHTYPE_SPRITE;
	//btch->primType = 0;
	D3DXCreateSprite(device, &btch->sprite);
	btch->vbo = 0;
	btch->texture = 0;
	btch->count = 0;

	SetCurrentBatch(btch);
	curBatch->sprite->Begin(D3DXSPRITE_ALPHABLEND);
}

void RazeRenderer::CheckPrimitiveBatch(D3DPRIMITIVETYPE desiredType, IDirect3DTexture9* tx)
{
	if (!curBatch)
	{
		MakePrimitiveBatch(desiredType, tx);
		return;
	}

	if (curBatch->type != BATCHTYPE_PRIMITIVE)
	{
		PushCurrentBatch();
		MakePrimitiveBatch(desiredType, tx);
	}

	if (curBatch->primType != desiredType)
	{
		PushCurrentBatch();
		MakePrimitiveBatch(desiredType, tx);
	}

	if (curBatch->texture != tx)
	{
		PushCurrentBatch();
		MakePrimitiveBatch(desiredType, tx);
	}
}

void RazeRenderer::CheckSpriteBatch()
{
	if (!curBatch)
	{
		MakeSpriteBatch();
		return;
	}

	if (curBatch->type != BATCHTYPE_SPRITE)
	{
		PushCurrentBatch();
		MakeSpriteBatch();
	}
}

void RazeRenderer::AddPrimitivesToBatch(RazeVertex* vtx, uint32 byteLen)
{
	void* vboData;

	curBatch->vbo->Lock(curBatch->count * sizeof(RazeVertex), byteLen, (void**)&vboData, D3DLOCK_DISCARD);
	memcpy(vboData, vtx, byteLen);
	curBatch->vbo->Unlock();

	curBatch->count += byteLen / sizeof(RazeVertex);
}

uint32 RazeRenderer::GetScreenWidth()
{
	return screenSize.w;
}

uint32 RazeRenderer::GetScreenHeight()
{
	return screenSize.h;
}

void RazeRenderer::DrawLine(int32 x0, int32 y0, int32 x1, int32 y1, Color clr)
{
	DWORD color = D3DCOLOR_RGBA(clr.r, clr.g, clr.b, clr.a);

	RazeVertex vtx[] =
	{
		{ x0, y0, color},
		{ x1, y1, color}
	};

	CheckPrimitiveBatch(D3DPT_LINELIST, 0);
	AddPrimitivesToBatch(vtx, sizeof(vtx));
}

void RazeRenderer::DrawGradientLine(int32 x0, int32 y0, int32 x1, int32 y1, Color startClr, Color endClr)
{
	DWORD color1 = D3DCOLOR_RGBA(startClr.r, startClr.g, startClr.b, startClr.a);
	DWORD color2 = D3DCOLOR_RGBA(endClr.r, endClr.g, endClr.b, endClr.a);

	RazeVertex vtx[] =
	{
		{ x0, y0, color1},
		{ x1, y1, color2}
	};

	CheckPrimitiveBatch(D3DPT_LINELIST, 0);
	AddPrimitivesToBatch(vtx, sizeof(vtx));
}

void RazeRenderer::DrawRect(int32 x, int32 y, int32 w, int32 h, Color clr)
{
	DWORD color = D3DCOLOR_RGBA(clr.r, clr.g, clr.b, clr.a);

	RazeVertex vtx[] =
	{
		{ x, y, color },
		{ x + w, y, color },
		{ x, y + h, color },

		{ x + w, y, color },
		{ x + w, y + h, color },
		{ x, y + h, color }
	};

	CheckPrimitiveBatch(D3DPT_TRIANGLELIST, 0);
	AddPrimitivesToBatch(vtx, sizeof(vtx));
}

void RazeRenderer::DrawGradientRect(int32 x, int32 y, int32 w, int32 h, Color startClr, Color endClr)
{
	DWORD color1 = D3DCOLOR_RGBA(startClr.r, startClr.g, startClr.b, startClr.a);
	DWORD color2 = D3DCOLOR_RGBA(endClr.r, endClr.g, endClr.b, endClr.a);

	RazeVertex vtx[] =
	{
		{ x, y, color1 },
		{ x + w, y, color1 },
		{ x, y + h, color2 },

		{ x + w, y, color1 },
		{ x + w, y + h, color2 },
		{ x, y + h, color2 }
	};

	CheckPrimitiveBatch(D3DPT_TRIANGLELIST, 0);
	AddPrimitivesToBatch(vtx, sizeof(vtx));
}

void RazeRenderer::DrawOutlinedRect(int32 x, int32 y, int32 w, int32 h, Color clr)
{
	DWORD color = D3DCOLOR_RGBA(clr.r, clr.g, clr.b, clr.a);

	RazeVertex vtx[] =
	{
		{ x, y, color },
		{ x + w, y, color },

		{ x + w, y, color },
		{ x + w, y + h, color },

		{ x + w, y + h, color },
		{ x, y + h, color },

		{ x, y + h, color },
		{ x, y, color }
	};

	CheckPrimitiveBatch(D3DPT_LINELIST, 0);
	AddPrimitivesToBatch(vtx, sizeof(vtx));
}

void RazeRenderer::DrawTexturedRect(int32 x, int32 y, int32 w, int32 h, Color clr, IDirect3DTexture9* texture)
{
	DWORD color = D3DCOLOR_RGBA(clr.r, clr.g, clr.b, clr.a);

	// this fixes the one pixel offset, very shitty method, dunno what it causes tho so this has to do
	w += 1;
	h += 1;

	RazeVertex vtx[] =
	{
		{ x, y, color, 0.f, 0.f },
		{ x + w, y, color, 1.f, 0.f },
		{ x, y + h, color, 0.f, 1.f },

		{ x + w, y, color, 1.f, 0.f },
		{ x + w, y + h, color, 1.f, 1.f },
		{ x, y + h, color, 0.f, 1.f }
	};

	CheckPrimitiveBatch(D3DPT_TRIANGLELIST, texture);
	AddPrimitivesToBatch(vtx, sizeof(vtx));
}

void RazeRenderer::DrawTexturedGradientRect(int32 x, int32 y, int32 w, int32 h, Color startClr, Color endClr, IDirect3DTexture9* texture)
{
	DWORD color1 = D3DCOLOR_RGBA(startClr.r, startClr.g, startClr.b, startClr.a);
	DWORD color2 = D3DCOLOR_RGBA(endClr.r, endClr.g, endClr.b, endClr.a);

	// this fixes the one pixel offset, very shitty method, dunno what it causes tho so this has to do
	w += 1;
	h += 1;

	RazeVertex vtx[] =
	{
		{ x, y, color1, 0.f, 0.f },
		{ x + w, y, color1, 1.f, 0.f },
		{ x, y + h, color2, 0.f, 1.f },

		{ x + w, y, color1, 1.f, 0.f },
		{ x + w, y + h, color2, 1.f, 1.f },
		{ x, y + h, color2, 0.f, 1.f }
	};

	CheckPrimitiveBatch(D3DPT_TRIANGLELIST, texture);
	AddPrimitivesToBatch(vtx, sizeof(vtx));
}

uint32 RazeRenderer::AddFont(const wchar_t* fontName, uint32 height)
{
	ID3DXFont* font;
	D3DXCreateFontW(device, height, 0, 500, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, fontName, &font);

	return fonts.AddToTail(font);
}

RazeRect& RazeRenderer::FontTextSize(uint32 fontID, const wchar_t* str)
{
	RECT rct;
	fonts[fontID]->DrawTextW(0, str, -1, &rct, DT_CALCRECT, 0);

	RazeRect rect;
	rect.x = 0;
	rect.y = 0;
	rect.w = rct.right - rct.left;
	rect.h = rct.bottom - rct.top;

	return rect;
}

void RazeRenderer::DrawString(uint32 fontID, int32 x, int32 y, const wchar_t* str, bool outline, DWORD flags, Color clr)
{
	CheckSpriteBatch();

	ID3DXFont* font = fonts[fontID];
	if (!font)
	{
		return;
	}

	DWORD color = D3DCOLOR_RGBA(clr.r, clr.g, clr.b, clr.a);
	uint32 len = wcslen(str);
	RECT rect;

	if (outline)
	{
		rect.left = x - 1;
		rect.top = y;
		font->DrawTextW(curBatch->sprite, str, len, &rect, flags, D3DCOLOR_RGBA(0, 0, 0, 255));

		rect.left = x + 1;
		rect.top = y;
		font->DrawTextW(curBatch->sprite, str, len, &rect, flags, D3DCOLOR_RGBA(0, 0, 0, 255));

		rect.left = x;
		rect.top = y - 1;
		font->DrawTextW(curBatch->sprite, str, len, &rect, flags, D3DCOLOR_RGBA(0, 0, 0, 255));

		rect.left = x;
		rect.top = y + 1;
		font->DrawTextW(curBatch->sprite, str, len, &rect, flags, D3DCOLOR_RGBA(0, 0, 0, 255));
	}

	rect.left = x;
	rect.top = y;
	font->DrawTextW(curBatch->sprite, str, len, &rect, flags, color);
}