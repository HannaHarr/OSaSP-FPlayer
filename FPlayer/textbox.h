#pragma once

#include <Windows.h>
#include <string>

class TextBox
{
public:
	TextBox();
	~TextBox();
	TextBox(RECT rect);
	TextBox(RECT rect, LPCWSTR text);
	void SetText(std::wstring text);
	void SetRect(RECT rect);
	std::wstring getText();
	void SetRect(int left, int top, int right, int bottom);
	void ShowText(HDC hdc, int styles);
	void SetFontCharacteristics(int height, int width, int weight, int escapement);
private:
	std::wstring text;
	RECT rect;
	LOGFONT logFont;
	void InitFont();
	const int defaultFontHeight = 30;
	const int defaultFontWidth = 15;
	const int defaultFontWeight = 20;
	const int defaultFontEscapement = 5;
};

