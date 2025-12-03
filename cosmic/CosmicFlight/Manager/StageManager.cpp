#include "StageManager.h"
#include "../global.h"
#include "../Input/Input.h"
#include <random>
#include "ScoreManager.h"
#include "../Camera/Camera.h"
#include "../Object/Object.h"
#include "MeteorManager.h"
#include "RingManager.h"
#include "../Object/Player.h"
#include "../Effect/particle.h"

StageManager* StageManager::current;// static宣言

StageManager::StageManager() {
	current = this;
	mesh = nullptr;

	//天球
	sky = std::make_unique<Object>();
	sky->Load("Assets/stage/skydome/skytest.fbx");
	skyUVAnim.Initialize();
	sky->SetScale(skySize);
	sky->SetPosition(skyPosition);

	//隕石
	meteor = std::make_unique<MeteorManager>();
	meteor->Initialize();

	//収集物
	ringManager = std::make_unique<RingManager>();
	ringManager->Initialize();

	scoreManager = std::make_unique<ScoreManager>();
	scoreManager->Initialize();

	meteor->SetScoreManager(scoreManager.get());
	ringManager->SetScoreManager(scoreManager.get());

	player = NEW Player();
	player->SetScoreManager(scoreManager.get());

	particle = std::make_unique<ParticleSystem>();
	particle->Initialize();
	particle->SetTexture("Assets/Effect/particle3.png", textureX, textureY);
	particle->SetAnimation(startAnim, endAnim);
	particle->SetSize(startSize, endSize);
	particle->SetBlendAdd();

	camera = NEW Camera();
	camera->Initialize();
}

StageManager::~StageManager() {
}

void StageManager::Update(){
	sky->Update();	//空更新
	skyUVAnim.Update();

	this->MeteorProcess();
	meteor->Update();
	ringManager->Update(player->GetPosition());
	scoreManager->Update();
	if (scoreManager->GetCurrentPhase() == ScoreManager::Phase::COMPLETE) {
		camera->SetState(Camera::STATE::GOAL);
	}

	ParticleUpdate();
}

void StageManager::Render(){
	//天球描画
	Shader::SetTransform(sky->GetTransForm());
	sky->Render();

	//リング
	ringManager->Render();

	//隕石
	meteor->Render();

	skyUVAnim.Render();

	if (scoreManager->GetCurrentPhase() != ScoreManager::Phase::COMPLETE) {
		particle->Render();
	}
}

void StageManager::SetTarget(Player* obj){
	player = obj;
}

void StageManager::MeteorProcess() {
	// 隕石ランダム位置生成
	--meteorTime;
	randXPos = GenerateRandomFloat(minRandPos, maxRandPos);
	randYPos = GenerateRandomFloat(minRandPos, maxRandPos);
	meteorPos = Vector3(randXPos + randXPosAdjust, randYPos, randZPos);
	if (scoreManager->GetCurrentPhase() == ScoreManager::Phase::FINAL) {
		--meteorTime;
	}
	if (meteorTime <= 0.0f) {
		meteor->Add(std::make_unique<Meteor>(), meteorPos);
		meteorTime = meteorFrequency;
	}
}

Meteor::MeteorType StageManager::MeteorFlag(Vector3 pos) {
	return meteor->MeteorJudge(pos);
}

bool StageManager::RingFlag(Vector3 pos) {
	if (ringManager->RingJudge(pos)) {
		return true;
	}
	return false;
}

float StageManager::GenerateRandomFloat(float min, float max) {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dis(min, max);
	return dis(gen);
}

void StageManager::ParticleUpdate() {
	// 隕石位置からパーティクル生成
	for (auto& obj : meteor->GetMeteorList()) {
		if (!obj) continue;
		meteorPos = obj->GetPosition(); // メテオの位置を取得

		exploVel = Vector3(
			(rand() % 200 - 100) * 0.01f,
			(rand() % 100) * 0.02f,
			(rand() % 200 - 100) * 0.01f
		);
		particle->Set(1.0f, meteorPos, exploVel); // メテオの位置にパーティクル発生
	}
		particle->Update();
}
