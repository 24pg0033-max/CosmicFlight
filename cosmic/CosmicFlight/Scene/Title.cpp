#include "Title.h"
#include "GameScene.h"
#include "../DXShader.h"
#include "../Input/Input.h"
#include "../global.h"
#include "../Game/Game.h"
#include "../Camera/Camera.h"
#include "../Effect/PostEffect.h"
#include "../Object/TitlePlayer.h"
#include "../Easing/Easing.h"
#include "../UI/2DUI.h"
#include "../AudioSystem/SoundManager.h"


Title::Title() {
}

Title::~Title(){
}

bool Title::Initialize(){
	camera = std::make_unique<Camera>();
	camera->Initialize();

	titleBG = std::make_unique<Render2D>();
	titleBG->Initialize("Assets/stage/background/titleBG.png");

	tPlayer = std::make_unique<TitlePlayer>();
	tPlayer->Initialize();

	logo = std::make_unique<_2DUI>();
	logo->Initialize("Assets/UI/title_logo.png", logoSizeX, logoSizeY, Vector3(logoX, logoY, 0.0f));

	start = std::make_unique<_2DUI>();
	start->Initialize("Assets/UI/start.png", startSizeX, startSizeY, Vector3(startBottomX, startBottomY, 0.0f));

	end = std::make_unique<_2DUI>();
	end->Initialize("Assets/UI/end.png", endSizeX, endSizeY, Vector3(endBottomX, endBottomY, 0.0f), 0.8f, true);

	selectEffect = std::make_unique<_2DUI>();
	selectEffect->Initialize("Assets/Effect/title_effect.png", selectW, selectH, Vector3(selectX, selectY, 0.0f));

	uSquare = std::make_unique<_2DUI>();
	uSquare->Initialize("Assets/Effect/square.png", selectW, selectH, Vector3(squareX, squareY, 0.0f), sSize);

	bSquare = std::make_unique<_2DUI>();
	bSquare->Initialize("Assets/Effect/square.png", selectW, selectH, Vector3(squareX2, squareY2, 0.0f), sSize);

	// 基本シェーダ読み込み
	shader.Create(L"Assets/shader/3D.fx", "VSMain", "PSMain");
	// 行列パラメータをSlot0に設定
	DxSystem::DeviceContext->VSSetConstantBuffers(0, 1, &Shader::CBBaseMatrix);
	DxSystem::DeviceContext->PSSetConstantBuffers(0, 1, &Shader::CBBaseMatrix);
	DxSystem::DeviceContext->GSSetConstantBuffers(0, 1, &Shader::CBBaseMatrix);

	return false;
}

void Title::Update() {
	tPlayer->Update();

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
	}
	else if(Input::Key_Pad_Push(VK_RETURN, Input::BUTTON::A) && playFlag == false){
		SoundManager::SoundPlay(SoundState::Enter_SE);
		exit(NULL);
	}
}

void Title::Render(){
	titleBG->Render();
	shader.Activate();
	Shader::SetTransform(tPlayer->GetTransForm());
	tPlayer->Render();
	logo->Render();
	start->Render();
	end->Render();
	uSquare->Render();
	bSquare->Render();
	selectEffect->Render();
}