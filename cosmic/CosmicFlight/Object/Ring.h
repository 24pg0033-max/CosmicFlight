#pragma once
#include "Object.h"
#include "../global.h"
#include "../Manager/ScoreManager.h"
#include "Meteor.h"

class Ring :public Object {
public:
	void Initialize(FBXMesh* sourceMesh);
	void Update();
	void Render();

	float ClampMin(float value, float minVal) {
		return (value > minVal) ? value : minVal;
	}

	//スコア
	ScoreManager* scoreManager = nullptr;
	void SetScoreManager(ScoreManager* manager) { scoreManager = manager; }

	Vector3 startPos;						//初期位置
	Vector3 targetPos;						//目標地点

	//リング
	float scatterTimer = 0.0f;
	float scatterDuration = 0.2f;			// 拡散完了までの時間
	bool isScattering = false;

	bool waitScatter = false;				// 拡散後の一時停止中フラグ
	float waitTimer = 0.0f;					// 一時停止カウント用タイマー
	float waitDuration = 0.1f;				// 一時停止時間

	const int getScore = 100;

	bool isAttracted = false;
	Vector3 attractTarget;					// 吸い寄せ先
	float attractSpeed = 12.0f;				// 吸い寄せる速さ

	void SetFromPlayerDrop(bool val) { isPlayerDrop = val; }
	bool IsFromPlayerDrop() const { return isPlayerDrop; }

private:
	const Vector3 firstRingScale = Vector3(0.2f, 0.2f, 0.2f);
	const Vector3 firstRingRotation = Vector3(0.0f, PI / 2.0f, 0.0f);
	const float ringSize = 0.3f;			// リングの大きさ
	const int survivalTime = 360;			// 生きている時間

	Vector3 newPos;
	Vector3 dir;
	float distance;

	// プレイヤー用
	bool isPlayerDrop = false;
	float lifeTimer = 0.0f;
	const float maxLife = 1.0f;

	float rotateAngle = 0.0f;
	float rotateSpeed = 1.5f;
	float orbitRadius = 1.0f;
	float shrinkSpeed = 0.2f;				// 半径の縮小スピード
	float minRadius = 0.05f;				// 最小半径

	Meteor::MeteorType meteorType;
};