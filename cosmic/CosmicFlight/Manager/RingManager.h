#pragma once
#include <list>
#include "../Object/Ring.h"

class RingManager {
public:
	RingManager();
	~RingManager();

	bool Initialize();
	void Update(Vector3 playerPos);
	void Render();
	void Add(std::unique_ptr<Ring> obj, Vector3 pos);
	void Clear();

	std::list<std::unique_ptr<Ring>>& GetList() { return ringList; } // リスト取得
	bool RingJudge(Vector3 pos);
	void ScatterRings(Vector3 center, int count, bool fromPlayer);// まき散らし
	//スコア
	void SetScoreManager(ScoreManager* manager) { scoreManager = manager; }

private:
	std::list<std::unique_ptr<Ring>> ringList;
	std::unique_ptr<FBXMesh> meshList;

	Vector3 ringPosition;
	Vector3 firstRingPosition = Vector3(1000.0f, 1000.0f, 1000.0f);
	const float RingDistance = 0.3f;

	Vector3 diff;
	float zStretchFactor;
	float effectiveDistance;

	float angle;
	float radius;
	float height;
	Vector3 targetPos;

	const float minRadius = 1.5f;
	ScoreManager* scoreManager = nullptr;
};