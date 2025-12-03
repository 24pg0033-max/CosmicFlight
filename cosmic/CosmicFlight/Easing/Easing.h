#pragma once
#include "../global.h"

class Easing{
public:
	enum STATE {
		LINEAR,
		EASEOUTQUAD,
		EASEINQUAD,
		EASEOUTCUBIC,		//　1-(1-x)^3    x:0.0f～1.0f
		EASEOUTQUINT,
		EASEINQUINT,
		EASEINCUBIC,
		EASEINSINE,
		EASEOUTEXPO,
	};

	Easing(Easing::STATE st = LINEAR, float speed = 0.01f, float start = 1.0f, bool firstIsOn = true);

	//セッター
	void SetIsOn(bool f) { isOn = f; }
	void SetChangeRate(float v) { changeRate = v; }
	void SetnowFrame(float v) { nowFrame = v; }

	//ゲッター
	bool GetIsMaxRate()const { return isMaxRate; }
	bool GetIsMinRate()const { return isMinRate; }

	float RateUpdate();			// 更新と値取得
	void Reset();
	void ChangeIsOn();			// trueとfalseをそれぞれ切り替える
	void ResetRateFlags() {
		isMaxRate = false;
		isMinRate = false;
	}
	void SetSpeed(float s);

private:
	void Linear();				//直線の更新処理
	void EaseOutQuad();         
	void EaseInQuad();         
	void EaseOutCubic();		
	void EaseOutQuint();
	void EaseInQuint();
	void EaseInCubic();
	void EaseInSine();
	void EaseOutExpo();


	float changeRate;			//変化率
	float changeSpeed;			//変化速度
	float changeRate_Max;		//最大変化率
	float changeRate_Min;		//最小変化率
	bool  isOn;					//変化する
	Easing::STATE easeState;		//変化の仕方
	float nowFrame;				//現在のフレーム数
	bool isMaxRate;				//変化が最大になったかどうか
	bool isMinRate;				//変化が最小になったかどうか
};

