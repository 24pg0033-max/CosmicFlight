#pragma once
#include "SoundSystem.h"
#include "../global.h"
#include "../singleton.h"

//音声データ種類
enum SoundState {
	//SE///////////////////
	Select_Up_SE,		//セレクト上
	Select_Down_SE,		//セレクト下
	Enter_SE,			//決定
	Player_Attack_SE,	//攻撃
	Player_Damage_SE,	//ダメージ
	Player_RingGet_SE,	//リング取得
	Enemy_Hit_SE,		//エネミーにヒット

	//VOICE////////////////

	//BGM//////////////////
	Title_BGM,				//Title
	InGame_BGM,				//インゲーム
	GameOver_BGM,			//ゲームオーバー
	Clear_BGM,				//クリアー
	Max_STATE,
};


class SoundManager :public Singleton<SoundManager>
{
	friend class Singleton<SoundManager>;
public:
	//音声データ
	struct SOUNDDATA {
		int soundNumber;
		float volume;
		float maxSec;
		bool nowPlay;//重複してはいけない音声用
	};
	//SEやVoiceパラメータ
	struct SOUNDPARAM {
		bool isAct;
		float sec;
		SOUNDDATA data;
		SoundState state;
		bool isLoop;
		float nowVolume;
	};
	static void Initialize();				//初期化
	static void Finalize();
	static void Update();					//更新
	static void SoundPlay(SoundState se,bool loop=false);//音声再生
	static void SoundStop(SoundState se);	//音声停止
	static void BGMPlay(SoundState bgm);	//BGM再生
	static void ChangeFrequency(SoundState se, float ratio);//周波数変更
	static SOUNDDATA CreateSound(const char* filename, float MaxSec, float volume=1.0f);//音声データ読み込み

private:
	SoundManager()=default;
	~SoundManager()=default;
	SoundManager(const SoundManager&) = delete;
	SoundManager& operator=(const SoundManager&) = delete;

	static const int soundMax = 100;					//同時に再生できるSEの数
	static const int volumeDownSpeed = 30;				//音声の再生時間が終わったら音量を減衰するスピード
	static SOUNDPARAM* sounds;							//再生する音声のデータ
	static SOUNDDATA soundData[SoundState::Max_STATE];	//読み込みした音声データ
};

