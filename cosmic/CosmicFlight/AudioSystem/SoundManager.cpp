#include "SoundManager.h"
#include "../Memory/Memory.h"
SoundManager::SOUNDPARAM* SoundManager::sounds;
SoundManager::SOUNDDATA SoundManager::soundData[SoundState::Max_STATE];

//初期化
void SoundManager::Initialize(){
	//音声パラメータ初期化
	sounds = NEW SOUNDPARAM[soundMax];
	for (int i = 0; i < soundMax; i++) {
		sounds[i].isAct = false;
		sounds[i].sec = 0;
		sounds[i].isLoop = false;
	}

	//音声データ読み込み
	soundData[SoundState::Select_Up_SE]			= CreateSound("Assets/sound/select_up.wav",1.0f);
	soundData[SoundState::Select_Down_SE]		= CreateSound("Assets/sound/select_down.wav", 1.0f);
	soundData[SoundState::Enter_SE]				= CreateSound("Assets/sound/enter.wav", 3.0f,0.5f);
	soundData[SoundState::Player_Attack_SE]		= CreateSound("Assets/sound/attack.wav", 1.5f);
	soundData[SoundState::Player_Damage_SE]		= CreateSound("Assets/sound/damage.wav", 8.0f);
	soundData[SoundState::Player_RingGet_SE]	= CreateSound("Assets/sound/ring.wav", 4.0f);
	soundData[SoundState::Enemy_Hit_SE]			= CreateSound("Assets/sound/hit.wav", 0.3f);

	soundData[SoundState::Title_BGM]			= CreateSound("Assets/sound/Title.wav", 0.3f,0.5f);
	soundData[SoundState::InGame_BGM]			= CreateSound("Assets/sound/InGame.wav", 0.5f,0.1f);
	soundData[SoundState::GameOver_BGM]			= CreateSound("Assets/sound/GameOver.wav", 0.3f);
	soundData[SoundState::Clear_BGM]			= CreateSound("Assets/sound/Clear.wav", 2.0f);
}

void SoundManager::Finalize() {
	if (sounds) {
		delete[] sounds;
		sounds = nullptr;
	}
	SoundSystem::FinalizeSound();
}

//更新
void SoundManager::Update(){
	//音声パラメータ更新
	for (int i = 0; i < soundMax; i++) {
		if (!sounds[i].isAct)continue;//使われているかどうか
		sounds[i].sec += DeltaTime;
		if (sounds[i].isLoop)continue;//ループしているものかどうか
		//設定している音声の時間を超えたら徐々に音量を下げる
		if (sounds[i].sec > sounds[i].data.maxSec) {
			sounds[i].nowVolume = sounds[i].data.volume - (sounds[i].sec - sounds[i].data.maxSec) * volumeDownSpeed;
			if (sounds[i].nowVolume < 0.0f) {//音量が０になったらfalse
				sounds[i].nowVolume = 0;
				sounds[i].isAct = false;
				soundData[sounds[i].state].nowPlay = false;
			}
		}
		//音量を変更
		SoundSystem::SetVolumeSound(sounds[i].data.soundNumber, sounds[i].nowVolume);
	}
}

//音声再生
void SoundManager::SoundPlay(SoundState se,bool loop){
	//空いているところを探す
	for (int i = 0; i < soundMax; i++) {
		if (sounds[i].isAct) {
			continue;
		}
		sounds[i].data = soundData[se];
		sounds[i].isAct = true;
		sounds[i].sec = 0;
		sounds[i].state = se;
		soundData[se].nowPlay = true;
		sounds[i].isLoop = loop;
		sounds[i].nowVolume = sounds[i].data.volume;
		//音声を流す
		SoundSystem::SoundPlay(sounds[i].data.soundNumber, loop, sounds[i].data.volume);
		return;
	}
	
}

//指定の音声停止
void SoundManager::SoundStop(SoundState se){
	SoundSystem::SoundStop(se);
}

//BGM再生
void SoundManager::BGMPlay(SoundState bgm){
	//SoundStateがBGMじゃなかったらreturn
	if (Title_BGM > bgm||bgm == Max_STATE)return;
	//既に流れていたらreturn
	if (soundData[bgm].nowPlay)return;

	//BGMの始めから最後まで回す
	for (int i = Title_BGM; i < Max_STATE; i++) {
		if (bgm == i) {//対象のＩＤなら流す
			soundData[i].nowPlay = true;
			SoundSystem::SoundPlay(soundData[i].soundNumber, true,soundData[i].volume);
		}
		else {//対象のＩＤじゃないなら止める
			soundData[i].nowPlay = false;
			SoundSystem::SoundStop(soundData[i].soundNumber);
		}
	}
}

//音声の周波数を変える
void SoundManager::ChangeFrequency(SoundState se, float ratio)
{
	SoundSystem::SetFrequencySound(soundData[se].soundNumber, ratio);
}

//音声読み込み
SoundManager::SOUNDDATA SoundManager::CreateSound(const char* filename, float MaxSec, float volume)
{
	SOUNDDATA stmp;
	stmp.soundNumber = SoundSystem::LoadWave(filename);
	stmp.maxSec = MaxSec;
	stmp.volume = volume;
	stmp.nowPlay = false;
	return stmp;
}
