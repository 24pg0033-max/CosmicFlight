#include "UIManager.h"
#include "../Memory/Memory.h"
#include "../Object/Player.h"

UIManager::UIManager(){
	player = nullptr;
	scoreManager = nullptr;

	gauge = std::make_unique<UI>();

	firstPhaseUI = std::make_unique<_2DUI>();
	firstPhaseUI->Initialize("Assets/UI/FirstPhase.png", phaseUISize.width, phaseUISize.height, Vector3(phaseUIPosX, phaseUIPosY, NULL));
	firstPhaseUI->SetChangingSize(true);
	firstPhaseUI->SetColor(1.0f, 1.0f, 0.3f);

	secondPhaseUI = std::make_unique<_2DUI>();
	secondPhaseUI->Initialize("Assets/UI/SecondPhase.png", phaseUISize.width, phaseUISize.height, Vector3(phaseUIPosX, phaseUIPosY, NULL));
	secondPhaseUI->SetChangingSize(true);
	secondPhaseUI->SetColor(1.0f, 1.0f, 0.3f);

	thirdPhaseUI = std::make_unique<_2DUI>();
	thirdPhaseUI->Initialize("Assets/UI/ThirdPhase.png", phaseUISize.width, phaseUISize.height, Vector3(phaseUIPosX, phaseUIPosY, NULL));
	thirdPhaseUI->SetChangingSize(true);
	thirdPhaseUI->SetColor(1.0f, 1.0f, 0.3f);
}

UIManager::~UIManager(){
}


void UIManager::Update() {
	phaseUITimer += DeltaTime;
	if (player) {
		gauge->SetHPRate(player->GetHPRate());
		gauge->Update();
	}

	if (scoreManager) {
		scoreManager->Update();

		if (scoreManager->GetCurrentPhase() != prevPhase) {
			phaseUITimer = 0.0f;
			timerResetDone = false;
			prevPhase = scoreManager->GetCurrentPhase();
		}

		if (phaseUITimer >= 3 && !timerResetDone)
		{
			switch (scoreManager->GetCurrentPhase()){
			case ScoreManager::Phase::FIRST:
				firstPhaseUI->StartMove(
					Vector3(phaseUIPosX, phaseUIPosY, NULL),
					Vector3(afterPhaseUIPosX, afterPhaseUIPosY, NULL),
					1.0f
				);
				break;

			case ScoreManager::Phase::SECOND:
				secondPhaseUI->StartMove(
					Vector3(phaseUIPosX, phaseUIPosY, NULL),
					Vector3(afterPhaseUIPosX, afterPhaseUIPosY, NULL),
					1.0f
				);
				break;

			case ScoreManager::Phase::FINAL:
				thirdPhaseUI->StartMove(
					Vector3(phaseUIPosX, phaseUIPosY, NULL),
					Vector3(afterPhaseUIPosX, afterPhaseUIPosY, NULL),
					1.0f
				);
				break;
			}
			timerResetDone = true;  // ˆê“x‚¾‚¯ˆÚ“®ŠJŽn‚³‚¹‚é
		}
		firstPhaseUI->Update();
		secondPhaseUI->Update();
		thirdPhaseUI->Update();
	}
}

void UIManager::Render() {
	gauge->Render();
	if (scoreManager) {
		scoreManager->Render();

		switch (scoreManager->GetCurrentPhase()) {
		case ScoreManager::Phase::FIRST:
			firstPhaseUI->Render();
			break;
		case ScoreManager::Phase::SECOND:
			secondPhaseUI->Render();
			break;
		case ScoreManager::Phase::FINAL:
			thirdPhaseUI->Render();
			break;
		}
	}
}
