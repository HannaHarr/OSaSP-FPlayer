#pragma once

class ProgramInfo {

private:
    const int defaultWidth = 500;
    const int defaultHeight = 200;
    const int defaultX = 300;
    const int defaultY = 300;
    int height;
    int width;
    int x;
    int y;

public:
    ProgramInfo();
    ProgramInfo(int x, int y, int width, int height);
    ~ProgramInfo();
    int getWidth();
    int getHeight();
    int getX();
    int getY();
};