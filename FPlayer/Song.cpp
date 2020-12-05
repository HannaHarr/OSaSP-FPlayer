#include "Song.h"

Song::Song(PCWSTR sURL, HRESULT *pHR)
{
	SongURL = sURL;
	Title = L"";
	AlbumArtist = L"";
	AlbumTitle = L"";
	Genre = L"";

	HRESULT hr = S_OK;
	CComPtr<IPropertyStore> propertyStore;

	hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	if (SUCCEEDED(hr)) 
	{
		hr = SHGetPropertyStoreFromParsingName(sURL, NULL, GPS_DEFAULT, IID_PPV_ARGS(&propertyStore));
		if (SUCCEEDED(hr))
		{
			PROPVARIANT prop;
			HRESULT rs;
			PropVariantInit(&prop);

			rs = propertyStore->GetValue(PKEY_Title, &prop);
			if (SUCCEEDED(rs) && IsPropVariantString(prop))
			{
				Title = PropVariantToStringWithDefault(prop, L"");
			}

			rs = propertyStore->GetValue(PKEY_Music_AlbumArtist, &prop);
			if (SUCCEEDED(rs) && IsPropVariantString(prop))
			{
				AlbumArtist = PropVariantToStringWithDefault(prop, L"");
			}

			rs = propertyStore->GetValue(PKEY_Music_AlbumTitle, &prop);
			if (SUCCEEDED(rs) && IsPropVariantString(prop))
			{
				AlbumTitle = PropVariantToStringWithDefault(prop, L"");
			}

			rs = propertyStore->GetValue(PKEY_Music_Genre, &prop);
			if (SUCCEEDED(rs) && IsPropVariantString(prop))
			{
				Genre = PropVariantToStringWithDefault(prop, L"");
			}

			PropVariantClear(&prop);
		}

		CoUninitialize();
	}

	*pHR = hr;
}
