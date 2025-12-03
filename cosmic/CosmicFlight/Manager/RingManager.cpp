#include "RingManager.h"
#include "../DXShader.h"
#include "../global.h"

RingManager::RingManager(){
	meshList = nullptr;
	angle = 0.0f;
	radius = 0.0f;
	height = 0.0f;
	zStretchFactor = 0.0f;
	effectiveDistance = 0.0f;
	ringList.clear();
}

RingManager::~RingManager(){
	Clear();
}

bool RingManager::Initialize(){
	meshList = std::make_unique<FBXMesh>();
	meshList->Create("Assets/obstacle/Ring/Ring.fbx");
	ringPosition = firstRingPosition;
	return false;
}

void RingManager::Update(Vector3 playerPos) {
	for (auto& obj : ringList) {
		if (!obj->isAttracted && !obj->isScattering) {
			diff = obj->GetPosition() - playerPos;

			// Z方向の距離をスケーリングして判定（Z方向に吸い寄せ範囲を拡張）
			zStretchFactor = 0.1f;
			effectiveDistance = sqrt(diff.x * diff.x + diff.y * diff.y + (diff.z * zStretchFactor) * (diff.z * zStretchFactor));

			if (effectiveDistance < 3.0f) {
				obj->isAttracted = true;
				obj->attractTarget = playerPos;
			}
		}

		obj->Update();
	}

	// 非アクティブリングの削除
	for (auto itr = ringList.begin(); itr != ringList.end();) {
		if (!(*itr)->GetIsActive()) {
			itr = ringList.erase(itr);
			continue;
		}
		++itr;
	}
}


void RingManager::Render(){
	for (auto& obj : ringList) {
		Shader::SetTransform(obj->GetTransForm());
		if (scoreManager) {
			obj->SetScoreManager(scoreManager);
		}
		obj->Render();
	}
}

void RingManager::Add(std::unique_ptr<Ring> obj, Vector3 pos) {
	obj->Initialize(meshList.get());
	obj->SetPosition(pos);
	ringList.push_back(std::move(obj));
}

void RingManager::Clear(){
	ringList.clear();
}

bool RingManager::RingJudge(Vector3 pos) {
	// 当たり判定
	for (auto& obj : ringList) {
		ringPosition = obj->GetPosition();
		Vector3 dist = ringPosition - pos;
		if (dist.Length() < RingDistance) {
			return true;
		}
	}
	return false;
}

void RingManager::ScatterRings(Vector3 center, int count, bool fromPlayer) {
	// リング拡散
	for (int i = 0; i < count; ++i) {
		auto ring = std::make_unique<Ring>();
		ring->Initialize(meshList.get());
		ring->SetFromPlayerDrop(fromPlayer);

		// ランダム方向と距離
		angle = (rand() % 360) * PI / 180.0f;
		radius = minRadius + (rand() % 100) * 0.01f; // 1.5〜2.5
		height = ((rand() % 100) - 50) * 0.02f;      // 上下ずらし

		Vector3 offset(cosf(angle) * radius, height, sinf(angle) * radius);
		targetPos = center + offset;

		ring->startPos = center;
		ring->targetPos = targetPos;
		ring->scatterTimer = 0.0f;
		ring->isScattering = true;

		ringList.push_back(std::move(ring));
	}
}

