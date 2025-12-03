#pragma once
#include <memory>
#include "2DUI.h"

class ScoreManager;

class UI {
public:
	UI();
	~UI();
	void Update();
	void Render();

	void SetHPRate(float rate);
private:
	// HPUI
	std::unique_ptr<_2DUI> hHPBar;
	std::unique_ptr<_2DUI> hHPGauge;
	const float hpUIPosX = 50.0f;
	const float hpUIPosY = 40.0f;
	const float hGazeSize = 0.5f;
	const float hHPGaugeWidth = 960.0f * hGazeSize;
	const float hHPGaugeHeight = 64.0f * hGazeSize;
	float HPRate;            //åªç›ÇÃÉQÅ[ÉWÇÃäÑçá

	TextureSize sHPBar = { 480,32 };
	TextureSize sHPGauge = { 960,64 };
};