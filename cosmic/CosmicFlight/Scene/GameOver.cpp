#include "GameOver.h"
#include "GameScene.h"
#include "TitleScene.h"
#include "../Object/Player.h"
#include "../Input/Input.h"
#include "../Game/Game.h"
#include "../AudioSystem/SoundManager.h"

void GameOver::Initialize() {
	hGameOver = std::make_unique<_2DUI>();
	hGameOver->Initialize("Assets/scene/gameoverback.png", SCREEN_WIDTH, SCREEN_HEIGHT);

	GameOverLogo = std::make_unique<_2DUI>();
	GameOverLogo->Initialize("Assets/scene/GameOver.png", gameOverW, gameOverH, Vector3(360,150,0));

	start = std::make_unique<_2DUI>();
	start->Initialize("Assets/UI/restart.png", startSizeX, startSizeY, Vector3(startBottomX, startBottomY, NULL));

	end = std::make_unique<_2DUI>();
	end->Initialize("Assets/UI/end.png", endSizeX, endSizeY, Vector3(endBottomX, endBottomY, NULL), 0.8f, true);

	selectEffect = std::make_unique<_2DUI>();
	selectEffect->Initialize("Assets/Effect/title_effect.png", selectW, selectH, Vector3(selectX, selectY, NULL));

	uSquare = std::make_unique<_2DUI>();
	uSquare->Initialize("Assets/Effect/square.png", selectW, selectH, Vector3(squareX, squareY, NULL), sSize);

	bSquare = std::make_unique<_2DUI>();
	bSquare->Initialize("Assets/Effect/square.png", selectW, selectH, Vector3(squareX2, squareY2, NULL), sSize);

}

void GameOver::Update() {
	//選択ボタン更新
	selectEffect->SetChangingSize(true);

	selectEffect->AlphaChange(1.0f);

	if (Input::Key_Pad_Push('S', Input::BUTTON::DOWN) && PlayFlag == true) {
		selectEffect->SetPosition(Vector3(selectX2, selectY2, NULL));
		SoundManager::SoundPlay(SoundState::Select_Down_SE);
		PlayFlag = false;
	}
	else if (Input::Key_Pad_Push('W', Input::BUTTON::UP) && PlayFlag == false) {
		selectEffect->SetPosition(Vector3(selectX, selectY, NULL));
		SoundManager::SoundPlay(SoundState::Select_Up_SE);
		PlayFlag = true;
	}

	if (Input::Key_Pad_Push(VK_RETURN, Input::BUTTON::A) && PlayFlag == true) {
		GetGameScene()->JumpScene(NEW GameScene);
		SoundManager::SoundPlay(SoundState::Enter_SE);
		SoundManager::BGMPlay(InGame_BGM);
	}
	else if (Input::Key_Pad_Push(VK_RETURN, Input::BUTTON::A) && PlayFlag == false) {
		GetGameScene()->JumpScene(NEW TitleScene);
		SoundManager::SoundPlay(SoundState::Enter_SE);
		SoundManager::BGMPlay(Title_BGM);
	}

}

void GameOver::Render() {
	hGameOver->Render();
	GameOverLogo->Render();
	start->Render();
	end->Render();
	selectEffect->Render();
	uSquare->Render();
	bSquare->Render();
}
