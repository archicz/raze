#ifndef RAZE_GUI_H
#define RAZE_GUI_H
#pragma once

#include <strojar/vector.h>
#include <raze/renderer.h>

#define FONTSIZE_TAHOMANORMAL 12
#define FONTSIZE_TAHOMATITLE 16

#define RAZE_PADDING 8
#define RAZE_TEXTPADDING 4

enum ValueType
{
	VALUETYPE_DECIMAL,
	VALUETYPE_ABSOLUTE,
	VALUETYPE_ANGLE
};

class RazeGUIPanel
{
public:
	RazeRenderer* renderer;
	RazeRect rect;
	RazePoint posPre;

	RazeRect contents;
	uint32 contentsTopH;

	bool draw;
	bool focused;
	bool isControl;

	bool drawOverride;
	bool controlOverride;

	RazeGUIPanel* parent;
	StrojarVector<RazeGUIPanel*> children;
public:
	RazeGUIPanel();

	void SetParent(RazeGUIPanel* panel);
	void AddChild(RazeGUIPanel* panel);

	template<class T>
	inline T* CreateChildPanel()
	{
		T* panel = new T(renderer);
		AddChild(panel);

		return panel;
	}
public:
	void AddContent(int32 w, int32 h);
	void SetPadding(int32 left, int32 top, int32 right, int32 bottom);

	void SetSize(int32 w, int32 h);
	void SetPos(int32 x, int32 y);

	void SetContentPos(int32 x, int32 y);
	void SetContentSize(int32 w, int32 h);

	void SetOverrideDraw(bool ov);
	void SetOverrideControl(bool ov);

	void SetDraw(bool draw);
public:
	uint32 GetWidth()
	{
		return rect.w;
	}

	uint32 GetHeight()
	{
		return rect.h;
	}

	uint32 GetContentsWidth()
	{
		return contents.w;
	}

	uint32 GetContentsHeight()
	{
		return contents.h;
	}
public:
	virtual void UpdatePosition();
	virtual void Draw() = 0;
};


class RazeGUIControl : public RazeGUIPanel
{
public:
	bool enabled;
public:
	RazeGUIControl();

	void SetEnabled(bool enabled);
public:
	virtual bool MouseDown(int32 x, int32 y) = 0;
	virtual bool MouseUp(int32 x, int32 y) = 0;
	virtual void MouseMove(int32 x, int32 y) = 0;

	virtual bool KeyDown(int32 x, int32 y, WPARAM wParam, LPARAM lParam) = 0;
	virtual bool KeyUp(int32 x, int32 y, WPARAM wParam, LPARAM lParam) = 0;
};


class RazeGUIImage : public RazeGUIPanel
{
public:
	IDirect3DTexture9* texture;
	Color color1;
	Color color2;
	bool gradient;
public:
	RazeGUIImage(RazeRenderer* renderer);
public:
	void SetTexture(IDirect3DTexture9* texture);
	void SetColor(const Color& clr);
	void SetGradientColor(const Color& clr1, const Color& clr2);
public:
	virtual void Draw() override;
};


class RazeGUIButton;
typedef void(*RazeGUIButtonFn)(RazeGUIButton* btn);

class RazeGUIButton : public RazeGUIControl
{
public:
	bool click;
	bool hovered;
	RazeGUIButtonFn btnFn;
	
	wchar_t title[64];
	RazeRect titleBounds;
	bool drawTitle;
public:
	RazeGUIButton(RazeRenderer* renderer);

	void OnClick(RazeGUIButtonFn fn);
	void SetTitle(const wchar_t* title);
public:
	virtual void Draw() override;

	virtual bool MouseDown(int32 x, int32 y) override;
	virtual bool MouseUp(int32 x, int32 y) override;
	virtual void MouseMove(int32 x, int32 y) override;

	virtual bool KeyDown(int32 x, int32 y, WPARAM wParam, LPARAM lParam) override;
	virtual bool KeyUp(int32 x, int32 y, WPARAM wParam, LPARAM lParam) override;
};


class RazeGUICheckBox : public RazeGUIControl
{
public:
	bool click;
	bool hovered;
	bool* toggle;

	wchar_t title[64];
	RazeRect titleBounds;
	bool drawTitle;
	
	RazeRect checkBox;
public:
	RazeGUICheckBox(RazeRenderer* renderer);

	void SetToggle(bool* toggle);
	void SetTitle(const wchar_t* title);
public:
	virtual void UpdatePosition() override;
	virtual void Draw() override;

	virtual bool MouseDown(int32 x, int32 y) override;
	virtual bool MouseUp(int32 x, int32 y) override;
	virtual void MouseMove(int32 x, int32 y) override;

	virtual bool KeyDown(int32 x, int32 y, WPARAM wParam, LPARAM lParam) override;
	virtual bool KeyUp(int32 x, int32 y, WPARAM wParam, LPARAM lParam) override;
};


class RazeGUISlider : public RazeGUIControl
{
public:
	ValueType valueType;
	float* value;
	wchar_t valueText[64];
	RazeRect valueTextBounds;

	wchar_t title[64];

	float minValue;
	float maxValue;

	RazeRect sliderRect;
	int32 dragOffsetX;
	bool dragging;
	bool hovered;
public:
	RazeGUISlider(RazeRenderer* renderer);

	void SetValue(float* value);
	void SetValueType(ValueType type);
	void SetTitle(const wchar_t* title);

	void SetMinMax(float min, float max);
	void SetDefaultValue(float val);

	int32 ValueToPixels();
	float PixelsToValue();

	void UpdateValueTitle();
	void UpdateValue();

	void SubaddValue(float change);
public:
	virtual void UpdatePosition() override;
	virtual void Draw() override;

	virtual bool MouseDown(int32 x, int32 y) override;
	virtual bool MouseUp(int32 x, int32 y) override;
	virtual void MouseMove(int32 x, int32 y) override;

	virtual bool KeyDown(int32 x, int32 y, WPARAM wParam, LPARAM lParam) override;
	virtual bool KeyUp(int32 x, int32 y, WPARAM wParam, LPARAM lParam) override;
};


class RazeGUIComboBox : public RazeGUIControl
{
public:
	struct ComboItem
	{
		RazePoint posPre;
		RazeRect rect;

		RazeRect nameBounds;
		wchar_t name[64];
		int32 value;
		bool hovered;
		bool click;
	};
public:
	int32* value;
	StrojarVector<ComboItem*> items;
	ComboItem* selectedItem;

	wchar_t title[64];

	int32 comboboxH;
	int32 itemY;

	bool click;
	bool hovered;
public:
	RazeGUIComboBox(RazeRenderer* renderer);
	
	void SetTitle(const wchar_t* title);
	void SetValue(int32* value);
	void AddItem(const wchar_t* name, int32 value);
	void UpdateItemPositions();
	void SelectItem(ComboItem* item);
public:
	virtual void UpdatePosition() override;
	virtual void Draw() override;

	virtual bool MouseDown(int32 x, int32 y) override;
	virtual bool MouseUp(int32 x, int32 y) override;
	virtual void MouseMove(int32 x, int32 y) override;

	virtual bool KeyDown(int32 x, int32 y, WPARAM wParam, LPARAM lParam) override;
	virtual bool KeyUp(int32 x, int32 y, WPARAM wParam, LPARAM lParam) override;
};


class RazeGUISection : public RazeGUIPanel
{
public:
	wchar_t title[64];
	
	RazeRect titleBounds;
	bool drawTitle;
	bool titleCenter;
	Color titleColor;
public:
	RazeGUISection(RazeRenderer* renderer);
public:
	void SetTitle(const wchar_t* title);
	void SetCenterTitle(bool center);
	void SetTextColor(const Color& clr);
public:
	virtual void Draw() override;
};


class RazeGUIManager;

class RazeGUIFrame : public RazeGUIControl
{
public:
	bool click;
	bool dragging;

	RazeGUIManager* mgr;
	char name[32];

	RazePoint dragOffset;

	RazePoint hitTestPre;
	RazeRect hitTest;
public:
	RazeGUIFrame(RazeRenderer* renderer, RazeGUIManager* mgr, const char* name);
public:
	void ScreenCenter();
	void SetHitTest(int32 x, int32 y, int32 w, int32 h);
public:
	virtual void UpdatePosition() override;
	virtual void Draw() override;

	virtual bool MouseDown(int32 x, int32 y) override;
	virtual bool MouseUp(int32 x, int32 y) override;
	virtual void MouseMove(int32 x, int32 y) override;

	virtual bool KeyDown(int32 x, int32 y, WPARAM wParam, LPARAM lParam) override;
	virtual bool KeyUp(int32 x, int32 y, WPARAM wParam, LPARAM lParam) override;
};


class RazeGUIInput
{
public:
	RazePoint pos;
	RazePoint posOld;

	bool leftDown;
	bool leftUp;
	bool leftMove;

	bool keyDown;
	bool keyUp;

	WPARAM wKey;
	LPARAM lKey;
public:
	RazeGUIInput();
public:
	void WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	void ResetStates();
};


class RazeGUIManager
{
public:
	RazeRenderer* renderer;
	RazeGUIInput* input;
	StrojarVector<RazeGUIFrame*> frames;
	RazeGUIFrame* focusFrame;
	RazeGUIPanel* focusControl;
public:
	RazeGUIManager(RazeRenderer* renderer);

	RazeGUIFrame* CreateFrame(const char* name);
	void SetFocusFrame(RazeGUIFrame* frame);
public:
	void DrawSingle(RazeGUIPanel* panel);
	void DrawRecursive(RazeGUIPanel* panel);

	bool MouseEventSingle(RazeGUIPanel* panel);
	bool MouseEventRecursive(RazeGUIPanel* panel);
public:
	void Render();
	void WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

#endif