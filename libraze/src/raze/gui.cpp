#include <windowsx.h>

#include "gui.h"
#include "math.h"

static uint32 fontTahomaNormal = 0;
static uint32 fontTahomaTitle = 0;

// GUI PANEL
RazeGUIPanel::RazeGUIPanel() :
	renderer(0), rect(0, 0, 0, 0), posPre(0, 0), draw(true), focused(false), isControl(false), drawOverride(false), controlOverride(false), parent(0), children(0, 0), contents(0, 0, 0, 0), contentsTopH(0)
{
}

void RazeGUIPanel::SetParent(RazeGUIPanel* panel)
{
	parent = panel;
}

void RazeGUIPanel::AddChild(RazeGUIPanel* panel)
{
	children.AddToTail(panel);
	panel->SetParent(this);
}

void RazeGUIPanel::AddContent(int32 w, int32 h)
{
	if (contents.w == w)
	{
		if (contents.h == h)
		{
			contents.x = contents.w;
			contents.y = contents.h;
		}
		else
		{
			contents.y += h;
			contents.h -= h;

			contentsTopH += h;
		}
	}
	else
	{
		if (contents.h == h)
		{
			contents.x += w;
			contents.y = contentsTopH;

			contents.w -= w;
			contents.h = rect.h - contentsTopH;
		}
		else
		{
			contents.y += h;
			contents.h -= h;
		}
	}
}

void RazeGUIPanel::SetPadding(int32 left, int32 top, int32 right, int32 bottom)
{
	if (left == -1)
	{
		left = RAZE_PADDING;
	}

	if (top == -1)
	{
		top = RAZE_PADDING;
	}

	if (right == -1)
	{
		right = RAZE_PADDING;
	}

	if (bottom == -1)
	{
		bottom = RAZE_PADDING;
	}

	if (!parent)
	{
		return;
	}

	uint32 sw = rect.w;
	uint32 sh = rect.h;

	uint32 pcx = parent->contents.x;
	uint32 pcy = parent->contents.y;

	uint32 pcw = parent->contents.w;
	uint32 pch = parent->contents.h;
	
	if (sw == 0 || sw > pcw)
	{
		sw = pcw;
	}

	if (sh == 0 || sh > pch)
	{
		sh = pch;
	}
	
	SetPos(pcx + left, pcy + top);

	parent->AddContent(sw, sh);

	if (right)
	{
		sw -= right + left;
	}

	if (bottom)
	{
		sh -= bottom + top;
	}

	SetSize(sw, sh);
}

void RazeGUIPanel::SetSize(int32 w, int32 h)
{
	rect.w = w;
	rect.h = h;
	
	SetContentSize(w, h);
	UpdatePosition();
}

void RazeGUIPanel::SetPos(int32 x, int32 y)
{
	posPre.x = x;
	posPre.y = y;

	UpdatePosition();
}

void RazeGUIPanel::SetContentPos(int32 x, int32 y)
{
	contents.x = x;
	contents.y = y;
}

void RazeGUIPanel::SetContentSize(int32 w, int32 h)
{
	contents.w = w;
	contents.h = h;
}

void RazeGUIPanel::SetOverrideDraw(bool ov)
{
	drawOverride = ov;
}

void RazeGUIPanel::SetOverrideControl(bool ov)
{
	controlOverride = ov;
}

void RazeGUIPanel::SetDraw(bool _draw)
{
	draw = _draw;
}

void RazeGUIPanel::UpdatePosition()
{
	if (parent)
	{
		rect.x = parent->rect.x + posPre.x;
		rect.y = parent->rect.y + posPre.y;
	}
	else
	{
		rect.x = posPre.x;
		rect.y = posPre.y;
	}

	for (RazeGUIPanel* childPanel : children)
	{
		childPanel->UpdatePosition();
	}
}


// GUI CONTROL
RazeGUIControl::RazeGUIControl() :
	enabled(true)
{
}

void RazeGUIControl::SetEnabled(bool _enabled)
{
	enabled = _enabled;
}


// GUI IMAGE
RazeGUIImage::RazeGUIImage(RazeRenderer* _renderer) :
	color1(255, 255), color2(255, 255), gradient(false)
{
	renderer = _renderer;
}

void RazeGUIImage::SetTexture(IDirect3DTexture9* _texture)
{
	texture = _texture;
}

void RazeGUIImage::SetColor(const Color& clr)
{
	color1 = clr;

	gradient = false;
}

void RazeGUIImage::SetGradientColor(const Color& clr1, const Color& clr2)
{
	color1 = clr1;
	color2 = clr2;

	gradient = true;
}

void RazeGUIImage::Draw()
{
	if (gradient)
	{
		renderer->DrawTexturedGradientRect(rect.x, rect.y, rect.w, rect.h, color1, color2, texture);
	}
	else
	{
		renderer->DrawTexturedRect(rect.x, rect.y, rect.w, rect.h, color1, texture);
	}
}


// GUI BUTTON
RazeGUIButton::RazeGUIButton(RazeRenderer* _renderer) :
	click(false), hovered(false), btnFn(0), titleBounds(0, 0, 0, 0), drawTitle(false)
{
	isControl = true; // it's a button
	renderer = _renderer;
}

void RazeGUIButton::OnClick(RazeGUIButtonFn fn)
{
	btnFn = fn;
}

void RazeGUIButton::SetTitle(const wchar_t* _title)
{
	drawTitle = true;
	wcscpy(title, _title);
	titleBounds = renderer->FontTextSize(fontTahomaNormal, title);
}

void RazeGUIButton::Draw()
{
	renderer->DrawOutlinedRect(rect.x, rect.y, rect.w, rect.h, hovered ? Color(0, 200, 200) : Color(0, 125, 125));

	if (drawTitle)
	{
		renderer->DrawString(fontTahomaNormal, rect.x + rect.w / 2 - titleBounds.w / 2, rect.y + rect.h / 2 - titleBounds.h / 2, title, false, DT_NOCLIP, hovered ? Color(255, 255) : Color(100, 255));
	}
}

bool RazeGUIButton::MouseDown(int32 x, int32 y)
{
	if (btnFn)
	{
		if (rect.PointInside(x, y))
		{
			click = true;
			return true;
		}
	}

	return false;
}

bool RazeGUIButton::MouseUp(int32 x, int32 y)
{
	if (btnFn)
	{
		if (click)
		{
			if (rect.PointInside(x, y))
			{
				click = false;
				btnFn(this);

				return true;
			}
		}
	}

	return false;
}

void RazeGUIButton::MouseMove(int32 x, int32 y)
{
	if (rect.PointInside(x, y))
	{
		hovered = true;
	}
	else
	{
		hovered = false;
	}
}

bool RazeGUIButton::KeyDown(int32 x, int32 y, WPARAM wParam, LPARAM lParam)
{
	return false;
}

bool RazeGUIButton::KeyUp(int32 x, int32 y, WPARAM wParam, LPARAM lParam)
{
	return false;
}


// GUI CHECKBOX
RazeGUICheckBox::RazeGUICheckBox(RazeRenderer* _renderer) :
	click(false), toggle(0), hovered(false), titleBounds(0, 0, 0, 0), drawTitle(false), checkBox(0, 0, 0, 0)
{
	isControl = true; // it's a checkbox
	renderer = _renderer;
}

void RazeGUICheckBox::SetToggle(bool* _toggle)
{
	toggle = _toggle;
}

void RazeGUICheckBox::SetTitle(const wchar_t* _title)
{
	drawTitle = true;
	wcscpy(title, _title);
	titleBounds = renderer->FontTextSize(fontTahomaNormal, title);
}

void RazeGUICheckBox::UpdatePosition()
{
	// run the original
	RazeGUIPanel::UpdatePosition();

	// checkbox updatez
	checkBox.x = rect.x;
	checkBox.y = rect.y;
	checkBox.w = rect.h;
	checkBox.h = rect.h;
}

void RazeGUICheckBox::Draw()
{
	renderer->DrawOutlinedRect(checkBox.x, checkBox.y, checkBox.w, checkBox.h, Color(0, 200, 200));

	if (toggle)
	{
		if (*toggle)
		{
			renderer->DrawRect(checkBox.x + 4, checkBox.y + 4, checkBox.w - 8, checkBox.h - 8, Color(0, 200, 200));
			renderer->DrawOutlinedRect(checkBox.x + 4, checkBox.y + 4, checkBox.w - 8, checkBox.h - 8, Color(0, 160, 160));
		}
	}

	if (drawTitle)
	{
		renderer->DrawString(fontTahomaNormal, checkBox.x + checkBox.w + RAZE_TEXTPADDING + 1, rect.y + rect.h / 2 - titleBounds.h / 2, title, false, DT_NOCLIP, hovered ? Color(255, 255) : Color(100, 255));
	}
}

bool RazeGUICheckBox::MouseDown(int32 x, int32 y)
{
	if (rect.PointInside(x, y))
	{
		click = true;
		return true;
	}

	return false;
}

bool RazeGUICheckBox::MouseUp(int32 x, int32 y)
{
	if (toggle)
	{
		if (click)
		{
			if (rect.PointInside(x, y))
			{
				click = false;
				*toggle = !*toggle;

				return true;
			}
		}
	}

	return false;
}

void RazeGUICheckBox::MouseMove(int32 x, int32 y)
{
	if (rect.PointInside(x, y))
	{
		hovered = true;
	}
	else
	{
		hovered = false;
	}
}

bool RazeGUICheckBox::KeyDown(int32 x, int32 y, WPARAM wParam, LPARAM lParam)
{
	return false;
}

bool RazeGUICheckBox::KeyUp(int32 x, int32 y, WPARAM wParam, LPARAM lParam)
{
	return false;
}


// GUI SLIDER
RazeGUISlider::RazeGUISlider(RazeRenderer* _renderer) :
	valueType(VALUETYPE_DECIMAL), value(0), valueTextBounds(0, 0, 0, 0), minValue(0.f), maxValue(0.f), sliderRect(0, 0, 0, 0), dragOffsetX(0), dragging(false), hovered(false)
{
	isControl = true; // it's a slider
	renderer = _renderer;
}

void RazeGUISlider::SetValue(float* _value)
{
	value = _value;
}

void RazeGUISlider::SetValueType(ValueType type)
{
	valueType = type;
}

void RazeGUISlider::SetTitle(const wchar_t* _title)
{
	wcscpy(title, _title);
}

void RazeGUISlider::SetMinMax(float min, float max)
{
	minValue = min;
	maxValue = max;
}

void RazeGUISlider::SetDefaultValue(float val)
{
	*value = val;
	dragOffsetX = ValueToPixels();
}

int32 RazeGUISlider::ValueToPixels()
{
	return (int32)remap(*value, minValue, maxValue, 0.f, (float)sliderRect.w);
}

float RazeGUISlider::PixelsToValue()
{
	return remap((float)dragOffsetX, 0.f, (float)sliderRect.w, minValue, maxValue);
}

void RazeGUISlider::UpdateValueTitle()
{
	if (!value)
	{
		return;
	}

	switch (valueType)
	{
		case VALUETYPE_DECIMAL:
			swprintf(valueText, sizeof(valueText) / sizeof(wchar_t), L"%.2f", *value);
		break;
		case VALUETYPE_ABSOLUTE:
			swprintf(valueText, sizeof(valueText) / sizeof(wchar_t), L"%.0f", *value);
		break;
		case VALUETYPE_ANGLE:
			swprintf(valueText, sizeof(valueText) / sizeof(wchar_t), L"%.0f°", *value);
		break;
	}

	valueTextBounds = renderer->FontTextSize(fontTahomaNormal, valueText);
}

void RazeGUISlider::UpdateValue()
{
	*value = PixelsToValue();
}

void RazeGUISlider::SubaddValue(float change)
{
	float newValue = clamp(*value + change, minValue, maxValue);
	*value = newValue;

	dragOffsetX = ValueToPixels();
}

void RazeGUISlider::UpdatePosition()
{
	// run the original
	RazeGUIPanel::UpdatePosition();

	// maek the slider rect
	sliderRect.x = rect.x + 1;
	sliderRect.y = rect.y + FONTSIZE_TAHOMANORMAL;
	sliderRect.w = rect.w - 1;
	sliderRect.h = RAZE_PADDING / 2;
}

void RazeGUISlider::Draw()
{
	//renderer->DrawOutlinedRect(rect.x, rect.y, rect.w, rect.h, Color(0, 50, 50));

	// updatenz
	UpdateValueTitle();

	// title
	renderer->DrawString(fontTahomaNormal, rect.x + 1, rect.y - RAZE_TEXTPADDING / 2, title, false, DT_NOCLIP, hovered ? Color(255, 255) : Color(100, 255));

	// value text
	renderer->DrawString(fontTahomaNormal, rect.x + rect.w - valueTextBounds.w + 1, rect.y - RAZE_TEXTPADDING / 2, valueText, false, DT_NOCLIP, hovered ? Color(255, 255) : Color(100, 255));

	// draw the slider yea
	renderer->DrawRect(sliderRect.x, sliderRect.y, sliderRect.w, sliderRect.h, Color(0, 80, 80));
	renderer->DrawRect(sliderRect.x, sliderRect.y, dragOffsetX, sliderRect.h, Color(0, 200, 200));
}

bool RazeGUISlider::MouseDown(int32 x, int32 y)
{
	if (rect.PointInside(x, y))
	{
		dragging = true;
		return true;
	}

	return false;
}

bool RazeGUISlider::MouseUp(int32 x, int32 y)
{
	if (dragging)
	{
		dragOffsetX = clamp(x - sliderRect.x, 0, sliderRect.w);
		UpdateValue();

		dragging = false;
		return true;
	}

	return false;
}

void RazeGUISlider::MouseMove(int32 x, int32 y)
{
	if (rect.PointInside(x, y))
	{
		hovered = true;
	}
	else
	{
		hovered = false;
	}

	if (dragging)
	{
		dragOffsetX = clamp(x - sliderRect.x, 0, sliderRect.w);
		UpdateValue();
	}
}

bool RazeGUISlider::KeyDown(int32 x, int32 y, WPARAM wParam, LPARAM lParam)
{
	if (hovered)
	{
		if (wParam == VK_LEFT)
		{
			switch (valueType)
			{
				case VALUETYPE_ABSOLUTE:
				case VALUETYPE_ANGLE:
					SubaddValue(-1.f);
				break;
				case VALUETYPE_DECIMAL:
					SubaddValue(-0.01f);
				break;
			}

			return true;
		}
		else if (wParam == VK_RIGHT)
		{
			switch (valueType)
			{
				case VALUETYPE_ABSOLUTE:
				case VALUETYPE_ANGLE:
					SubaddValue(1.f);
				break;
				case VALUETYPE_DECIMAL:
					SubaddValue(0.01f);
				break;
			}

			return true;
		}
	}

	return false;
}

bool RazeGUISlider::KeyUp(int32 x, int32 y, WPARAM wParam, LPARAM lParam)
{
	return false;
}


// GUI COMBOBOX
RazeGUIComboBox::RazeGUIComboBox(RazeRenderer* _renderer) :
	items(0, 0), click(false), hovered(false)
{
	isControl = true; // it's a combo box
	renderer = _renderer;
}

void RazeGUIComboBox::SetTitle(const wchar_t* _title)
{
	wcscpy(title, _title);
}

void RazeGUIComboBox::SetValue(int32* _value)
{
	value = _value;
}

void RazeGUIComboBox::AddItem(const wchar_t* name, int32 value)
{
	ComboItem* item = new ComboItem;
	item->posPre = RazePoint(0, 0);
	item->rect = RazeRect(0, 0, 0, 0);
	item->nameBounds = renderer->FontTextSize(fontTahomaNormal, name);
	wcscpy(item->name, name);
	item->value = value;
	item->hovered = false;
	item->click = false;

	itemY += comboboxH;
	item->posPre.x = 0;
	item->posPre.y = itemY;

	items.AddToTail(item);

	UpdateItemPositions();
}

void RazeGUIComboBox::UpdateItemPositions()
{
	for (ComboItem* item : items)
	{
		item->rect.x = rect.x + item->posPre.x;
		item->rect.y = rect.y + item->posPre.y;
		item->rect.w = rect.w;
		item->rect.h = comboboxH;
	}
}

void RazeGUIComboBox::SelectItem(ComboItem* item)
{
	selectedItem = item;
	*value = selectedItem->value;
}

void RazeGUIComboBox::UpdatePosition()
{
	// run the original
	RazeGUIPanel::UpdatePosition();

	// adjust the combobox height
	comboboxH = rect.h - FONTSIZE_TAHOMANORMAL;

	// set item Y
	itemY = FONTSIZE_TAHOMANORMAL;

	// translate all item positions
	UpdateItemPositions();
}

void RazeGUIComboBox::Draw()
{
	// title
	renderer->DrawString(fontTahomaNormal, rect.x + 1, rect.y - RAZE_TEXTPADDING / 2, title, false, DT_NOCLIP, hovered ? Color(255, 255) : Color(100, 255));
	
	// combobox rect
	renderer->DrawOutlinedRect(rect.x, rect.y + FONTSIZE_TAHOMANORMAL, rect.w, comboboxH, focused || hovered ? Color(0, 200, 200) : Color(0, 125, 125));

	// selected item title
	if (selectedItem)
	{
		renderer->DrawString(fontTahomaNormal, rect.x + rect.w / 2 - selectedItem->nameBounds.w / 2, rect.y + rect.h / 2 + FONTSIZE_TAHOMANORMAL / 2 - selectedItem->nameBounds.h / 2, selectedItem->name, false, DT_NOCLIP, focused || hovered ? Color(255, 255) : Color(100, 255));
	}

	// if the control is focused, the user clicked on the combobox
	// show all the items under this
	if (focused)
	{
		for (ComboItem* item : items)
		{
			renderer->DrawRect(item->rect.x, item->rect.y, item->rect.w, item->rect.h, Color(16, 255));
			renderer->DrawOutlinedRect(item->rect.x, item->rect.y, item->rect.w, item->rect.h, Color(0, 200, 200));

			renderer->DrawString(fontTahomaNormal, item->rect.x + item->rect.w / 2 - item->nameBounds.w / 2, item->rect.y + item->rect.h / 2 - item->nameBounds.h / 2, item->name, false, DT_NOCLIP, item->hovered ? Color(255, 255) : Color(100, 255));
		}
	}
}

bool RazeGUIComboBox::MouseDown(int32 x, int32 y)
{
	if (rect.PointInside(x, y))
	{
		click = true;
		return true;
	}
	else if (focused)
	{
		for (ComboItem* item : items)
		{
			if (item->rect.PointInside(x, y))
			{
				item->click = true;
			}
		}
	}

	return false;
}

bool RazeGUIComboBox::MouseUp(int32 x, int32 y)
{
	if (rect.PointInside(x, y))
	{
		if (click)
		{
			click = false;
			
			if (focused)
			{
				focused = false;
			}
			else
			{
				focused = true;
			}
		}
		
		return true;
	}
	else if (focused)
	{
		for (ComboItem* item : items)
		{
			if (item->click)
			{
				item->click = false;

				hovered = false;
				focused = false;
				click = false;

				SelectItem(item);

				return true;
			}
		}
	}

	return false;
}

void RazeGUIComboBox::MouseMove(int32 x, int32 y)
{
	if (rect.PointInside(x, y))
	{
		hovered = true;
	}
	else
	{
		hovered = false;

		if (focused)
		{
			for (ComboItem* item : items)
			{
				if (item->rect.PointInside(x, y))
				{
					item->hovered = true;
					hovered = true;
				}
				else
				{
					item->hovered = false;
				}
			}
		}
	}
}

bool RazeGUIComboBox::KeyDown(int32 x, int32 y, WPARAM wParam, LPARAM lParam)
{
	return false;
}

bool RazeGUIComboBox::KeyUp(int32 x, int32 y, WPARAM wParam, LPARAM lParam)
{
	return false;
}


// GUI SECTION
RazeGUISection::RazeGUISection(RazeRenderer* _renderer) :
	drawTitle(false), titleCenter(false), titleColor(255, 255)
{
	renderer = _renderer;
}

void RazeGUISection::SetTitle(const wchar_t* _title)
{
	drawTitle = true;
	wcscpy(title, _title);
	titleBounds = renderer->FontTextSize(fontTahomaNormal, title);
}

void RazeGUISection::SetCenterTitle(bool center)
{
	titleCenter = center;
}

void RazeGUISection::SetTextColor(const Color& clr)
{
	titleColor = clr;
}

void RazeGUISection::Draw()
{
	if (drawTitle)
	{
		// left top - left bottom
		renderer->DrawLine(rect.x, rect.y, rect.x, rect.y + rect.h, Color(0, 200, 200));

		// left bottom - right bottom
		renderer->DrawLine(rect.x, rect.y + rect.h, rect.x + rect.w, rect.y + rect.h, Color(0, 200, 200));

		// right bottom - right top
		renderer->DrawLine(rect.x + rect.w, rect.y + rect.h, rect.x + rect.w, rect.y, Color(0, 200, 200));

		if (titleCenter)
		{
			// left top - text start
			renderer->DrawLine(rect.x, rect.y, rect.x + rect.w / 2 - titleBounds.w / 2 - 2 - 1, rect.y, Color(0, 200, 200));

			// text end - right top
			renderer->DrawLine(rect.x + rect.w / 2 + titleBounds.w / 2 + 2, rect.y, rect.x + rect.w, rect.y, Color(0, 200, 200));

			// the title in center
			renderer->DrawString(fontTahomaNormal, rect.x + rect.w / 2 - titleBounds.w / 2, rect.y - titleBounds.h / 2, title, false, DT_NOCLIP, titleColor);
		}
		else
		{
			// left top - 4 + 1(font bug) px right
			renderer->DrawLine(rect.x, rect.y, rect.x + 4, rect.y, Color(0, 200, 200));

			// the title
			renderer->DrawString(fontTahomaNormal, rect.x + 4 + 2 + 1, rect.y - titleBounds.h / 2, title, false, DT_NOCLIP, titleColor);

			// remaining - right top
			renderer->DrawLine(rect.x + 4 + 2 + titleBounds.w + 2, rect.y, rect.x + rect.w, rect.y, Color(0, 200, 200));
		}
	}
	else
	{
		renderer->DrawOutlinedRect(rect.x, rect.y, rect.w, rect.h, Color(0, 200, 200));
	}
}


// GUI FRAME
RazeGUIFrame::RazeGUIFrame(RazeRenderer* _renderer, RazeGUIManager* _mgr, const char* _name) :
	mgr(_mgr), dragOffset(0, 0), hitTest(0, 0, 0, 0), hitTestPre(0, 0), dragging(false), click(false)
{
	strcpy(name, _name);
	renderer = _renderer;
}

void RazeGUIFrame::ScreenCenter()
{
	SetPos(renderer->GetScreenWidth() / 2 - rect.w / 2, renderer->GetScreenHeight() / 2 - rect.h / 2);
}

void RazeGUIFrame::SetHitTest(int32 x, int32 y, int32 w, int32 h)
{
	hitTestPre.x = x;
	hitTestPre.y = x;
	hitTest.w = w == 0 ? rect.w : w;
	hitTest.h = h == 0 ? rect.h : h;

	// accept mouse events
	isControl = true;

	// bruh
	UpdatePosition();
}

void RazeGUIFrame::UpdatePosition()
{
	// run the original
	RazeGUIPanel::UpdatePosition();

	// update hitTest rect
	hitTest.x = rect.x + hitTestPre.x;
	hitTest.y = rect.y + hitTestPre.y;
}

void RazeGUIFrame::Draw()
{
	renderer->DrawRect(rect.x, rect.y, rect.w, rect.h, Color(16, 255));
	renderer->DrawOutlinedRect(rect.x, rect.y, rect.w, rect.h, Color(0, 200, 200));
}

bool RazeGUIFrame::MouseDown(int32 x, int32 y)
{
	if (hitTest.PointInside(x, y))
	{
		if (focused)
		{
			dragOffset.x = x - posPre.x;
			dragOffset.y = y - posPre.y;
			dragging = true;
		}
		else
		{
			click = true;
		}

		return true;
	}

	return false;
}

bool RazeGUIFrame::MouseUp(int32 x, int32 y)
{
	if (dragging)
	{
		dragOffset.x = 0;
		dragOffset.y = 0;
		dragging = false;

		return true;
	}

	if (!focused)
	{
		if (click)
		{
			if (rect.PointInside(x, y))
			{
				click = false;
				mgr->SetFocusFrame(this);

				return true;
			}
		}
	}

	return false;
}

void RazeGUIFrame::MouseMove(int32 x, int32 y)
{
	if (dragging)
	{
		SetPos(x - dragOffset.x, y - dragOffset.y);
	}
}

bool RazeGUIFrame::KeyDown(int32 x, int32 y, WPARAM wParam, LPARAM lParam)
{
	return false;
}

bool RazeGUIFrame::KeyUp(int32 x, int32 y, WPARAM wParam, LPARAM lParam)
{
	return false;
}


// GUI INPUT
RazeGUIInput::RazeGUIInput() :
	pos(0, 0), leftDown(false), leftUp(false), leftMove(false), keyDown(false), keyUp(false), wKey(0), lKey(0)
{
	ResetStates();
}

void RazeGUIInput::WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int32 x = GET_X_LPARAM(lParam);
	int32 y = GET_Y_LPARAM(lParam);

	ResetStates();
	
	switch (message)
	{
		case WM_MOUSEMOVE:
			pos.x = x;
			pos.y = y;

			leftMove = true;
		break;
		case WM_LBUTTONDOWN:
			leftDown = true;
		break;
		case WM_LBUTTONUP:
			leftUp = true;
		break;
		case WM_KEYDOWN:
			keyDown = true;

			wKey = wParam;
			lKey = lParam;
		break;
		case WM_KEYUP:
			keyUp = true;

			wKey = wParam;
			lKey = lParam;
		break;
	}
}

void RazeGUIInput::ResetStates()
{
	leftDown = false;
	leftUp = false;
	leftMove = false;

	keyDown = false;
	keyUp = false;
	wKey = 0;
	lKey = 0;
}


// GUI MANAGER CLASS
RazeGUIManager::RazeGUIManager(RazeRenderer* _renderer) :
	renderer(_renderer), frames(0, 0), focusFrame(0)
{
	input = new RazeGUIInput;

	fontTahomaNormal = renderer->AddFont(L"Tahoma", FONTSIZE_TAHOMANORMAL);
	fontTahomaTitle = renderer->AddFont(L"Tahoma", FONTSIZE_TAHOMATITLE);
}

RazeGUIFrame* RazeGUIManager::CreateFrame(const char* name)
{
	RazeGUIFrame* frame = new RazeGUIFrame(renderer, this, name);
	frames.AddToTail(frame);

	SetFocusFrame(frame);
	return frame;
}

void RazeGUIManager::SetFocusFrame(RazeGUIFrame* frame)
{
	if (focusFrame)
	{
		focusFrame->focused = false;
	}

	frame->focused = true;
	focusFrame = frame;
}


void RazeGUIManager::DrawSingle(RazeGUIPanel* panel)
{
	if (panel->draw)
	{
		if (panel->focused && panel != focusFrame)
		{
			focusControl = panel;
		}
		else
		{
			if (!panel->drawOverride)
			{
				panel->Draw();
			}
		}
	}
}

void RazeGUIManager::DrawRecursive(RazeGUIPanel* panel)
{
	DrawSingle(panel);

	for (RazeGUIPanel* childPanel : panel->children)
	{
		DrawRecursive(childPanel);
	}
}

bool RazeGUIManager::MouseEventSingle(RazeGUIPanel* panel)
{
	if (panel->isControl)
	{
		RazeGUIControl* panelControl = (RazeGUIControl*)panel;
		if (panelControl->rect.IsZeroed())
		{
			return false;
		}

		// control is disabled
		if (!panelControl->enabled)
		{
			return false;
		}

		// control uses it's parents input calls or something like that, aka the input is overriden
		if (panelControl->controlOverride)
		{
			return false;
		}
		
		if (input->leftMove)
		{
			panelControl->MouseMove(input->pos.x, input->pos.y);
		}

		if (input->leftDown)
		{
			if (panelControl->MouseDown(input->pos.x, input->pos.y))
			{
				return true;
			}
		}
		else if (input->leftUp)
		{
			if (panelControl->MouseUp(input->pos.x, input->pos.y))
			{
				return true;
			}
		}

		if (input->keyDown)
		{
			if (panelControl->KeyDown(input->pos.x, input->pos.y, input->wKey, input->lKey))
			{
				return true;
			}
		}
		else if (input->keyUp)
		{
			if (panelControl->KeyUp(input->pos.x, input->pos.y, input->wKey, input->lKey))
			{
				return true;
			}
		}
	}

	return false;
}

bool RazeGUIManager::MouseEventRecursive(RazeGUIPanel* panel)
{
	if (MouseEventSingle(panel))
	{
		return true;
	}

	for (RazeGUIPanel* childPanel : panel->children)
	{
		if (MouseEventRecursive(childPanel))
		{
			return true;
		}
	}

	return false;
}

void RazeGUIManager::Render()
{
	// first draw all the frames
	for (RazeGUIFrame* frame : frames)
	{
		if (frame != focusFrame)
		{
			DrawRecursive(frame);
		}
	}

	// draw the focus frame as the last to draw over the others non-focused ones
	DrawRecursive(focusFrame);

	// if we got a focused control which wants to draw over the whole menu, draw it now
	if (focusControl)
	{
		focusControl->Draw();
	}
}

void RazeGUIManager::WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	input->WinProc(hWnd, message, wParam, lParam);

	// if the focused frame has a focused control, run input on that panel and skip everything else
	if (focusControl && focusControl->focused)
	{
		MouseEventSingle(focusControl);
		return;
	}

	// run input on the focused frame
	if (MouseEventRecursive(focusFrame))
	{
		return;
	}

	// finally run input on the rest
	for (RazeGUIFrame* frame : frames)
	{
		if (frame != focusFrame)
		{
			if (MouseEventSingle(frame))
			{
				break;
			}
		}
	}
}