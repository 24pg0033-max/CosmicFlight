#include "UI.h"
#include "../Memory/Memory.h"

UI::UI(){
	hHPBar = std::make_unique<_2DUI>();
	hHPBar->Initialize("Assets/UI/HPFrame2.png", sHPBar.width, sHPBar.height, Vector3(hpUIPosX, hpUIPosY, NULL), 1.0f);
	hHPGauge = std::make_unique<_2DUI>();
	hHPGauge->Initialize("Assets/UI/HPBar.png", sHPGauge.width, sHPGauge.height, Vector3(hpUIPosX, hpUIPosY, NULL), 1.0f);

    HPRate = 1.0f;
}

UI::~UI(){
}

void UI::Update(){
	hHPGauge->SetUV(0.0f, 0.0f, HPRate, 1.0f);
	hHPGauge->SetSize(hHPGaugeWidth * HPRate, hHPGaugeHeight);
}

void UI::Render(){
	hHPBar->Render();
	hHPGauge->Render();
}

void UI::SetHPRate(float rate){
	if (rate < 0.0f)rate = 0.0f; //最小値に変更
	if (rate > 1.0f)rate = 1.0f; //最大値に変更
	HPRate = rate;
}
