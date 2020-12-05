#pragma once

#include "progressbar.h"
#include "TextBox.h"
#include "Player.h"
#include "Song.h"
#include <string>

class MediaPlayer
{
public:
	ProgressBar SoundtrackProgressBar;
	ProgressBar VolumeProgressBar;
	TextBox TextBoxSong;
	TextBox TextBoxTime;
	Player* player;
	HRESULT Handle;
	MediaPlayer();
	MediaPlayer(RECT rect);
	~MediaPlayer();
	void LoadSong(PCWSTR songname);
	void Init(RECT rect);
	void Display(HDC hdc);
	void Update();
	bool IsVolumeEnabled();
	void SetTimeByProgress(int x);
	void EnableVolume();
	void DisableVolume();
	void SetVolumeProgress(int x);
	void AddProgress();
	void SubstractProgress();
	int secondsPlayed = 0;
private:
	Song* song;
	bool isVolumeEnabled = true;
	const float MAXVOLUME = 1.0;
	const float MINVOLUME = 0.0;
	float volume = MAXVOLUME;
	std::wstring GetTimeInStr(int time);
	int ConvertMFTIMEToSeconds(MFTIME time);
	MFTIME ConvertSecondsToMFTIME(int seconds);
	const std::wstring defaultSongText = L"NONAME";
	const std::wstring defaultTimeText = L"00:00:00/00:00:00";
};

