#pragma once
#include <memory>
#include "../DXShader.h"
#include "../global.h"
#include "../UI/UI.h"
#include "../UI/2DUI.h"
#include "ScoreManager.h"

class Player;

class UIManager {
public:
	UIManager();
	~UIManager();
	void Update();
	void Render();

	void SetScoreManager(ScoreManager* sm) { scoreManager = sm; }
	void SetPlayer(Player* p) { player = p;}

private:
	// PhaseUI
	std::unique_ptr<_2DUI> firstPhaseUI;
	std::unique_ptr<_2DUI> secondPhaseUI;
	std::unique_ptr<_2DUI> thirdPhaseUI;

	TextureSize phaseUISize = { 375,85 };

	const float phaseUIPosX = 450.0f;
	const float phaseUIPosY = 200.0f;
	const float afterPhaseUIPosX = 820.0f;
	const float afterPhaseUIPosY = 100.0f;
	float phaseUITimer = 0.0f;
	bool timerResetDone = false;

	ScoreManager::Phase prevPhase = ScoreManager::Phase::FIRST;

	std::unique_ptr<Shader> shader;
	std::unique_ptr<UI> gauge;
	Player* player;
	ScoreManager* scoreManager;
};