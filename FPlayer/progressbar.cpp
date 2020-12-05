#include "Progressbar.h"
#include <Windows.h>

ProgressBar::ProgressBar()
{
	this->x = 0;
	this->y = 0;
	this->height = 0;
	this->width = 0;
	this->redBorder = defaultRedBorder;
	this->greenBorder = defaultGreenBorder;
	this->blueBorder = defaultBlueBorder;
	this->redRegion = defaultRedRegion;
	this->greenRegion = defaultGreenRegion;
	this->blueRegion = defaultBlueRegion;
	this->progressX = 0;
}

ProgressBar::ProgressBar(int x, int y)
{
	this->x = x;
	this->y = y;
	this->height = 0;
	this->width = 0;
	this->redBorder = defaultRedBorder;
	this->greenBorder = defaultGreenBorder;
	this->blueBorder = defaultBlueBorder;
	this->redRegion = defaultRedRegion;
	this->greenRegion = defaultGreenRegion;
	this->blueRegion = defaultBlueRegion;
	this->progressX = 0;
}

ProgressBar::ProgressBar(int x, int y, int width, int height)
{
	this->x = x;
	this->y = y;
	this->height = height;
	this->width = width;
	this->redBorder = defaultRedBorder;
	this->greenBorder = defaultGreenBorder;
	this->blueBorder = defaultBlueBorder;
	this->redRegion = defaultRedRegion;
	this->greenRegion = defaultGreenRegion;
	this->blueRegion = defaultBlueRegion;
	this->progressX = 0;
}

ProgressBar::~ProgressBar()
{

}

bool ProgressBar::CheckPoint(int x, int y)
{
	bool rightX = ((x > this->x) && (x < this->x + this->width));
	bool rightY = ((y > this->y) && (y < this->y + this->height));
	if (rightX && rightY)
		return true;
	else
		return false;
}

int ProgressBar::getRedBorder()
{
	return this->redBorder;
}

int ProgressBar::getGreenBorder()
{
	return this->greenBorder;
}

int ProgressBar::getBlueBorder()
{
	return this->blueBorder;
}

void ProgressBar::SetBorderColors(int red, int green, int blue)
{
	this->redBorder = red;
	this->greenBorder = green;
	this->blueBorder = blue;
}

void ProgressBar::SetRegionColors(int red, int green, int blue)
{
	this->redRegion = red;
	this->greenRegion = green;
	this->blueRegion = blue;
}

int ProgressBar::getRedRegion()
{
	return this->redRegion;
}

int ProgressBar::getGreenRegion()
{
	return this->greenRegion;
}

int ProgressBar::getBlueRegion()
{
	return this->blueRegion;
}

void ProgressBar::Paint(HDC hdc)
{
	RECT rect;
	rect.left = this->x;
	rect.top = this->y;
	rect.right = this->x + this->width;
	rect.bottom = this->y + this->height;

	COLORREF colorBorder = RGB(this->redBorder,
							   this->greenBorder,
							   this->blueBorder);
	COLORREF colorRegion = RGB(this->redRegion,
							   this->greenRegion,
							   this->blueRegion);
	HBRUSH hBrushBorder = CreateSolidBrush(colorBorder);
	HBRUSH hBrushRegion = CreateSolidBrush(colorRegion);

	FillRect(hdc, &rect, hBrushBorder);

	rect.left = this->x + indent;
	rect.top = this->y + indent;
	rect.right = this->x + this->progressX;
	rect.bottom = this->y + this->height - indent;
	FillRect(hdc, &rect, hBrushRegion);
}

void ProgressBar::Update()
{
	this->progressX += this->delta;
	if ((this->progressX < this->x) || (this->progressX > this->x + this->width))
	{
		this->progressX = 0;
	}
}

void ProgressBar::SetProgress(int x)
{
	this->progressX = x;
}

int ProgressBar::getProgress()
{
	return this->progressX;
}

int ProgressBar::getWidth()
{
	return this->width;
}

void ProgressBar::SetDelta(int delta)
{
	this->delta = delta;
}

RECT ProgressBar::getRect()
{
	RECT temp;
	temp.left = this->x;
	temp.right = this->x + this->width;
	temp.top = this->y;
	temp.bottom = this->y + this->height;
	return temp;
}

void ProgressBar::LoadCoordinates(int x, int y, int width, int height)
{
	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;
}

int ProgressBar::getIndent()
{
	return this->indent;
}