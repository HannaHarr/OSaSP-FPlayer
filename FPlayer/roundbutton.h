#include <windows.h>
#pragma once

class RoundButton
{
public:
	HWND hButton;
	HBITMAP hBitmap;
	RoundButton();
	RoundButton(int mValue);
	RoundButton(int x, int y, int mValue);
	~RoundButton();
	void Create(HWND hWnd);
	void Create(HWND hWnd, int x, int y);
	int GetHeight();
	int GetWidth();
	int GetX();
	int GetY();
	int GetMenuValue();
	void LoadButton(LPCWSTR lpBitmapName);
	void SetBitmap(LPCWSTR bitmapname);
private:
	HBITMAP GetBitmapHandle(LPCWSTR lpBitmapName);
	HWND GetButtonHandle(HWND hWnd, int width, int height);
	int x;
	int y;
	int height;
	int width;
	int mValue;
};

