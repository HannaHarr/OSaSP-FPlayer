#include "MediaPlayer.h"
#include "Song.h"

MediaPlayer::MediaPlayer()
{
	
}

MediaPlayer::MediaPlayer(RECT rect)
{

}

MediaPlayer::~MediaPlayer()
{
    delete this->player;
    delete this->song;
}

void MediaPlayer::Init(RECT rect)
{
    this->SoundtrackProgressBar.LoadCoordinates(rect.left + 10, rect.bottom - 60, rect.right - 20, rect.bottom / 6);
    this->SoundtrackProgressBar.SetBorderColors(255, 216, 181);
    this->VolumeProgressBar.LoadCoordinates(rect.right - 110, rect.top + 25, 100, 15);
    this->VolumeProgressBar.SetBorderColors(255, 216, 181);

    this->TextBoxSong.SetRect(rect.left + 10, rect.bottom / 2 - 15, rect.right - 20, rect.bottom / 2 + 50);
    this->TextBoxSong.SetText(this->defaultSongText);

    this->TextBoxTime.SetRect(rect.left + 10, rect.bottom - 30, 200, 150);
    this->TextBoxTime.SetText(this->defaultTimeText);
    this->TextBoxTime.SetFontCharacteristics(20, 10, 10, 0);
    this->player = new Player(&Handle);
}

void MediaPlayer::Display(HDC hdc)
{
    this->SoundtrackProgressBar.Paint(hdc);
    this->VolumeProgressBar.Paint(hdc);
    SetBkMode(hdc, TRANSPARENT);
    this->TextBoxSong.ShowText(hdc, DT_CENTER);
    this->TextBoxTime.ShowText(hdc, DT_LEFT);
}

void MediaPlayer::Update()
{
    MFTIME time;
    this->player->GetDuration(&time);
    int seconds = this->ConvertMFTIMEToSeconds(time);
    std::wstring strSongTime = this->GetTimeInStr(this->secondsPlayed);
    if (this->secondsPlayed >= seconds)
    {
        this->secondsPlayed = 0;
        seconds = 0;
    }
    else
    {
        strSongTime += L"/" + this->GetTimeInStr(seconds);
        this->TextBoxTime.SetText(strSongTime.c_str());
        this->SoundtrackProgressBar.SetDelta(this->SoundtrackProgressBar.getWidth() / seconds);
        this->SoundtrackProgressBar.Update();
    }
}

void MediaPlayer::LoadSong(PCWSTR songname)
{
    this->player = new Player(&Handle);
    this->song = new Song(songname, &Handle);
    Handle = this->player->OpenURL(songname);
    std::wstring songTitle = song->Title;
    if (songTitle.size() == 0)
    {
        std::wstring songName = songname;
        this->TextBoxSong.SetText(songName);
    }
    else
        this->TextBoxSong.SetText(song->Title);
    this->Update();  
}

void MediaPlayer::EnableVolume()
{
    this->isVolumeEnabled = true;
    this->player->SetVolume(volume);
}

bool MediaPlayer::IsVolumeEnabled()
{
    return this->isVolumeEnabled;
}

void MediaPlayer::DisableVolume()
{
    this->isVolumeEnabled = false;
    this->player->SetVolume(0);
}

void MediaPlayer::SetTimeByProgress(int x)
{
    this->SoundtrackProgressBar.SetProgress(x);
    MFTIME time;
    this->player->GetDuration(&time);
    MFTIME currentTime = time * this->SoundtrackProgressBar.getProgress() / this->SoundtrackProgressBar.getWidth();
    this->player->SetPosition(currentTime);
    this->secondsPlayed = this->ConvertMFTIMEToSeconds(currentTime);
}

void MediaPlayer::AddProgress()
{
    MFTIME time;
    this->player->GetDuration(&time);
    int songLength = this->ConvertMFTIMEToSeconds(time);
    this->secondsPlayed += 10;
    if (this->secondsPlayed > songLength)
        this->secondsPlayed = songLength;
    this->player->SetPosition(this->ConvertSecondsToMFTIME(this->secondsPlayed));
    this->SoundtrackProgressBar.SetProgress(this->secondsPlayed * this->SoundtrackProgressBar.getWidth() / songLength);
}

void MediaPlayer::SubstractProgress()
{
    MFTIME time;
    this->player->GetDuration(&time);
    int songLength = this->ConvertMFTIMEToSeconds(time);
    this->secondsPlayed -= 10;
    if (this->secondsPlayed < 0)
        this->secondsPlayed = 0;
    this->player->SetPosition(this->ConvertSecondsToMFTIME(this->secondsPlayed));
    this->SoundtrackProgressBar.SetProgress(this->secondsPlayed * this->SoundtrackProgressBar.getWidth() / songLength);
}

void MediaPlayer::SetVolumeProgress(int x)
{
    this->VolumeProgressBar.SetProgress(x);
    this->volume = this->MAXVOLUME * this->VolumeProgressBar.getProgress() / this->VolumeProgressBar.getWidth();
    if (this->IsVolumeEnabled())
    {
        this->player->SetVolume(this->volume);
    }
}

int MediaPlayer::ConvertMFTIMEToSeconds(MFTIME time)
{
    const MFTIME ONE_SECOND = 10000000;
    return time / ONE_SECOND;
}

MFTIME MediaPlayer::ConvertSecondsToMFTIME(int seconds)
{
    const MFTIME ONE_SECOND = 10000000;
    MFTIME temp = ONE_SECOND * seconds;
    return temp;
}

std::wstring MediaPlayer::GetTimeInStr(int time)
{
    const int MinutesPerHour = 60;
    const int SecondsPerMinute = 60;
    
    int seconds = time % SecondsPerMinute;
    int minutes = time / SecondsPerMinute;
    int hours = minutes / MinutesPerHour;
    minutes = minutes % MinutesPerHour;

    WCHAR arrayseconds[4];
    WCHAR arrayminutes[4];
    WCHAR arrayhours[4];
    _itow_s(seconds, arrayseconds, 10);
    _itow_s(minutes, arrayminutes, 10);
    _itow_s(hours, arrayhours, 10);
    LPCWSTR strseconds = arrayseconds;
    LPCWSTR strminutes = arrayminutes;
    LPCWSTR strhours = arrayhours;
    LPCWSTR separator = L":";
    LPCWSTR zero = L"0";

    std::wstring df = L"";

    if (hours < 10)
        df += zero + std::wstring(strhours);
    else
        df += std::wstring(strhours);

    df += separator;

    if (minutes < 10)
        df += zero + std::wstring(strminutes);
    else
        df += std::wstring(strminutes);

    df += separator;

    if (seconds < 10)
        df += zero + std::wstring(strseconds);
    else
        df += std::wstring(strseconds);
    return df;
}