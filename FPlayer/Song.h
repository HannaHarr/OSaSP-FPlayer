#pragma once

#include "Common.h"

// Property includes
#include <shobjidl.h>
#include <propkey.h>
#include <propvarutil.h>

#pragma comment(lib, "propsys.lib")

class Song
{
public:

	/// <summary>
	/// Строка адреса песни
	/// </summary>
	PCWSTR SongURL;

	/// <summary>
	/// Название
	/// </summary>
	PCWSTR Title;

	/// <summary>
	/// Главный исполнитель
	/// </summary>
	PCWSTR AlbumArtist;

	/// <summary>
	/// Название альбома
	/// </summary>
	PCWSTR AlbumTitle;

	/// <summary>
	/// Жанр
	/// </summary>
	PCWSTR Genre;

	/// <summary>
	/// Конструктор
	/// </summary>
	/// <param name="sURL"> строка адреса песни </param>
	Song(PCWSTR sURL, HRESULT* pHR);
	~Song() {};
};

