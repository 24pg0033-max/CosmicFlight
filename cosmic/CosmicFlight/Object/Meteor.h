#pragma once
#include "Object.h"

class Meteor:public Object{
public:
	Meteor();
	~Meteor();

	enum class MotionType {
		Straight,
		SpiralR,
		SpiralL,
		Snake,
		Wave,
		Diagonal,
	};
	MotionType motionType = MotionType::Straight;

	enum class ArcState {
		Arc,
		NormalMotion
	};
	ArcState arcState = ArcState::Arc;

	void Initialize(FBXMesh* pMesh);
	void Update();

	void ArcMotion();

	void MotionStraight();
	void MotionSpiralBase();
	void MotionSpiralR();
	void MotionSpiralL();
	void MotionSnake();
	void MotionWave();
	void MotionDiagonal();

	bool GetFlag() const { return flag; }				// 当たったかどうかのフラグ取得
	void SetFlag(bool flag) { flag = flag; }	// 当たったかどうかのセット

	void SetScale(const Vector3& s) { scale = s; }
	void SetRotationSpeed(const Vector3& rs) { rotationSpeed = rs; }
	void SetRotation(const Vector3& r) { rotation = r; }

	enum class MeteorType {
		None,     // 当たってない
		Type1,    // 通常
		Type2     // 赤色(即死用)
	};
	MeteorType type = MeteorType::None;

	MeteorType GetType() const { return type; }
	void SetType(MeteorType t) { type = t; }

private:
	bool flag;						// 当たったかどうかのフラグ
	Vector3 velocity = Vector3(0.0f, 0.0f, 20.0f);
	Vector3 rotationSpeed = Vector3(0.0f, 6.0f, 0.0f);
	Vector3 move;
	const Vector3 firstMeteorScale = Vector3(2.5f, 2.5f, 2.5f);

	float motionTime = 0.0f;
	float spiralRadius = 15.0f;
	float spiralSpeed = 3.0f;
	float waveAmplitude = 5.0f;
	float waveFrequency = 3.0f;

	float spiralX = 0.0f;
	float spiralY = 0.0f;
	float snakeX = 0.0f;

	float arcDuration = 1.0f;   // 弧を描く時間（秒）
	float arcElapsed = 0.0f;
	float arcRadius = 20.0f;     // 弧の大きさ
	float arcSpeed = 3.0f;      // 弧の角速度（ラジアン/秒）
	float arcAngle = 0.0f;
	Vector3 arcStartPos;
	Vector3 straightDir;
	bool arcInit = false;
	float waveX = 0.0f;
	float waveY = 0.0f;
	float diagonalX = 0.0f;
	float diagonalY = 0.0f;
};