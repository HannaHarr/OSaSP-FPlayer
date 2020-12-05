#include "TextBox.h"
#include <Windows.h>

TextBox::TextBox()
{
	this->rect.left = 0;
	this->rect.top = 0;
	this->rect.right = 0;
	this->rect.bottom = 0;
	this->text = L"";
	InitFont();
}

TextBox::TextBox(RECT rect, LPCWSTR text)
{
	this->rect.left = rect.left;
	this->rect.top = rect.top;
	this->rect.right = rect.right;
	this->rect.bottom = rect.bottom;
	this->text = text;
	InitFont();
}

TextBox::TextBox(RECT rect)
{
	this->rect.left = rect.left;
	this->rect.top = rect.top;
	this->rect.right = rect.right;
	this->rect.bottom = rect.bottom;
	this->text = L"";
	InitFont();
}

TextBox::~TextBox()
{

}

void TextBox::SetText(std::wstring text)
{
	this->text = text;
};

std::wstring TextBox::getText()
{
	return this->text;
}

void TextBox::SetRect(RECT rect)
{
	this->rect.left = rect.left;
	this->rect.top = rect.top;
	this->rect.right = rect.right;
	this->rect.bottom = rect.bottom;
}

void TextBox::SetRect(int left, int top, int right, int bottom)
{
	this->rect.left = left;
	this->rect.top = top;
	this->rect.right = right;
	this->rect.bottom = bottom;
}

void TextBox::ShowText(HDC hdc, int styles)
{
	HFONT hSongFont = CreateFontIndirect(&logFont);
	SelectObject(hdc, hSongFont);
	DrawText(hdc, this->text.c_str(), -1, &this->rect, styles);
	DeleteObject(hSongFont);
}

void TextBox::InitFont()
{
	this->logFont.lfCharSet = DEFAULT_CHARSET;
	this->logFont.lfPitchAndFamily = DEFAULT_PITCH;
	strcpy_s((char*)this->logFont.lfFaceName, _countof(this->logFont.lfFaceName), "Arial");
	this->logFont.lfHeight = defaultFontHeight;
	this->logFont.lfWidth = defaultFontWidth;
	this->logFont.lfWeight = defaultFontWeight;
	this->logFont.lfEscapement = defaultFontEscapement;
}

void TextBox::SetFontCharacteristics(int height, int width, int weight, int escapement)
{
	this->logFont.lfHeight = height;
	this->logFont.lfWidth = width;
	this->logFont.lfWeight = weight;
	this->logFont.lfEscapement = escapement;
}