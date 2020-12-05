#include <Windows.h>

#pragma once
class ProgressBar
{
public:
	ProgressBar();
	ProgressBar(int x, int y);
	ProgressBar(int x, int y, int width, int height);
	~ProgressBar();
	bool CheckPoint(int x, int y);
	void LoadCoordinates(int x, int y, int width, int height);
	int getRedBorder();
	int getGreenBorder();
	int getBlueBorder();
	int getRedRegion();
	int getGreenRegion();
	int getBlueRegion();
	void Paint(HDC hdc);
	void Update();
	int getWidth();
	void SetProgress(int x);
	int getProgress();
	void SetDelta(int delta);
	int getIndent();
	RECT getRect();
	void SetBorderColors(int red, int green,int blue);
	void SetRegionColors(int red, int green,int blue);
private:
	int x;
	int y;
	int height;
	int width;
	int redBorder;
	int greenBorder;
	int blueBorder;
	int redRegion;
	int greenRegion;
	int blueRegion;
	int progressX;
	const int defaultRedBorder = 255;
	const int defaultGreenBorder = 255;
	const int defaultBlueBorder = 255;
	const int defaultRedRegion = 223;
	const int defaultGreenRegion = 87;
	const int defaultBlueRegion = 29;
	const int indent = 3;
	int delta = 20;
};

