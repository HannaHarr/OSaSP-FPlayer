#pragma once

#include "TopoBuilder.h"

enum class PlayerState
{
    Closed,         // ��� ������
    Ready,          // ������ ���� �������, ����� � �������� �����
    OpenPending,    // ���� ������
    Started,        // ���� ������
    Paused,         // ���� �� �����
    Seek,           // ���������
    Stopped,        // ������ �����������
    Closing         // ��������� MESessionClosed
};

class Player : public IMFAsyncCallback
{
public:

    /// <summary>
    /// �����������
    /// </summary>
    /// <param name="pHr"></param>
    Player(HRESULT* pHR);
    ~Player();
    
    /// <summary>
    /// ��������� ���� �� ���������������
    /// </summary>
    /// <param name="sURL"> ������ ������ </param>
    /// <returns></returns>
    HRESULT OpenURL(PCWSTR sURL);

    /// <summary>
    /// ��������� 
    /// </summary>
    /// <returns></returns>
    HRESULT Play();

    /// <summary>
    /// �������������
    /// </summary>
    /// <returns></returns>
    HRESULT Pause();

    /// <summary>
    /// ����������
    /// </summary>
    /// <returns></returns>
    HRESULT Stop();

    /// <summary>
    /// �������� ������� ���������
    /// </summary>
    /// <returns></returns>
    PlayerState GetState();

    /// <summary>
    /// ����������� ���������
    /// </summary>
    /// <param name="canSeek"></param>
    /// <returns></returns>
    HRESULT CanSeek(BOOL* canSeek);

    /// <summary>
    /// �������� ����������������� �����
    /// </summary>
    /// <param name="duration"></param>
    /// <returns></returns>
    HRESULT GetDuration(MFTIME* duration);

    /// <summary>
    /// �������� ������� ������� ���������������
    /// </summary>
    /// <param name="position"></param>
    /// <returns></returns>
    HRESULT GetCurrentPosition(MFTIME* position);

    /// <summary>
    /// ���������� ������� ���������������
    /// </summary>
    /// <param name="position"></param>
    /// <returns></returns>
    HRESULT SetPosition(MFTIME position);

    /// <summary>
    /// �������� ������� ���������
    /// </summary>
    /// <param name="volume"></param>
    /// <returns></returns>
    HRESULT GetVolume(float* volume);

    /// <summary>
    /// ���������� ���������
    /// </summary>
    /// <param name="volume"></param>
    /// <returns></returns>
    HRESULT SetVolume(float volume);

    // IMFAsyncCallback implementation
    STDMETHODIMP GetParameters(DWORD* pdwFlags, DWORD* pdwQueue);
    STDMETHODIMP Invoke(IMFAsyncResult* asyncResult);

    // IUnknown methods
    STDMETHODIMP QueryInterface(REFIID riid, void** ppv);
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();

private:

    /// <summary>
    /// ������� ������
    /// </summary>
    /// <returns></returns>
    HRESULT CreateSession();

    /// <summary>
    /// ������� ������
    /// </summary>
    /// <returns></returns>
    HRESULT CloseSession();

    /// <summary>
    /// ������ ���������������
    /// </summary>
    /// <returns></returns>
    HRESULT StartPlayback();
    
    /// <summary>
    /// ��������� ������� MF
    /// </summary>
    /// <param name="mediaEvent"></param>
    /// <returns></returns>
    HRESULT ProcessMediaEvent(CComPtr<IMFMediaEvent>& mediaEvent);

    /// <summary>
    /// ��������� ������� ���������� ���������
    /// </summary>
    /// <returns></returns>
    HRESULT OnTopologyReady();

    /// <summary>
    /// ���������� ������
    /// </summary>
    long refCount;

    /// <summary>
    /// ����������� ������
    /// </summary>
    CComAutoCriticalSection critSec;

    /// <summary>
    /// ����������� ���������
    /// </summary>
    TopoBuilder topoBuilder;

    /// <summary>
    /// ������ ������� ������
    /// </summary>
    CComPtr<IMFMediaSession> session;

    /// <summary>
    /// ������� �������������� �������� ������
    /// </summary>
    HANDLE closeCompleteEvent;

    struct SeekState
    {
        PlayerState state;
        MFTIME  start;
    };

    /// <summary>
    /// ��������� �� ������ �� ������������
    /// </summary>
    BOOL IsPending;

    /// <summary>
    /// ������� ���������
    /// </summary>
    SeekState currentState;

    /// <summary>
    /// ��������� ������
    /// </summary>
    SeekState pendingRequest;

    /// <summary>
    /// ����������� ������
    /// </summary>
    DWORD sessionCaps;

    /// <summary>
    /// ����
    /// </summary>
    CComPtr<IMFPresentationClock> clock;

    /// <summary>
    /// 
    /// </summary>
    /// <param name="hnsPosition"></param>
    /// <returns></returns>
    HRESULT SetPositionInternal(const MFTIME& hnsPosition);

    /// <summary>
    /// 
    /// </summary>
    /// <param name="state"></param>
    /// <returns></returns>
    HRESULT UpdatePendingCommands(PlayerState state);

    /// <summary>
    /// ������������ ���������
    /// </summary>
    CComPtr<IMFSimpleAudioVolume> simpleAudioVolume;
};

