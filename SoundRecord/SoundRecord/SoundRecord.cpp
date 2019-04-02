#include "SoundRecord.h"

BOOL SoundRecord::InitWaveIn()
{
	INT nWaveInNum = waveInGetNumDevs();
	if (!nWaveInNum)
	{
		MessageBox(0, "无法获取声音设备", "Tips", 0);
		return FALSE;
	}

	MMRESULT mmResult;
	UINT dwThreadID = 0;

	m_hSoundCallBack = (HANDLE)_beginthreadex(0, 0, SoundThread, this, 
		CREATE_SUSPENDED, &dwThreadID);
	mmResult = waveInOpen(&m_hWaveIn, (WORD)WAVE_MAPPER,&(m_stGSM610.wfx), 
		(LONG)dwThreadID, (LONG)0, CALLBACK_THREAD);
	
	if (MMSYSERR_NOERROR != mmResult)
	{
		return FALSE;
	}

	for (int i = 0; i < 2; i++)
	{
		m_lpWaveInHdr[i]->lpData = (LPSTR)m_lpInSoundBuffer[i];
		m_lpWaveInHdr[i]->dwBufferLength = m_nSoundBufferSize;
		m_lpWaveInHdr[i]->dwFlags = 0;
		m_lpWaveInHdr[i]->dwLoops = 0;
		waveInPrepareHeader(m_hWaveIn, m_lpWaveInHdr[i], sizeof(WAVEHDR));
	}

	waveInAddBuffer(m_hWaveIn, m_lpWaveInHdr[m_nInSoundIndex], sizeof(WAVEHDR));
	ResumeThread(m_hSoundCallBack);
	waveInStart(m_hWaveIn);
	m_bWaveIn = TRUE;
	return m_bWaveIn;
}

BOOL SoundRecord::InitWaveOut()
{
	if (waveOutGetNumDevs() == NULL)
	{
		MessageBox(0, "播放声音设置获取失败", "Tips", 0);
		return FALSE;
	}

	for (int i=0;i<2;i++)
	{
		memset(m_lpOutSoundBuffer[i], 0, sizeof(BYTE)*m_nSoundBufferSize);
	}

	MMRESULT mmResult = NULL;
	mmResult = waveOutOpen(&m_hWaveOut, (WORD)WAVE_MAPPER, &(m_stGSM610.wfx), NULL, NULL, CALLBACK_NULL);
	if (mmResult != MMSYSERR_NOERROR)
	{
		return FALSE;
	}

	for (int i=0;i<2;i++)
	{
		m_lpWaveOutHdr[i]->lpData = (LPSTR)m_lpOutSoundBuffer[i];
		m_lpWaveOutHdr[i]->dwBufferLength = m_nSoundBufferSize;
		m_lpWaveOutHdr[i]->dwFlags = 0;
		m_lpWaveOutHdr[i]->dwLoops = 0;
		waveOutPrepareHeader(m_hWaveOut, m_lpWaveOutHdr[i], sizeof(WAVEHDR));
	}
	m_bWaveOut = TRUE;
	return m_bWaveOut;
}

UINT CALLBACK SoundRecord::SoundThread(LPVOID lpData)
{
	SoundRecord* pThis = (SoundRecord*)lpData;
	MSG stMsg = { 0 };
	while (GetMessage(&stMsg,0,0,0))
	{
		if (stMsg.message== MM_WIM_DATA)
		{
			//通知数据到来
			SetEvent(pThis->m_hEventWaveIn);
			//等待数据获取声音
			WaitForSingleObject(pThis->m_hStart, INFINITE);

			pThis->m_nInSoundIndex = 1 - pThis->m_nInSoundIndex;

			MMRESULT mmResult = waveInAddBuffer(pThis->m_hWaveIn,
				pThis->m_lpWaveInHdr[pThis->m_nInSoundIndex], sizeof(WAVEHDR));
			if (mmResult != MMSYSERR_NOERROR)
			{
				break;
			}

		}

		if (stMsg.message == MM_MIM_CLOSE)
		{
			break;
		}

		TranslateMessage(&stMsg);
		DispatchMessage(&stMsg);
	}
	return 0;
}

SoundRecord::SoundRecord()
{
	//自动重置事件
	m_hEventWaveIn = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hStart = CreateEvent(NULL, FALSE, FALSE, NULL);

	m_hSoundCallBack = NULL;

	m_nInSoundIndex = NULL;
	m_nOutSoundIndex = NULL;
	m_nSoundBufferSize = 1000;

	m_bWaveIn = FALSE;
	m_bWaveOut = FALSE;

	for (int i = 0; i < 2; i++)
	{
		m_lpInSoundBuffer[i] = new BYTE[m_nSoundBufferSize];
		m_lpOutSoundBuffer[i] = new BYTE[m_nSoundBufferSize];

		m_lpWaveInHdr[i] = new WAVEHDR;
		m_lpWaveOutHdr[i] = new WAVEHDR;
	}

	memset(&m_stGSM610, 0, sizeof(GSM610WAVEFORMAT));

	m_stGSM610.wfx.wFormatTag = WAVE_FORMAT_GSM610;
	m_stGSM610.wfx.nChannels = 1;
	m_stGSM610.wfx.nSamplesPerSec = 8000;
	m_stGSM610.wfx.nAvgBytesPerSec = 1625;
	m_stGSM610.wfx.nBlockAlign = 65;
	m_stGSM610.wfx.wBitsPerSample = 0;
	m_stGSM610.wfx.cbSize = 2;
	m_stGSM610.wSamplesPerBlock = 320;
}


SoundRecord::~SoundRecord()
{
	if (m_bWaveIn)
	{
		//停止获取和重置
		waveInStop(m_hWaveIn);
		waveInReset(m_hWaveIn);

		for (int i = 0; i < 2; i++)
		{
			waveInUnprepareHeader(m_hWaveIn, m_lpWaveInHdr[i], sizeof(WAVEHDR));
		}
		waveInClose(m_hWaveIn);

		//关闭线程
		TerminateThread(m_hSoundCallBack, -1);
	}
	if (m_bWaveOut)
	{
		waveOutReset(m_hWaveOut);
		for (int i = 0; i < 2; i++)
		{
			waveOutUnprepareHeader(m_hWaveOut, m_lpWaveOutHdr[i], sizeof(WAVEHDR));
		}
		waveOutClose(m_hWaveOut);
	}

	for (int i=0;i<2;i++)
	{
		delete[] m_lpInSoundBuffer[i];
		delete[] m_lpOutSoundBuffer[i];

		delete m_lpWaveInHdr[i];
		delete m_lpWaveOutHdr[i];
	}

	CloseHandle(m_hEventWaveIn);
	CloseHandle(m_hStart);
	CloseHandle(m_hSoundCallBack);
}

LPBYTE SoundRecord::GetSoundBuffer(LPDWORD pdwSized)
{
	if (InitWaveIn() == FALSE && m_bWaveIn == FALSE)
	{
		return nullptr;
	}

	if (pdwSized == nullptr)
	{
		return nullptr;
	}

	SetEvent(m_hStart);
	WaitForSingleObject(m_hEventWaveIn, INFINITE);
	*pdwSized = m_nSoundBufferSize;
	return m_lpInSoundBuffer[m_nInSoundIndex];
}

BOOL SoundRecord::PlaySoundBuffer(CONST LPBYTE pSoundBuffer, DWORD dwSize)
{
	if (InitWaveOut() == FALSE && m_bWaveOut == FALSE)
	{
		return FALSE;
	}

	if (pSoundBuffer == nullptr || dwSize == NULL)
	{
		return FALSE;
	}

	for (int i = 0; i < (INT)dwSize; i += m_nSoundBufferSize)
	{
		memcpy(m_lpOutSoundBuffer[m_nOutSoundIndex], pSoundBuffer, m_nSoundBufferSize);
		waveOutWrite(m_hWaveOut, m_lpWaveOutHdr[m_nOutSoundIndex], sizeof(WAVEHDR));
		m_nOutSoundIndex = 1 - m_nOutSoundIndex;
	}
	return TRUE;
}
