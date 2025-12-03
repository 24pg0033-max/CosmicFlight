#pragma once
#include "../DxSystem.h"
#include "../DXShader.h"
#include "../GameSystem.h"

class Easing;

class Camera
{
public:
	enum STATE {
		NORMAL, // 通常追従
		START,  // 開始時の演出
		GOAL,   // ゴール演出
	};
	void Initialize();
	void Update();
	void SetState(Camera::STATE st);
	void ResetGoalState();
	void SetTarget(Vector3 val);

	Vector3 CalcOffsetCameraPos(float dist) const;

	Vector3 SmoothFollow(const Vector3& current, const Vector3& target, float smoothSpeed);

	Vector3 GetPosition() const { return cameraPos; }
	Vector3 WorldToScreen(const Vector3& worldPos, const Matrix& view, const Matrix& proj);

	float goalElapsedTime = 0.0f;
private:
	STATE cameraState = START;
	void Start();
	void Goal();
	void CameraDelay();

	Vector3 cameraPos;
	Vector3 target;

	Vector3 firstCameraPos = Vector3(0.0f, 1.0f, 3.0f);			// 初期配置
	Vector3 firstTarget = Vector3(0.0f, 1.0f, 0.0f);			// 初期注視点
	const float viewingAngle = 0.9f;							// 視野角
	const float nearLimit = 0.1f;								// 近限界
	const float farLimit = 1500.0f;								// 遠限界

	//Delay
	const float camDistance  = 3.0f;				// 距離
	const float camSmoothSpeed  = -10.0f;
	float camAngleX = 0.0f;							// 方向X
	float camAngleY = 0.0f;							// 方向Y
	Vector3 targetPos;								// カメラの位置
	Vector3 camForce;
	const float maxOffset = 0.8f;
	float smooth;
	const float spring = 0.8f;

	//Start
	Vector3 finalPos;
	bool isFollowing = false;
	float elapsedTime = 0.0f;
	float startAngle = 0.0f;
	const float startEyeHeight = 5.0f;
	float startOffsetX = 0.0f;						// 差し引き用
	float startOffsetZ = 0.0f;
	const float startDuration = 4.0f;				// 半周にかける時間（秒）
	const float startRadius = 10.0f;				// プレイヤーを中心とした回転半径
	Vector3 startEye;								// Start()のカメラ位置
	float startTarget;								// 最終位置

	//Goal
	float goalAngle = 0.0f;                         // 回転角
	float goalOffsetX;                              // 回転オフセット横
	float goalOffsetZ;                              // 回転オフセット奥

	const float goalDuration = 3.5f;                // ゴール演出の時間
	const float eyePosY = 3.5f;						// カメラ高さ
	const float goalRadius = 6.0f;					// 回転半径
	const float goalRotRate = 0.75f;				// 1周しないゆっくり回転率
};

