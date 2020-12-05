#include "Player.h"

#define CMD_PENDING      0x01
#define CMD_PENDING_SEEK 0x02

//// {2715279F-4139-4ba0-9CB1-B351F1B58A4A}
//static const GUID AudioSessionVolumeCtx =
//{ 0x2715279f, 0x4139, 0x4ba0, { 0x9c, 0xb1, 0xb3, 0x51, 0xf1, 0xb5, 0x8a, 0x4a } };

Player::Player(HRESULT *pHR)
{
	closeCompleteEvent = NULL;
	refCount = 1;
	IsPending = false;
	sessionCaps = 0;
	currentState.state = PlayerState::Closed;
	pendingRequest.state = PlayerState::Closed;

	HRESULT hr = S_OK;

	// инициализация Com
	hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	if (SUCCEEDED(hr))
	{
		// инициализация Media Foundation
		hr = MFStartup(MF_VERSION);
		if (SUCCEEDED(hr))
		{
			// создание события
			closeCompleteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
			if (closeCompleteEvent == NULL) 
			{
				hr = E_UNEXPECTED;
			}
		}
	}

	//hr = InitializeAudioSession();

	*pHR = hr;
}

Player::~Player()
{
	CloseSession();

	// закрытие Media Foundation
	MFShutdown();

	// закрытие Com
	CoUninitialize();

	// закрытие события
	CloseHandle(closeCompleteEvent);
}

HRESULT Player::OpenURL(PCWSTR sURL)
{
	CComPtr<IMFTopology> topology;
	CComPtr<IMFPresentationDescriptor> descr;
	HRESULT hr = S_OK;

	CComCritSecLock<CComAutoCriticalSection> lock(critSec);

	if (session == NULL)
	{
		hr = CreateSession();
		if (FAILED(hr)) 
		{
			currentState.state = PlayerState::Closed;
			return hr;
		}
	}

	hr = session->GetSessionCapabilities(&sessionCaps);
	if (FAILED(hr))
	{
		currentState.state = PlayerState::Closed;
		return hr;
	}

	hr = topoBuilder.RenderURL(sURL);
	if (FAILED(hr)) 
	{
		currentState.state = PlayerState::Closed;
		return hr;
	}

	topology = topoBuilder.GetTopology();
	if (topology == NULL) 
	{
		currentState.state = PlayerState::Closed;
		return E_UNEXPECTED;
	}

	hr = session->SetTopology(0, topology);
	if (FAILED(hr)) 
	{
		currentState.state = PlayerState::Closed;
		return hr;
	}

	if (currentState.state == PlayerState::Ready)
		currentState.state = PlayerState::OpenPending;

	return hr;
}

HRESULT Player::Play()
{
	HRESULT hr = S_OK;

	CComCritSecLock<CComAutoCriticalSection> lock(critSec);

	if (IsPending) 
	{
		pendingRequest.state = PlayerState::Started;
	}
	else 
	{
		if (currentState.state != PlayerState::Paused && currentState.state != PlayerState::Stopped)
		{
			return MF_E_INVALIDREQUEST;
		}

		if (session == NULL) return E_UNEXPECTED;

		// play
		hr = StartPlayback();
		if (FAILED(hr)) return hr;

		currentState.state = PlayerState::Started;
	}
	
	return hr;
}

HRESULT Player::Pause()
{
	HRESULT hr = S_OK;

	CComCritSecLock<CComAutoCriticalSection> lock(critSec);

	if (IsPending)
	{
		pendingRequest.state = PlayerState::Paused;
	}
	else
	{
		if (currentState.state != PlayerState::Started)
		{
			return MF_E_INVALIDREQUEST;
		}

		if (session == NULL) return E_UNEXPECTED;

		// pause
		hr = session->Pause();
		if (FAILED(hr)) return hr;

		currentState.state = PlayerState::Paused;
	}
	
	return hr;
}

HRESULT Player::Stop()
{
	HRESULT hr = S_OK;

	CComCritSecLock<CComAutoCriticalSection> lock(critSec);

	if (IsPending)
	{
		pendingRequest.state = PlayerState::Stopped;
	}
	else
	{
		if (currentState.state != PlayerState::Started)
		{
			return MF_E_INVALIDREQUEST;
		}

		if (session == NULL) return E_UNEXPECTED;

		// stop
		hr = session->Stop();
		if (FAILED(hr)) return hr;

		currentState.state = PlayerState::Stopped;
	}
	
	return hr;
}

PlayerState Player::GetState()
{
	return currentState.state;
}

HRESULT Player::CanSeek(BOOL* canSeek)
{
	if (canSeek == NULL)
	{
		return E_POINTER;
	}

	*canSeek = (((sessionCaps & MFSESSIONCAP_SEEK) == MFSESSIONCAP_SEEK) &&
				 (topoBuilder.duration > 0) && (clock != NULL));

	return S_OK;
}

HRESULT Player::GetDuration(MFTIME* _duration)
{
	if (_duration == NULL)
	{
		return E_POINTER;
	}

	*_duration = topoBuilder.duration;

	if (topoBuilder.duration == 0)
	{
		return MF_E_NO_DURATION;
	}
	else
	{
		return S_OK;
	}
}

HRESULT Player::GetCurrentPosition(MFTIME* position)
{
	if (position == NULL)
	{
		return E_POINTER;
	}

	HRESULT hr = S_OK;

	CComCritSecLock<CComAutoCriticalSection> lock(critSec);

	if (clock == NULL)
	{
		return MF_E_NO_CLOCK;
	}

	// Return, in order:
	// 1. Cached seek request (nominal position).
	// 2. Pending seek operation (nominal position).
	// 3. Presentation time (actual position).

	if (pendingRequest.state == PlayerState::Seek)
	{
		*position = pendingRequest.start;
	}
	else if (IsPending & CMD_PENDING_SEEK)
	{
		*position = currentState.start;
	}
	else
	{
		hr = clock->GetTime(position);
	}

	return hr;
}

HRESULT Player::SetPosition(MFTIME position)
{
	HRESULT hr = S_OK;

	CComCritSecLock<CComAutoCriticalSection> lock(critSec);

	if (IsPending)
	{
		pendingRequest.state = PlayerState::Seek;
		pendingRequest.start = position;
	}
	else
	{
		hr = SetPositionInternal(position);
	}

	return hr;
}

STDMETHODIMP_(HRESULT __stdcall) Player::GetParameters(DWORD* pdwFlags, DWORD* pdwQueue)
{
	return E_NOTIMPL;
}

HRESULT Player::Invoke(IMFAsyncResult* asyncResult)
{
	CComPtr<IMFMediaEvent> event;
	HRESULT hr = S_OK;

	CComCritSecLock<CComAutoCriticalSection> lock(critSec);

	if (asyncResult == NULL) return E_UNEXPECTED;

	hr = session->EndGetEvent(asyncResult, &event);
	if (FAILED(hr)) return hr;

	if (currentState.state != PlayerState::Closing)
	{
		hr = ProcessMediaEvent(event);

		if (hr != S_FALSE)
		{
			hr = session->BeginGetEvent(this, NULL);
		}

		if (FAILED(hr)) return hr;
	}

	return hr;
}

HRESULT Player::QueryInterface(REFIID riid, void** ppv)
{
	HRESULT hr = S_OK;

	if (ppv == NULL)
	{
		return E_POINTER;
	}

	if (riid == __uuidof(IMFAsyncCallback))
	{
		*ppv = static_cast<IMFAsyncCallback*>(this);
	}
	else if (riid == __uuidof(IUnknown))
	{
		*ppv = static_cast<IUnknown*>(this);
	}
	else
	{
		*ppv = NULL;
		hr = E_NOINTERFACE;
	}

	if (SUCCEEDED(hr))
		AddRef();

	return hr;
}

ULONG Player::AddRef()
{
	return InterlockedIncrement(&refCount);
}

ULONG Player::Release()
{
	ULONG count = InterlockedDecrement(&refCount);
	if (count == 0)
	{
		delete this;
	}
	return count;
}

HRESULT Player::CreateSession()
{
	HRESULT hr = S_OK;

	CComCritSecLock<CComAutoCriticalSection> lock(critSec);

	hr = CloseSession();
	if (FAILED(hr)) return hr;

	if (currentState.state != PlayerState::Closed)
	{
		return E_UNEXPECTED;
	}

	hr = MFCreateMediaSession(NULL, &session);
	if (FAILED(hr)) return hr;
	if (session == NULL) return E_UNEXPECTED;

	currentState.state = PlayerState::Ready;

	hr = session->BeginGetEvent((IMFAsyncCallback*)this, NULL);
	return hr;
}

HRESULT Player::CloseSession()
{
	HRESULT hr = S_OK;
	DWORD dwWaitResult = 0;

	CComCritSecLock<CComAutoCriticalSection> lock(critSec);

	currentState.state = PlayerState::Closing;

	if (session != NULL)
	{
		currentState.state = PlayerState::Closing;

		hr = session->Close();
		if (SUCCEEDED(hr))
		{
			dwWaitResult = WaitForSingleObject(closeCompleteEvent, 5000);
			if (dwWaitResult == WAIT_TIMEOUT)
			{
				return E_UNEXPECTED;
			}
		}
	}

	if (session != NULL)
	{
		session->Shutdown();
	}

	session = NULL;
	currentState.state = PlayerState::Closed;
	return hr;
}

HRESULT Player::StartPlayback()
{
	HRESULT hr = S_OK;
	PROPVARIANT varStart;

	if (session == NULL) return E_UNEXPECTED;

	PropVariantInit(&varStart);

	varStart.vt = VT_EMPTY;

	hr = session->Start(&GUID_NULL, &varStart);

	PropVariantClear(&varStart);

	return hr;
}

HRESULT Player::ProcessMediaEvent(CComPtr<IMFMediaEvent>& mediaEvent)
{
	// Статус события
	HRESULT hrStatus = S_OK;
	HRESULT hr = S_OK;
	UINT32 TopoStatus = MF_TOPOSTATUS_INVALID;
	MediaEventType eventType;

	if (mediaEvent == NULL) return E_POINTER;

	hr = mediaEvent->GetType(&eventType);
	if (FAILED(hr)) return hr;

	hr = mediaEvent->GetStatus(&hrStatus);
	if (FAILED(hr)) return hr;
	if (FAILED(hrStatus)) return hrStatus;

	if (eventType == MESessionTopologyStatus)
	{
		// Получить статус код
		hr = mediaEvent->GetUINT32(MF_EVENT_TOPOLOGY_STATUS, (UINT32*)&TopoStatus);
		if (FAILED(hr)) return hr;

		if (TopoStatus == MF_TOPOSTATUS_READY)
		{
			currentState.state = PlayerState::Stopped;
			hr = OnTopologyReady();
		}
	}
	else if (eventType == MEEndOfPresentation)
	{
		currentState.state = PlayerState::Stopped;
	}
	else if (eventType == MESessionClosed)
	{
		SetEvent(closeCompleteEvent);
		hr = S_FALSE;
	}
	else if (eventType == MESessionCapabilitiesChanged)
	{
		sessionCaps = MFGetAttributeUINT32(mediaEvent, MF_EVENT_SESSIONCAPS, sessionCaps);
	}
	else if (eventType == MESessionStarted)
	{
		UpdatePendingCommands(PlayerState::Started);
	}
	else if (eventType == MESessionStopped)
	{
		UpdatePendingCommands(PlayerState::Stopped);
	}
	else if (eventType == MESessionPaused)
	{
		UpdatePendingCommands(PlayerState::Paused);
	}
	return hr;
}

HRESULT Player::OnTopologyReady()
{
	HRESULT hr = S_OK;
	CComPtr<IMFClock> _clock;

	hr = session->GetClock(&_clock);
	if (SUCCEEDED(hr))
	{
		hr = _clock->QueryInterface(IID_PPV_ARGS(&clock));
	}

	hr = MFGetService(session, MR_POLICY_VOLUME_SERVICE, IID_IMFSimpleAudioVolume, (void**)&simpleAudioVolume);

	hr = Play();
	return hr;
}

HRESULT Player::SetPositionInternal(const MFTIME& position)
{
	if (IsPending) 
	{
		return MF_E_INVALIDREQUEST;
	}

	if (session == NULL)
	{
		return MF_E_INVALIDREQUEST;
	}

	HRESULT hr = S_OK;

	PROPVARIANT varStart;
	varStart.vt = VT_I8;
	varStart.hVal.QuadPart = position;

	hr = session->Start(&GUID_NULL, &varStart);

	if (SUCCEEDED(hr))
	{
		// Store the pending state.
		currentState.state = PlayerState::Started;
		currentState.start = position;
		IsPending = CMD_PENDING_SEEK;
	}
	return hr;
}

HRESULT Player::UpdatePendingCommands(PlayerState state)
{
	HRESULT hr = S_OK;

	CComCritSecLock<CComAutoCriticalSection> lock(critSec);

	PROPVARIANT varStart;
	PropVariantInit(&varStart);

	if (IsPending && currentState.state == state)
	{
		IsPending = FALSE;

		// The current pending command has completed.

		// Now look for seek requests.
		switch (pendingRequest.state)
		{
		case PlayerState::Started:
			Play();
			break;

		case PlayerState::Paused:
			Pause();
			break;

		case PlayerState::Stopped:
			Stop();
			break;

		case PlayerState::Seek:
			SetPositionInternal(pendingRequest.start);
			break;
		}
		pendingRequest.state = PlayerState::Closed;
	}

	return hr;
}

HRESULT Player::GetVolume(float* volume)
{
	HRESULT hr = S_OK;

	if (simpleAudioVolume == NULL)
	{
		hr = E_FAIL;
	}
	else
	{
		hr = simpleAudioVolume->GetMasterVolume(volume);
	}
	return hr;
}

HRESULT Player::SetVolume(float volume)
{
	HRESULT hr = S_OK;

	if (simpleAudioVolume == NULL)
	{
		hr = E_FAIL;
	}
	else
	{
		hr = simpleAudioVolume->SetMasterVolume(volume);
	}
	return hr;
}
