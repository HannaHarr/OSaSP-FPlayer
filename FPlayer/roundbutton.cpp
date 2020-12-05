#include "RoundButton.h"
#include <windows.h>

RoundButton::RoundButton()
{
	this->x = 0;
	this->y = 0;
	this->height = 0;
	this->width = 0;
	this->hBitmap = NULL;
	this->hButton = NULL;
	this->mValue = 0;
}

RoundButton::RoundButton(int mValue)
{
	this->x = 0;
	this->y = 0;
	this->height = 0;
	this->width = 0;
	this->hBitmap = NULL;
	this->hButton = NULL;
	this->mValue = mValue;
}

RoundButton::RoundButton(int x, int y, int mValue)
{
	this->x = x;
	this->y = y;
	this->height = 0;
	this->width = 0;
	this->hBitmap = NULL;
	this->hButton = NULL;
	this->mValue = mValue;
}

HWND RoundButton::GetButtonHandle(HWND hWnd, int width, int height)
{
	return CreateWindowExW(0, L"BUTTON", L"", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | BS_BITMAP | BS_PUSHBUTTON,
		this->x, this->y, width, height, hWnd, (HMENU)this->mValue, 0, 0);
}


HBITMAP RoundButton::GetBitmapHandle(LPCWSTR bitmapname)
{
	return LoadBitmap(GetModuleHandle(0), bitmapname);
}

int RoundButton::GetHeight()
{
	return this->height;
}

int RoundButton::GetMenuValue()
{
	return mValue;
}

int RoundButton::GetWidth()
{
	return this->width;
}

int RoundButton::GetX()
{
	return this->x;
}

int RoundButton::GetY()
{
	return this->y;
}

void RoundButton::Create(HWND windowhandle)
{	
	this->hButton = GetButtonHandle(windowhandle, this->width, this->height);
	HRGN hRgn = CreateEllipticRgn(0, 0, this->width, this->height);
	SetWindowRgn(this->hButton, hRgn, false);
	SendMessage(this->hButton, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)this->hBitmap);
}

void RoundButton::SetBitmap(LPCWSTR bitmapname)
{
	this->hBitmap = GetBitmapHandle(bitmapname);
	BITMAP bmp;
	GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&bmp);
	this->height = bmp.bmHeight;
	this->width = bmp.bmWidth;
	SendMessage(this->hButton, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)this->hBitmap);
}


void RoundButton::LoadButton(LPCWSTR bitmapname)
{
	this->hBitmap = GetBitmapHandle(bitmapname);
	BITMAP bmp;
	GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&bmp);
	this->height = bmp.bmHeight;
	this->width = bmp.bmWidth;
}

void RoundButton::Create(HWND hWnd, int x, int y)
{
	this->x = x;
	this->y = y;
	this->hButton = GetButtonHandle(hWnd, this->width, this->height);
	HRGN hRgn = CreateEllipticRgn(0, 0, this->width, this->height);
	SetWindowRgn(this->hButton, hRgn, false);
	SendMessage(this->hButton, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)this->hBitmap);
}

RoundButton::~RoundButton()
{

}