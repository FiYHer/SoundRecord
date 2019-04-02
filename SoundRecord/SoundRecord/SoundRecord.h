#pragma once

#include <windows.h>
#include <mmsystem.h>
#include <process.h>
#include <mmreg.h>
#pragma comment(lib,"winmm.lib")

class SoundRecord
{
protected:
	//声音数据大小
	INT m_nSoundBufferSize;

	//读取声音和播放声音的缓冲区,保持声音的连续
	LPBYTE m_lpInSoundBuffer[2];
	LPBYTE m_lpOutSoundBuffer[2];

	//声音句柄
	HWAVEIN m_hWaveIn;
	HWAVEOUT m_hWaveOut;

	//读取声音的播放声音的索引
	INT m_nInSoundIndex;
	INT m_nOutSoundIndex;

	//声音获取事件
	HANDLE m_hEventWaveIn;
	HANDLE m_hStart;

	//声音回调线程的句柄
	HANDLE m_hSoundCallBack;

	//识别波形音频缓冲区的报头
	LPWAVEHDR m_lpWaveInHdr[2];
	LPWAVEHDR m_lpWaveOutHdr[2];

	//是否使用
	BOOL m_bWaveIn;
	BOOL m_bWaveOut;

	//采样率结构体
	GSM610WAVEFORMAT m_stGSM610;

private:
	/*
	初始化声音获取
	*/
	BOOL InitWaveIn();

	/*
	初始化声音播放
	*/
	BOOL InitWaveOut();

	/*
	声音处理回调函数
	*/
	static UINT CALLBACK SoundThread(LPVOID);
public:
	SoundRecord();
	virtual ~SoundRecord();
public:
	/*
	获取声音数据
	*/
	LPBYTE GetSoundBuffer(LPDWORD);

	/*
	开始播放声音
	*/
	BOOL PlaySoundBuffer(CONST LPBYTE, DWORD);

};

