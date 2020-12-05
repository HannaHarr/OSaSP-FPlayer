#include "ProgramInfo.h"

/// <summary>
/// 
/// </summary>
/// <returns></returns>
ProgramInfo::ProgramInfo()
{
    this->width = defaultWidth;
    this->height = defaultHeight;
    this->x = defaultX;
    this->y = defaultY;
}

/// <summary>
/// 
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param>
/// <param name="width"></param>
/// <param name="height"></param>
/// <returns></returns>
ProgramInfo::ProgramInfo(int x, int y, int width, int height)
{
    this->x = x;
    this->y = y;
    this->height = height;
    this->width = width;
}

/// <summary>
/// 
/// </summary>
/// <returns></returns>
ProgramInfo::~ProgramInfo()
{
}

/// <summary>
/// 
/// </summary>
/// <returns></returns>
int ProgramInfo::getWidth()
{
    return this->width;
}

/// <summary>
/// 
/// </summary>
/// <returns></returns>
int ProgramInfo::getHeight()
{
    return this->height;
}

/// <summary>
/// 
/// </summary>
/// <returns></returns>
int ProgramInfo::getX()
{
    return this->x;
}

/// <summary>
/// 
/// </summary>
/// <returns></returns>
int ProgramInfo::getY()
{
    return this->y;
}
