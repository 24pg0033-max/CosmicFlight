#pragma once
#include<memory>
#include "../UI/2DUI.h"

class Loading {
public:
	Loading();
	~Loading();
	bool Initialize();
	void Update();
	void Render();

	void ChecKPoint();

	bool GetLoadEnd() { return  loadEnd; }

private:
	std::unique_ptr<Render2D> loading;
	std::unique_ptr<_2DUI> loadingUI;
	std::unique_ptr<_2DUI> nLoadingUI;
	std::unique_ptr<_2DUI> circle;
	std::unique_ptr<_2DUI> circle2;
	std::unique_ptr<_2DUI> circle3;

	std::unique_ptr<Easing> eLoad;
	float eLoadRate = 0.0f;
	const float LoadSpeed = 0.01f;//イージング速度
	const float LoadAlphaMax = 1.0f;//背景透明度最大
	bool loadEnd;
	float Time = 0.0f;
	float length;
	float barLength;
	float barLengthMax;
	int checkPoint;
	float loadingRate;
	bool cFlag = true;
	int count = 0;
};