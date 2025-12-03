#include "InGame.h"
#include "../Render2D.h"
#include "../Input/Input.h"
#include "../Manager/StageManager.h"
#include "ResultScene.h"
#include <thread>
#include "../Object/Player.h"
#include "../Camera/Camera.h"
#include "Loading.h"
#include "../Manager/RingManager.h"
#include "../Manager/UIManager.h"
#include "../Object/EnemyManager.h"
#include "../AudioSystem/SoundManager.h"
#include "../Effect/PostEffect.h"
#include "../Object/Planet.h"
#include "../Game/Game.h"
#include "../RenderState.h"

InGame* InGame::Current;//staticの宣言
bool InGame::IsLoad;

InGame::InGame() {
}

InGame::~InGame() {
}

//ロード中処理
void InGame::LoadThread(){
	camera = NEW Camera();
	camera->Initialize();
	camera->SetState(Camera::STATE::START);

	postEffect = std::make_unique<PostEffect>();
	postEffect->Initialize();

	loading->ChecKPoint();  // ロードチェックポイント　１つ目

	stage = std::make_unique<StageManager>();
	stage->SetCamera(camera);

	loading->ChecKPoint();  // ロードチェックポイント　2つ目

	//プレイヤ
	astro = std::make_unique<Player>();
	astro->SetCamera(camera);
	astro->SetRingManager(stage->GetRingManager());
	astro->SetScoreManager(stage->GetScoreManager());

	loading->ChecKPoint();  // ロードチェックポイント　3つ目
	//UI
	UIMan = std::make_unique<UIManager>();
	UIMan->SetPlayer(astro.get());
	UIMan->SetScoreManager(stage->GetScoreManager());
	
	loading->ChecKPoint();  // ロードチェックポイント　4つ目
	
	// 基本シェーダ読み込み
	shader3D = std::make_unique<Shader>();
	bool result = shader3D->Create(L"Assets/shader/3D.fx", "VSMain", "PSMain");
	// 行列パラメータをSlot0に設定
	DxSystem::DeviceContext->VSSetConstantBuffers(0, 1, &Shader::CBBaseMatrix);
	DxSystem::DeviceContext->PSSetConstantBuffers(0, 1, &Shader::CBBaseMatrix);
	DxSystem::DeviceContext->GSSetConstantBuffers(0, 1, &Shader::CBBaseMatrix);
	assert(result);

	loading->ChecKPoint();  // ロードチェックポイント　5つ目

	ghost = std::make_unique<EnemyManager>();
	ghost->SetRingManager(stage->GetRingManager());
	astro->SetEnemyManager(ghost.get());

	planet = std::make_unique<Planet>();
	planet->SetCamera(camera);

	//読み込み終わり
	IsLoad = true;

	SoundManager::BGMPlay(InGame_BGM);
}


bool InGame::Initialize() {
	isLoad = false;
	loading = std::make_unique<Loading>();
	loading->Initialize();
	std::thread T([&] {
		LoadThread();
		});
	T.join();
	return true;
}

void InGame::Release(){
	postEffect->Release();
}

void InGame::Update() {
	if (!isLoad) {
		loading->Update();
		return;
	}

	ghost->Update(astro.get());

	StageManager::current->Update();
	StageManager::current->SetTarget(astro.get());

	astro->Update();

	// カメラ更新
	Vector3 pPosition = astro->GetPosition();
	if (astro->GetState() != Player::STATE::ATTACK) {
		camera->SetTarget(pPosition + Vector3(0.0f, 0.5f, 0.0f));
	}
	camera->Update();

	if (!postEffect->IsFadeCompleted()) {
		postEffect->UpdateFadeInOut(false, 2.0f, DeltaTime);
		postEffect->Update();
	}

	// カメラのGoal関数内で設定したシーン遷移条件を確認
	if (camera->goalElapsedTime >= 5.0f) { // 5秒経過後にシーン遷移
		postEffect->UpdateFadeInOut(true, 3.0f, DeltaTime);
		if (postEffect->IsFadeCompleted()) {
			GetGameScene()->JumpScene(NEW ResultScene);
			SoundManager::BGMPlay(Clear_BGM);
			return;
		}
	}
	
	UIMan->Update();
	planet->Update();

	if (astro->GetState() == Player::STATE::DEATH) {
		astro->Death();
	}
}

void InGame::Render() {
	if (!isLoad) {
		loading->Render();
		if (loading->GetLoadEnd() == true && Input::Key_Pad_Push(VK_RETURN, Input::BUTTON::A))
			isLoad = true;
		return;
	}
	postEffect->BloomActivate();
	{
		shader3D->Activate();
		StageManager::current->Render();
		ghost->Render(camera);
		Shader::SetTransform(astro->GetTransForm());
	}

	postEffect->Activate();
	{
		shader3D->Activate();
		StageManager::current->Render();
		Shader::SetTransform(planet->GetTransForm());
		planet->Render();
		ghost->Render(camera);
		Shader::SetTransform(astro->GetTransForm());
		astro->Render();
	}

	postEffect->Deactivate();
	postEffect->Render();


	UIMan->Render();
}