#include "Camera.h"
#include "../Input/Input.h"
#include "../global.h"
#include "../Scene/ResultScene.h"
#include "../Easing/Easing.h"
#include "../Memory/Memory.h"
#include "../Game/Game.h"

void Camera::Initialize(){
	// 初期カメラ設定
	DxSystem::ViewMatrix.LookAt(firstCameraPos, firstTarget);
	// 初期投影設定
	DxSystem::ProjectionMatrix.PerspectiveFov(
	//視野角 縦横比	幅/高                                            近限界　 遠限界
		viewingAngle,(float)DxSystem::ScreenWidth / DxSystem::ScreenHeight, nearLimit, farLimit
	);
	//視野角　小さくすると望遠鏡のように使える
	//縦横比　そのままの意味
	//近限界　カメラからその位置までの範囲が見えない
	//遠限界　カメラその位置までの範囲が見える
}

void Camera::Update() {
	switch (cameraState) {
	case NORMAL: CameraDelay(); break;
	case START: Start(); break;
	case GOAL: Goal(); break;
	}
}

void Camera::SetState(Camera::STATE st) {
	if (cameraState != st) {
		cameraState = st;

		ResetGoalState();
	}
}

void Camera::ResetGoalState() {
	// GOAL演出開始時のパラメータ初期化
	if (cameraState == STATE::GOAL) {
		goalElapsedTime = 0.0f;
		elapsedTime = 0.0f;
		isFollowing = false;
	}
}

void Camera::SetTarget(Vector3 val){
	target = val;
}

Vector3 Camera::CalcOffsetCameraPos(float dist) const {
	// カメラ角度から前方ベクトルを求め距離分オフセット
	Matrix mat;
	mat.RotationXYZ(-camAngleY, camAngleX, 0);
	return Vector3(mat._31, mat._32, mat._33) * dist;
}

Vector3 Camera::SmoothFollow(const Vector3& current, const Vector3& target, float smoothSpeed) {
	// expを利用したカメラ追従の平滑化
	float smooth = 1.0f - expf(smoothSpeed * DeltaTime);
	return current * (1.0f - smooth) + target * smooth;
}

Vector3 Camera::WorldToScreen(const Vector3& worldPos, const Matrix& view, const Matrix& proj){
	// ビュー × プロジェクション合成
	Matrix vp;
	vp.multiply(view, proj);

	// NDC変換
	Vector3 ndc;
	ndc.TransformCoord((Vector3&)worldPos, vp);

	// スクリーン座標変換
	Vector3 screen;
	screen.x = (ndc.x * 0.5f + 0.5f) * DxSystem::ScreenWidth;
	screen.y = (-ndc.y * 0.5f + 0.5f) * DxSystem::ScreenHeight;
	screen.z = ndc.z;

	return screen;
}

void Camera::CameraDelay(){
	if (!isFollowing) return;

	// 現在角度からカメラ位置を算出
	Vector3 desiredPos = target + CalcOffsetCameraPos(camDistance);

	// 最大ズレ制限
	Vector3 diff = desiredPos - cameraPos;
	float len = diff.Length();

	if (len > maxOffset) {
		diff = diff / len * maxOffset;
		desiredPos = cameraPos + diff;
	}

	cameraPos = SmoothFollow(cameraPos, desiredPos, camSmoothSpeed);

	// スプリング感
	cameraPos = cameraPos + diff * spring * DeltaTime;

	DxSystem::ViewMatrix.LookAt(cameraPos, target);
}

void Camera::Start() {
	// カメラの最終位置 
	finalPos = target + CalcOffsetCameraPos(camDistance);

	if (!isFollowing) {
		elapsedTime += DeltaTime;

		// 角度を時間に応じて変化
		startAngle = (elapsedTime / startDuration) * PI;
		startOffsetX = startRadius * sin(startAngle);  // X軸方向の移動
		startOffsetZ = startRadius * (1 - cos(startAngle)); // Z軸方向の移動

		// カメラの現在位置（プレイヤーの正面からスタートし、背後に移動）
		startEye = Vector3(target.x + startOffsetX, target.y + startEyeHeight, target.z - startRadius + startOffsetZ);

		// 最終位置に移動
		startTarget = elapsedTime / startDuration;
		startEye = startEye * (1.0f - startTarget) + finalPos * startTarget;

		DxSystem::ViewMatrix.LookAt(startEye, target);

		if (elapsedTime >= startDuration) {
			isFollowing = true;
			cameraPos = finalPos;
			cameraState = STATE::NORMAL;
		}
	}
}

void Camera::Goal() {
	goalElapsedTime += DeltaTime;

	// 時間に応じた角度
	goalAngle = (goalElapsedTime / goalDuration) * (PI * goalRotRate);

	goalOffsetX = goalRadius * cos(goalAngle); // 横
	goalOffsetZ = goalRadius * sin(goalAngle); // 奥行き方向

	// プレイヤーを中心に円を描くようにカメラ配置
	Vector3 gEye = target + Vector3(-goalOffsetX, eyePosY, -goalOffsetZ); // 少し上から

	DxSystem::ViewMatrix.LookAt(gEye, target);

	// 終了後位置固定
	if (goalElapsedTime >= goalDuration) {
		cameraPos = gEye;
	}
}