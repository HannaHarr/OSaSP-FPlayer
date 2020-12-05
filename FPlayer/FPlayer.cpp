// FPlayer.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "FPlayer.h"
#include "ProgramInfo.h"
#include "RoundButton.h"
#include <shellapi.h>
#include <strsafe.h>
#include <string>
#include <commdlg.h>
#include "Progressbar.h"
#include "TextBox.h"
#include "MediaPlayer.h"
#include "Song.h"

#pragma comment(linker, "\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#define MAX_LOADSTRING 100
#define IDM_EXIT 10000

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int, ProgramInfo*);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

// Круглые кнопки
RoundButton bbStop(1003);
RoundButton bbPlayPause(IDM_PLAYPAUSE);
RoundButton bbRewindBack(IDM_REWINDBACK);
RoundButton bbRewindForward(IDM_REWINDFORWARD);
RoundButton bbVolume(1008);
RoundButton bbOpenFile(IDM_OPENSONG);

// Составные части медиаплеера
MediaPlayer mediaPlayer;
LPCWSTR cmdArg;

// Структура для отображения уведомлений в области уведомлений
NOTIFYICONDATA notifyIconData;
UINT const WMAPP_NOTIFYCALLBACK = WM_APP + 1;

// Объявление функций
void ProcessWM_PAINT(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

void ProcessWM_CREATE(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

void ProcessWM_COMMAND(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

void ProcessWM_SIZE(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

void ProcessWM_LBUTTONDOWN(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

void ProcessWM_NOTIFYCALLBACK(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

void ProcessWM_TIMER(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

void CreateNotifyIcon(HWND hWnd, NOTIFYICONDATA* nf);

void CreateNotifyIconContextMenu(HWND hWnd);

void Stop(HWND hWnd);

void LoadAndPlay(HWND hWnd, LPCWSTR url);

void RunningLine();

/// <summary>
/// Входная точка программы
/// </summary>
/// <param name="hInstance"></param>
/// <param name="hPrevInstance"></param>
/// <param name="lpCmdLine"></param>
/// <param name="nCmdShow"></param>
/// <returns></returns>
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    ProgramInfo programinfo;
    cmdArg = lpCmdLine;

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_FPLAYER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance(hInstance, nCmdShow, &programinfo))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_FPLAYER));

    MSG msg;

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}

/// <summary>
/// Региструет главный оконный класс
/// </summary>
/// <param name="hInstance"></param>
/// <returns></returns>
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPICON));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    //wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.hbrBackground = CreateSolidBrush(RGB(0, 123, 207));
    wcex.lpszMenuName = 0;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPICON));

    return RegisterClassExW(&wcex);
}

/// <summary>
/// Создает главное окно приложения
/// </summary>
/// <param name="hInstance"></param>
/// <param name="nCmdShow"></param>
/// <param name="programinfo"></param>
/// <returns></returns>
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow, ProgramInfo* programinfo)
{
    hInst = hInstance;

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_DLGFRAME | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZE,
        programinfo->getX(), programinfo->getY(), programinfo->getWidth(), programinfo->getHeight(),
        nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

/// <summary>
/// Обрабатывает сообщения главного окна приложения
/// </summary>
/// <param name="hWnd"></param>
/// <param name="message"></param>
/// <param name="wParam"></param>
/// <param name="lParam"></param>
/// <returns></returns>
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        ProcessWM_CREATE(hWnd, message, wParam, lParam);
    }
    break;

    case WM_TIMER:
    {
        ProcessWM_TIMER(hWnd, message, wParam, lParam);
    }
    break;

    case WM_COMMAND:
    {
        ProcessWM_COMMAND(hWnd, message, wParam, lParam);
    }
    break;

    case WM_PAINT:
    {
        ProcessWM_PAINT(hWnd, message, wParam, lParam);
    }
    break;

    case WMAPP_NOTIFYCALLBACK:
    {
        ProcessWM_NOTIFYCALLBACK(hWnd, message, wParam, lParam);
    }
    break;

    case WM_LBUTTONDOWN:
    {
        ProcessWM_LBUTTONDOWN(hWnd, message, wParam, lParam);
    }
    break;

    case WM_DESTROY:
    {
        PostQuitMessage(0);
    }
    break;

    case WM_SIZE:
    {
        ProcessWM_SIZE(hWnd, message, wParam, lParam);
    }

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

/// <summary>
/// Обрабатывает сообщение WM_PAINT
/// </summary>
/// <param name="hWnd"></param>
/// <param name="message"></param>
/// <param name="wParam"></param>
/// <param name="lParam"></param>
void ProcessWM_PAINT(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    RECT rect;
    HDC hdc = BeginPaint(hWnd, &ps);
    GetClientRect(hWnd, &rect);

    mediaPlayer.Display(hdc);
    EndPaint(hWnd, &ps);
}

/// <summary>
/// Обрабатывает сообщение WM_CREATE
/// </summary>
/// <param name="hWnd"></param>
/// <param name="message"></param>
/// <param name="wParam"></param>
/// <param name="lParam"></param>
void ProcessWM_CREATE(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    bbPlayPause.LoadButton(MAKEINTRESOURCE(IDB_BBPLAY));
    bbStop.LoadButton(MAKEINTRESOURCE(IDB_BBSTOP));
    bbRewindForward.LoadButton(MAKEINTRESOURCE(IDB_BBREWINDFORWARD));
    bbRewindBack.LoadButton(MAKEINTRESOURCE(IDB_BBREWINDBACK));
    bbVolume.LoadButton(MAKEINTRESOURCE(IDB_BBVOLUMEENABLED));
    bbOpenFile.LoadButton(MAKEINTRESOURCE(IDB_BBOPENFILE));

    RECT rect;
    GetClientRect(hWnd, &rect);
    int y = rect.top + 10;
    bbOpenFile.Create(hWnd, rect.left + 10, y);
    bbRewindBack.Create(hWnd, (rect.right * 2 / 10) - bbRewindBack.GetWidth() / 2, y);
    bbPlayPause.Create(hWnd, (rect.right * 3 / 10) - bbPlayPause.GetWidth() / 2, y);
    bbStop.Create(hWnd, (rect.right * 4 / 10) - bbStop.GetWidth() / 2, y);
    bbRewindForward.Create(hWnd, (rect.right * 5 / 10) - bbRewindForward.GetWidth() / 2, y);
    bbVolume.Create(hWnd, (rect.right * 7.3 / 10) - bbRewindBack.GetWidth() / 2, y);
    mediaPlayer.Init(rect);

    if (wcslen(cmdArg) > 0)
    {
        std::wstring str = cmdArg;
        int index1 = str.find_first_of(L"\"");
        int index2 = str.find_last_of(L"\"");
        if (index1 != -1)
            str = str.substr(index1 + 1);
        if (index2 != -1)
            str = str.substr(0, index2 - 1);
        LoadAndPlay(hWnd, str.c_str());
        InvalidateRect(hWnd, NULL, TRUE);
    }
}

/// <summary>
/// Обрабатывает сообщение WM_COMMAND
/// </summary>
/// <param name="hWnd"></param>
/// <param name="message"></param>
/// <param name="wParam"></param>
/// <param name="lParam"></param>
void ProcessWM_COMMAND(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (LOWORD(wParam) == bbPlayPause.GetMenuValue())
    {
        PlayerState state = mediaPlayer.player->GetState();
        if ((state == PlayerState::Paused) || (state == PlayerState::Stopped))
        {
            mediaPlayer.player->Play();
            SetTimer(hWnd, 1, 1000, NULL);
            bbPlayPause.SetBitmap(MAKEINTRESOURCE(IDB_BBPAUSE));
        }
        else if (state == PlayerState::Started)
        {
            KillTimer(hWnd, 1);
            mediaPlayer.player->Pause();
            bbPlayPause.SetBitmap(MAKEINTRESOURCE(IDB_BBPLAY));
        }
    }
    else if (LOWORD(wParam) == bbStop.GetMenuValue())
    {
        Stop(hWnd);
        InvalidateRect(hWnd, NULL, TRUE);
    }
    else if (LOWORD(wParam) == bbRewindForward.GetMenuValue())
    {
        mediaPlayer.AddProgress();
        mediaPlayer.Update();
        InvalidateRect(hWnd, NULL, TRUE);
    }
    else if (LOWORD(wParam) == bbRewindBack.GetMenuValue())
    {
        mediaPlayer.SubstractProgress();
        mediaPlayer.Update();
        InvalidateRect(hWnd, NULL, TRUE);
    }
    else if (LOWORD(wParam) == bbVolume.GetMenuValue())
    {
        if (mediaPlayer.IsVolumeEnabled())
        {
            mediaPlayer.DisableVolume();
            bbVolume.SetBitmap(MAKEINTRESOURCE(IDB_BBVOLUMEDISABLED));
        }
        else
        {
            mediaPlayer.EnableVolume();
            bbVolume.SetBitmap(MAKEINTRESOURCE(IDB_BBVOLUMEENABLED));
        }
    }
    else if (LOWORD(wParam) == IDM_OPENSONG)
    {
        OPENFILENAME ofn;
        wchar_t szFile[260];
        HANDLE hf;
        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = hWnd;
        ofn.lpstrFile = szFile;
        ofn.lpstrFile[0] = '\0';
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFilter = L"Mp3 files(*.mp3)\0*.mp3\0Wav files(*.wav)\0*.wav\0All files(*.*)\0*.*\0\0";
        ofn.nFilterIndex = 1;
        ofn.lpstrFileTitle = NULL;
        ofn.nMaxFileTitle = 0;
        ofn.lpstrInitialDir = NULL;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER;

        // Display the Open dialog box. 
        if (GetOpenFileName(&ofn) == TRUE)
        {
            PlayerState state = mediaPlayer.player->GetState();
            if (state == PlayerState::Started || state == PlayerState::Paused)
            {
                Stop(hWnd);
            }
            LoadAndPlay(hWnd, ofn.lpstrFile);
            InvalidateRect(hWnd, NULL, TRUE);
        }
    }
    else if (LOWORD(wParam) == IDM_EXIT)
    {
        DestroyWindow(hWnd);
    }
}

/// <summary>
/// Обрабатывает сообщение WM_SIZE
/// </summary>
/// <param name="hWnd"></param>
/// <param name="message"></param>
/// <param name="wParam"></param>
/// <param name="lParam"></param>
void ProcessWM_SIZE(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (wParam == SIZE_MINIMIZED)
    {
        CreateNotifyIcon(hWnd, &notifyIconData);
        Shell_NotifyIcon(NIM_ADD, &notifyIconData);
        ShowWindow(hWnd, SW_HIDE);
    }

    if (wParam == SIZE_RESTORED)
    {
        Shell_NotifyIcon(NIM_DELETE, &notifyIconData);
    }
}

/// <summary>
/// Обработка сообщений из области уведомлений
/// </summary>
/// <param name="hWnd"></param>
/// <param name="message"></param>
/// <param name="wParam"></param>
/// <param name="lParam"></param>
void ProcessWM_NOTIFYCALLBACK(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (LOWORD(lParam))
    {
    case WM_LBUTTONDOWN:
    {
        ShowWindow(hWnd, SW_SHOW);
        ShowWindow(hWnd, SW_RESTORE);
    }
    break;

    case WM_RBUTTONDOWN:
    {
        CreateNotifyIconContextMenu(hWnd);
    }
    break;
    }
}

/// <summary>
/// Обрабатывает нажатие левой клавиши мыши
/// </summary>
/// <param name="hWnd"></param>
/// <param name="message"></param>
/// <param name="wParam"></param>
/// <param name="lParam"></param>
void ProcessWM_LBUTTONDOWN(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    POINT ptMousePos = { LOWORD(lParam), HIWORD(lParam) };
    if (mediaPlayer.SoundtrackProgressBar.CheckPoint(ptMousePos.x, ptMousePos.y))
    {
        PlayerState state = mediaPlayer.player->GetState();
        if (state == PlayerState::Started)
        {
            int progress = ptMousePos.x - mediaPlayer.SoundtrackProgressBar.getRect().left;
            mediaPlayer.SetTimeByProgress(progress);
            mediaPlayer.Update();
            InvalidateRect(hWnd, NULL, TRUE);
        }
    }
    else if (mediaPlayer.VolumeProgressBar.CheckPoint(ptMousePos.x, ptMousePos.y))
    {
        int progress = ptMousePos.x - mediaPlayer.VolumeProgressBar.getRect().left;
        mediaPlayer.SetVolumeProgress(progress);
        InvalidateRect(hWnd, NULL, TRUE);
    }
}

/// <summary>
/// Обрабатывает сообщения от таймера
/// </summary>
/// <param name="hWnd"></param>
/// <param name="message"></param>
/// <param name="wParam"></param>
/// <param name="lParam"></param>
void ProcessWM_TIMER(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PlayerState state = mediaPlayer.player->GetState();
    if (state == PlayerState::Started)
    {
        mediaPlayer.secondsPlayed++;
        mediaPlayer.Update();
        InvalidateRect(hWnd, NULL, TRUE);
    }
}

/// <summary>
/// Создает иконку в панели уведомлений
/// </summary>
/// <param name="hWnd"></param>
/// <param name="nf"></param>
void CreateNotifyIcon(HWND hWnd, NOTIFYICONDATA* nf)
{
    nf->hWnd = hWnd;
    nf->uID = NULL;
    nf->uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    StringCchCopy(nf->szTip, ARRAYSIZE(nf->szTip), L"Foudre");
    nf->uCallbackMessage = WMAPP_NOTIFYCALLBACK;
    nf->hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_APPICON));
}

/// <summary>
/// Создает в панели уведомлений контекстное меню
/// </summary>
/// <param name="hWnd"></param>
void CreateNotifyIconContextMenu(HWND hWnd)
{
    POINT point;
    GetCursorPos(&point);
    HMENU hPopupMenu = CreatePopupMenu();
    InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, IDM_EXIT, L"Exit");
    InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_SEPARATOR, 0, L"");
    InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, bbPlayPause.GetMenuValue(), L"Play/pause");
    InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, IDM_OPENSONG, L"Open file");
    SetForegroundWindow(hWnd);
    TrackPopupMenuEx(hPopupMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, point.x, point.y, hWnd, NULL);
}


void Stop(HWND hWnd)
{
    KillTimer(hWnd, 1);
    KillTimer(hWnd, 2);
    mediaPlayer.SetTimeByProgress(0);
    bbPlayPause.SetBitmap(MAKEINTRESOURCE(IDB_BBPLAY));
    mediaPlayer.Update();
    mediaPlayer.player->Stop();
}

void LoadAndPlay(HWND hWnd, LPCWSTR url)
{
    mediaPlayer.LoadSong(url);
    mediaPlayer.player->Play();
    std::wstring temp = mediaPlayer.TextBoxSong.getText();
    temp.append(L"                       ");
    mediaPlayer.TextBoxSong.SetText(temp);
    mediaPlayer.SetVolumeProgress(mediaPlayer.VolumeProgressBar.getProgress());
    bbPlayPause.SetBitmap(MAKEINTRESOURCE(IDB_BBPAUSE));
    SetTimer(hWnd, 1, 1000, NULL);
    SetTimer(hWnd, 2, 750, (TIMERPROC)RunningLine);
}

void RunningLine()
{
    std::wstring temp = mediaPlayer.TextBoxSong.getText();
    wchar_t first = temp.data()[0];
    for (int i = 1; i < temp.size(); i++)
    {
        temp[i - 1] = temp.data()[i];
    }
    temp[temp.size() - 1] = first;
    mediaPlayer.TextBoxSong.SetText(temp);
}
