#pragma once

#include "TopoBuilder.h"

enum class PlayerState
{
    Closed,         // Нет сессии
    Ready,          // Сессия была создана, готов к открытию файла
    OpenPending,    // Файл открыт
    Started,        // Файл играет
    Paused,         // Файл на паузе
    Seek,           // Перемотка
    Stopped,        // Сессия остановлена
    Closing         // Ожидается MESessionClosed
};

class Player : public IMFAsyncCallback
{
public:

    /// <summary>
    /// Конструктор
    /// </summary>
    /// <param name="pHr"></param>
    Player(HRESULT* pHR);
    ~Player();
    
    /// <summary>
    /// Поставить файл на воспроизведение
    /// </summary>
    /// <param name="sURL"> строка адреса </param>
    /// <returns></returns>
    HRESULT OpenURL(PCWSTR sURL);

    /// <summary>
    /// Запустить 
    /// </summary>
    /// <returns></returns>
    HRESULT Play();

    /// <summary>
    /// Приостановить
    /// </summary>
    /// <returns></returns>
    HRESULT Pause();

    /// <summary>
    /// Остановить
    /// </summary>
    /// <returns></returns>
    HRESULT Stop();

    /// <summary>
    /// Получить текущее состояние
    /// </summary>
    /// <returns></returns>
    PlayerState GetState();

    /// <summary>
    /// Возможность перемотки
    /// </summary>
    /// <param name="canSeek"></param>
    /// <returns></returns>
    HRESULT CanSeek(BOOL* canSeek);

    /// <summary>
    /// Получить продолжительность песни
    /// </summary>
    /// <param name="duration"></param>
    /// <returns></returns>
    HRESULT GetDuration(MFTIME* duration);

    /// <summary>
    /// Получить текущую позицию воспроизведения
    /// </summary>
    /// <param name="position"></param>
    /// <returns></returns>
    HRESULT GetCurrentPosition(MFTIME* position);

    /// <summary>
    /// Установить позицию воспроизведения
    /// </summary>
    /// <param name="position"></param>
    /// <returns></returns>
    HRESULT SetPosition(MFTIME position);

    /// <summary>
    /// Получить текущую громкость
    /// </summary>
    /// <param name="volume"></param>
    /// <returns></returns>
    HRESULT GetVolume(float* volume);

    /// <summary>
    /// Установить громкость
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
    /// Создать сессию
    /// </summary>
    /// <returns></returns>
    HRESULT CreateSession();

    /// <summary>
    /// Закрыть сессию
    /// </summary>
    /// <returns></returns>
    HRESULT CloseSession();

    /// <summary>
    /// Начать воспроизведение
    /// </summary>
    /// <returns></returns>
    HRESULT StartPlayback();
    
    /// <summary>
    /// Обработка события MF
    /// </summary>
    /// <param name="mediaEvent"></param>
    /// <returns></returns>
    HRESULT ProcessMediaEvent(CComPtr<IMFMediaEvent>& mediaEvent);

    /// <summary>
    /// Обработка события готовности топологии
    /// </summary>
    /// <returns></returns>
    HRESULT OnTopologyReady();

    /// <summary>
    /// Количество ссылок
    /// </summary>
    long refCount;

    /// <summary>
    /// Критическая секция
    /// </summary>
    CComAutoCriticalSection critSec;

    /// <summary>
    /// Построитель топологии
    /// </summary>
    TopoBuilder topoBuilder;

    /// <summary>
    /// Объект текущей сессии
    /// </summary>
    CComPtr<IMFMediaSession> session;

    /// <summary>
    /// Событие окончательного закрытия сессии
    /// </summary>
    HANDLE closeCompleteEvent;

    struct SeekState
    {
        PlayerState state;
        MFTIME  start;
    };

    /// <summary>
    /// Поставлен ли вопрос на рассмотрение
    /// </summary>
    BOOL IsPending;

    /// <summary>
    /// Текущее состояние
    /// </summary>
    SeekState currentState;

    /// <summary>
    /// Ожидающий запрос
    /// </summary>
    SeekState pendingRequest;

    /// <summary>
    /// Возможности сессии
    /// </summary>
    DWORD sessionCaps;

    /// <summary>
    /// Часы
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
    /// Регулировщик громкости
    /// </summary>
    CComPtr<IMFSimpleAudioVolume> simpleAudioVolume;
};

