#pragma once

#include <windows.h>
#include <mmsystem.h>
#include <process.h>
#include <mmreg.h>
#pragma comment(lib,"winmm.lib")

class SoundRecord
{
protected:
	//�������ݴ�С
	INT m_nSoundBufferSize;

	//��ȡ�����Ͳ��������Ļ�����,��������������
	LPBYTE m_lpInSoundBuffer[2];
	LPBYTE m_lpOutSoundBuffer[2];

	//�������
	HWAVEIN m_hWaveIn;
	HWAVEOUT m_hWaveOut;

	//��ȡ�����Ĳ�������������
	INT m_nInSoundIndex;
	INT m_nOutSoundIndex;

	//������ȡ�¼�
	HANDLE m_hEventWaveIn;
	HANDLE m_hStart;

	//�����ص��̵߳ľ��
	HANDLE m_hSoundCallBack;

	//ʶ������Ƶ�������ı�ͷ
	LPWAVEHDR m_lpWaveInHdr[2];
	LPWAVEHDR m_lpWaveOutHdr[2];

	//�Ƿ�ʹ��
	BOOL m_bWaveIn;
	BOOL m_bWaveOut;

	//�����ʽṹ��
	GSM610WAVEFORMAT m_stGSM610;

private:
	/*
	��ʼ��������ȡ
	*/
	BOOL InitWaveIn();

	/*
	��ʼ����������
	*/
	BOOL InitWaveOut();

	/*
	��������ص�����
	*/
	static UINT CALLBACK SoundThread(LPVOID);
public:
	SoundRecord();
	virtual ~SoundRecord();
public:
	/*
	��ȡ��������
	*/
	LPBYTE GetSoundBuffer(LPDWORD);

	/*
	��ʼ��������
	*/
	BOOL PlaySoundBuffer(CONST LPBYTE, DWORD);

};

