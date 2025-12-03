#include "SoundSystem.h"
#include <string>

IXAudio2* SoundSystem:: xaudio;
IXAudio2MasteringVoice* SoundSystem::masteringVoice;
SoundInfo SoundSystem::se[SE_MAX];

bool SoundSystem::InitializeSound(){
#if USE_SOUND
	HRESULT hr;

	if (FAILED(hr = CoInitializeEx(NULL, COINIT_MULTITHREADED))) {
		return false;
	}
	for (int i = 0; i < SE_MAX; i++) {
		se[i].count = 0;
	}
	UINT32 flags = 0;
#ifdef _DEBUG
	flags |= XAUDIO2_DEBUG_ENGINE;
#endif
	if (FAILED(hr = XAudio2Create(&xaudio, flags))) {
#ifdef _DEBUG
		flags ^= XAUDIO2_DEBUG_ENGINE;
		if (FAILED(hr = XAudio2Create(&xaudio, flags)))
#endif
		{
			return false;
		}
	}

	if (FAILED(hr = xaudio->CreateMasteringVoice(&masteringVoice))) {
		return false;
	}
#endif
	return true;
}

void SoundSystem::FinalizeSound(){
#if USE_SOUND
	if (masteringVoice) {
		masteringVoice->DestroyVoice();
		masteringVoice = nullptr;
	}
	if (xaudio) {
		xaudio->Release();
		xaudio = nullptr;
	}

	CoUninitialize();
#endif
}

int SoundSystem::LoadWave(const char* filename){
#if USE_SOUND
	//既に読み込まれてるかチェック
	int id = FindSoundIDFromFilename(filename);
	if (id >= 0) {
		++se[id].count;
		return id;
	}

	//SEID発行
	id = GetEmptySoundID();
	if (id < 0) {
		return -1;
	}


	RIFFHEADER rh;
	FORMATCHUNK* pFormat = nullptr;	//フォーマットチャンク
	DATACHUNK* pData = nullptr;		//データチャンク
	
	//TODO: WAVEファイルの読み込み処理をここに書く
	FILE* fr = fopen(filename, "rb");
	if (fr == nullptr)return false;
	//RIFFヘッダ読み取り
	fread(&rh, sizeof(rh), 1, fr);

	unsigned long b = *reinterpret_cast<unsigned long*>(const_cast<char*>(RIFF_ID));
	if (rh.riffID != *reinterpret_cast<unsigned long*>(const_cast<char*>(RIFF_ID))) {
		//OutputDebugString("RIFFIDエラー\n");
		return false;
	}
	if (rh.waveID != *reinterpret_cast<unsigned long*>(const_cast<char*>(WAVE_ID))) {
		//OutputDebugString("RIFFIDエラー\n");
		return false;
	}

	while (!feof(fr) && !ferror(fr)) {
		unsigned long chunkID = 0;
		long chunkSize = 0;
		fread(&chunkID, sizeof(chunkID), 1, fr);
		fread(&chunkSize, sizeof(chunkSize), 1, fr);
		if (chunkSize <= 0)break;

		unsigned long a = *reinterpret_cast<unsigned long*>(const_cast<char*>(FORMAT_ID));

		if (chunkID == *reinterpret_cast<unsigned long*>(const_cast<char*>(FORMAT_ID))) {
			//FORMATチャンク発見したので読み取り
			pFormat = static_cast<FORMATCHUNK*>(malloc(chunkSize + 8));
			fread(reinterpret_cast<char*>(pFormat) + 8, chunkSize, 1, fr);
			char str[256];
			sprintf(str, "チャンネル数:%d,　サンプリング周波数:%dHZ ビットレート:%d\n",
				pFormat->wChannels, pFormat->dwSamplesPerSec, pFormat->wBitsPerSample);
			//OutputDebugString(str);

		}
		else if (chunkID == *reinterpret_cast<unsigned long*>(const_cast<char*>(DATA_ID))) {
			//DATAチャンク発見したので読み取り
			pData = static_cast<DATACHUNK*>(malloc(chunkSize + 8));
			//g_pBuffer = static_cast<BYTE*>(malloc(chunkSize));
			pData->chunkSize = chunkSize;
			fread(&pData->waveformData, chunkSize, 1, fr);
		}
		else {
			//(chunksize-8 = chunkID,chunkSizeを除いたチャンクのサイズ)
			fseek(fr, chunkSize, SEEK_CUR);
		}


	}

	//WAVのフォーマット 例）16ビット、44100Hz、ステレオなど
	WAVEFORMATEX wfex;
	wfex.wFormatTag = WAVE_FORMAT_PCM;					//PCMなので固定
	wfex.nChannels = pFormat->wChannels;				//チャンネル数(モノラル:1 ステレオ:2)
	wfex.nSamplesPerSec = pFormat->dwSamplesPerSec;		//サンプリングレート(22050Hz, 44100Hzなど)
	wfex.nAvgBytesPerSec = pFormat->dwAvgBytesPerSec;	//データ速度(秒間当たりのデータ量 channel*sample*bit/8)
	wfex.nBlockAlign = pFormat->wBlockAlign;			//ブロックサイズ(16bit ステレオなら 2*2=4)
	wfex.wBitsPerSample = pFormat->wBitsPerSample;		//サンプル当たりのビット数(8bit:8 16bit:16)
	wfex.cbSize = 0;									//PCMなので固定

	HRESULT hr;
	BYTE* pBuffer = static_cast<BYTE*>(malloc(pData->chunkSize));
	IXAudio2SourceVoice* sourceVoice = nullptr;
	memcpy(pBuffer, &pData->waveformData, pData->chunkSize);
	if (FAILED(hr = xaudio->CreateSourceVoice(&sourceVoice, &wfex))) {
		if (pFormat) free(pFormat);
		if (pData) free(pData);
		if (pBuffer) free(pBuffer);
		return false;
	}


	XAUDIO2_BUFFER audioBuffer = { 0 };
	audioBuffer.pAudioData = pBuffer;
	audioBuffer.Flags = XAUDIO2_END_OF_STREAM;
	audioBuffer.AudioBytes = pData->chunkSize;

	SoundInfo* pse = &se[id];
	pse->count = 1;
	strcpy_s(pse->filename, filename);
	pse->SourceVoice = sourceVoice;
	pse->audioBuffer = audioBuffer;
	pse->pBuffer = pBuffer;

	
	free(pFormat);
	free(pData);
	return id;
#else
return -1;
#endif
}

void SoundSystem::SoundPlay(int seid, bool isLoop, float volume){
#if USE_SOUND
	SoundInfo* pse = &se[seid];
	if (pse->count <= 0) return;

	pse->audioBuffer.LoopCount = isLoop ? XAUDIO2_LOOP_INFINITE : 0;

	pse->SourceVoice->Stop();
	pse->SourceVoice->FlushSourceBuffers();
	pse->SourceVoice->SubmitSourceBuffer(&pse->audioBuffer);
	pse->SourceVoice->SetVolume(volume);
	pse->SourceVoice->Start();
#endif
}

void SoundSystem::SoundStop(int seid){
#if USE_SOUND
	SoundInfo* pse = &se[seid];
	if (pse->count <= 0) return;

	pse->SourceVoice->Stop();
#endif
}

void SoundSystem::SetVolumeSound(int seid, float volume){
#if USE_SOUND
	SoundInfo* pse = &se[seid];
	if (pse->count <= 0) return;

	pse->SourceVoice->SetVolume(volume);
#endif
}

void SoundSystem::SetFrequencySound(int seid, float ratio){
#if USE_SOUND
	SoundInfo* pse = &se[seid];
	if (pse->count <= 0) return;

	pse->SourceVoice->SetFrequencyRatio(ratio);
#endif
}

bool SoundSystem::IsPlaySound(int seid){
#if USE_SOUND
	SoundInfo* pse = &se[seid];
	if (pse->count <= 0) return false;

	XAUDIO2_VOICE_STATE state;
	pse->SourceVoice->GetState(&state);
	return (state.BuffersQueued > 0);
#else
	return false;
#endif
}

void SoundSystem::ReleaseSound(int seid){
#if USE_SOUND
	SoundInfo* pse = &se[seid];
	if (pse->count <= 0) return;

	if (--pse->count <= 0) {
		pse->SourceVoice->DestroyVoice();
		pse->SourceVoice = nullptr;
		free(pse->pBuffer);
	}
#endif
}
int  SoundSystem::GetEmptySoundID() {
#if USE_SOUND
	for (int i = 0; i < SE_MAX; i++) {
		if (se[i].count <= 0) return i;
	}
#endif
	return -1;
}

#if USE_SOUND
int  SoundSystem::FindSoundIDFromFilename(const char* filename) {
	for (int i = 0; i < SE_MAX; i++) {
		if (se[i].count > 0 && strcmp(filename, se[i].filename) == 0) {
			return i;
		}
	}
	return -1;
}
#endif
