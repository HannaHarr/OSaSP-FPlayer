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
	/// ������ ������ �����
	/// </summary>
	PCWSTR SongURL;

	/// <summary>
	/// ��������
	/// </summary>
	PCWSTR Title;

	/// <summary>
	/// ������� �����������
	/// </summary>
	PCWSTR AlbumArtist;

	/// <summary>
	/// �������� �������
	/// </summary>
	PCWSTR AlbumTitle;

	/// <summary>
	/// ����
	/// </summary>
	PCWSTR Genre;

	/// <summary>
	/// �����������
	/// </summary>
	/// <param name="sURL"> ������ ������ ����� </param>
	Song(PCWSTR sURL, HRESULT* pHR);
	~Song() {};
};

