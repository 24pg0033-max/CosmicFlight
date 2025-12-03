#pragma once
#include <stdio.h>
#include <math.h>
#include <xaudio2.h>

#define RIFF_ID		"RIFF"
#define WAVE_ID		"WAVE"
#define FORMAT_ID	"fmt "
#define DATA_ID		"data"
#define USE_SOUND	(1)
#define MAX_PATH	260
#define SE_MAX		1024

struct RIFFHEADER {
	unsigned long riffID;
	unsigned long dwRiffSize;
	unsigned long waveID;
};

struct FORMATCHUNK {
	unsigned long	chunkID;
	long			chunkSize;
	short			wFormatTag;
	unsigned short	wChannels;
	unsigned long	dwSamplesPerSec;
	unsigned long	dwAvgBytesPerSec;
	unsigned short	wBlockAlign;
	unsigned short	wBitsPerSample;

};

struct DATACHUNK {
	unsigned long	chunkID;
	long			chunkSize;
	unsigned char	waveformData;
};

struct SoundInfo {
	char					filename[MAX_PATH];
	int						count;
	IXAudio2SourceVoice*	SourceVoice;
	XAUDIO2_BUFFER			audioBuffer;
	BYTE*					pBuffer;
};

class SoundSystem
{
public:
	
	static bool InitializeSound();					//サウンド初期化
	static void FinalizeSound();					//サウンド終了
	static int	LoadWave(const char* filename);		//音声読み込み
	static void SoundPlay(int seid, bool isLoop = false, float volume = 1.0f);//サウンド再生
	static void SoundStop(int seid);				//サウンドストップ
	static void SetVolumeSound(int seid, float volume);//ボリューム変更
	static void SetFrequencySound(int seid, float ratio);//周波数変更
	static bool IsPlaySound(int seid);
	static void ReleaseSound(int seid);

private:
	
	static IXAudio2* xaudio;
	static IXAudio2MasteringVoice* masteringVoice;
	static SoundInfo se[SE_MAX];

	static int GetEmptySoundID();
	static int FindSoundIDFromFilename(const char* filename);

};

