#include "MeteorManager.h"
#include "../DXShader.h"
#include "ScoreManager.h"
#include "../global.h"

MeteorManager::MeteorManager(){
	selectedMesh = nullptr;
	meteorList.clear();
}

MeteorManager::~MeteorManager(){
	Clear();
}

bool MeteorManager::Initialize(){
	mesh1 = std::make_unique<FBXMesh>();
	mesh1->Create("Assets/obstacle/Meteor/Meteor_1/StoneStandard_1.fbx");

	mesh2 = std::make_unique<FBXMesh>();
	mesh2->Create("Assets/obstacle/Meteor/Meteor_2/StoneStandard_2.fbx");

	meteorPosition = firstMeteorPosition;
	return true;
}

void MeteorManager::Update() {
	// フェーズ完了なら更新停止
	if (scoreManager->GetCurrentPhase() == ScoreManager::Phase::COMPLETE) {
		return;
	}

	for (auto& obj : meteorList) {
		obj->Update();
	}

	// z座標が範囲外のメテオを削除
	for (auto itr = meteorList.begin(); itr != meteorList.end();) {
		if ((*itr)->GetPosition().z > meteorRange) {
			itr = meteorList.erase(itr);
		}
		else {
			++itr;
		}
	}
}

void MeteorManager::Render(){
	// ターゲット到達していたらメテオ停止
	if (scoreManager->GetCurrentPhase() == ScoreManager::Phase::COMPLETE) {
		return;
	}
	for (auto& obj : meteorList) {
		Shader::SetTransform(obj->GetTransForm());
		obj->Render();
	}
}

void MeteorManager::Add(std::unique_ptr<Meteor> obj, Vector3 pos){
	AddMeteorType(obj.get());
	AddMotionType(obj.get());
	obj->Initialize(selectedMesh);

	meteorScale = minScale + static_cast<float>(rand()) / RAND_MAX * (maxScale - minScale);
	obj->SetScale(Vector3(meteorScale, meteorScale, meteorScale));

	// ランダム回転速度
	Vector3 rotSpeed(
		rotMin + static_cast<float>(rand()) / RAND_MAX * (rotMax - rotMin),
		rotMin + static_cast<float>(rand()) / RAND_MAX * (rotMax - rotMin),
		rotMin + static_cast<float>(rand()) / RAND_MAX * (rotMax - rotMin)
	);
	obj->SetRotationSpeed(rotSpeed);

	// 回転角度ランダム
	Vector3 rotInit(
		static_cast<float>(rand()) / RAND_MAX * PI,
		static_cast<float>(rand()) / RAND_MAX * PI,
		static_cast<float>(rand()) / RAND_MAX * PI
	);
	obj->SetRotation(rotInit);
	obj->SetPosition(pos);
	meteorList.push_back(std::move(obj));
}

void MeteorManager::AddMeteorType(Meteor* obj) {
	selectedMesh = mesh1.get();

	if (scoreManager->GetCurrentPhase() == ScoreManager::Phase::SECOND ||
		scoreManager->GetCurrentPhase() == ScoreManager::Phase::FINAL) {

		if (rand() % 4 == 0) {
			selectedMesh = mesh2.get();
			obj->SetType(Meteor::MeteorType::Type2);		// リング減少隕石
		}
		else {
			selectedMesh = mesh1.get();
			obj->SetType(Meteor::MeteorType::Type1);        // 通常隕石
		}
	}
	else {
		obj->SetType(Meteor::MeteorType::Type1);
	}
}

void MeteorManager::AddMotionType(Meteor* obj) {
	// 最終フェーズMotion追加
	if (scoreManager->GetCurrentPhase() == ScoreManager::Phase::FINAL) {
		obj->motionType = Meteor::MotionType(rand() % 5 + 1);
	}
	else {
		obj->motionType = Meteor::MotionType::Straight;
	}
}

void MeteorManager::Clear() {
	meteorList.clear();
}

bool MeteorManager::GetActive() {
	for (auto& obj : meteorList) {
		return obj->GetFlag();
	}
	return false;
}

void MeteorManager::SetActive(bool active) {
	for (auto& obj : meteorList) {
		obj->SetFlag(active);
	}
}

Meteor::MeteorType MeteorManager::MeteorJudge(Vector3 pos) {
	for (auto& obj : meteorList) {
		if (!obj) continue;

		Vector3 mPosition = obj->GetPosition();
		Vector3 dist = mPosition - pos;
		Vector3 scaleFactor = obj->GetScale();

		switch (obj->GetType()) {
		case Meteor::MeteorType::Type2:
			radius = scaleFactor.x;
			if (dist.Length() < radius * meteorDistanceAdjust)
				return Meteor::MeteorType::Type2;
			break;

		case Meteor::MeteorType::Type1:
			radius = scaleFactor.x;
			if (dist.Length() < radius * meteorDistanceAdjust)
				return Meteor::MeteorType::Type1;
			break;

		default:
			break;
		}
	}
	return Meteor::MeteorType::None;
}