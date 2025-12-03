#include "Result.h"
#include "GameScene.h"
#include "TitleScene.h"
#include "../Object/Player.h"
#include "../Input/Input.h"
#include "../Game/Game.h"
#include "../AudioSystem/SoundManager.h"

Result::Result() {
}

Result::~Result() {
}

void Result::Initialize() {
	hGameOver = std::make_unique<_2DUI>();
	hGameOver->Initialize("Assets/scene/gameoverback.png", SCREEN_WIDTH, SCREEN_HEIGHT);

	gameOverLogo = std::make_unique<_2DUI>();
	gameOverLogo->Initialize("Assets/UI/GameClear.png", gameOverW, gameOverH, Vector3(360.0f, 150.0f, 0.0f));

	start = std::make_unique<_2DUI>();
	start->Initialize("Assets/UI/restart.png", startSizeX, startSizeY, Vector3(startBottomX, startBottomY, 0.0f));

	end = std::make_unique<_2DUI>();
	end->Initialize("Assets/UI/end.png", endSizeX, endSizeY, Vector3(endBottomX, endBottomY, 0.0f), 0.8f, true);

	selectEffect = std::make_unique<_2DUI>();
	selectEffect->Initialize("Assets/Effect/title_effect.png", selectW, selectH, Vector3(selectX, selectY, 0.0f));

	uSquare = std::make_unique<_2DUI>();
	uSquare->Initialize("Assets/Effect/square.png", selectW, selectH, Vector3(squareX, squareY, 0.0f), sSize);

	bSquare = std::make_unique<_2DUI>();
	bSquare->Initialize("Assets/Effect/square.png", selectW, selectH, Vector3(squareX2, squareY2, 0.0f), sSize);
}

void Result::Update() {
	//選択ボタン更新
	selectEffect->SetChangingSize(true);

	selectEffect->AlphaChange(1.0f);

	if (Input::Key_Pad_Push('S', Input::BUTTON::DOWN) && playFlag == true) {
		selectEffect->SetPosition(Vector3(selectX2, selectY2, NULL));
		SoundManager::SoundPlay(SoundState::Select_Down_SE);
		playFlag = false;
	}
	else if (Input::Key_Pad_Push('W', Input::BUTTON::UP) && playFlag == false) {
		selectEffect->SetPosition(Vector3(selectX, selectY, NULL));
		SoundManager::SoundPlay(SoundState::Select_Up_SE);
		playFlag = true;
	}

	if (Input::Key_Pad_Push(VK_RETURN, Input::BUTTON::A) && playFlag == true) {
		GetGameScene()->JumpScene(NEW GameScene);
		SoundManager::SoundPlay(SoundState::Enter_SE);
		SoundManager::BGMPlay(InGame_BGM);
	}
	else if (Input::Key_Pad_Push(VK_RETURN, Input::BUTTON::A) && playFlag == false) {
		GetGameScene()->JumpScene(NEW TitleScene);
		SoundManager::SoundPlay(SoundState::Enter_SE);
		SoundManager::BGMPlay(Title_BGM);
	}

}

void Result::Render() {
	hGameOver->Render();
	gameOverLogo->Render();
	start->Render();
	end->Render();
	selectEffect->Render();
	uSquare->Render();
	bSquare->Render();
}
