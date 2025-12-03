#pragma once
#include "../DXShader.h"
#include "Character.h"
#include "../Easing/Easing.h"
#include "Meteor.h"

class GameOverScene;
class UI;
class RingManager;
class Camera;
class MeteorManager;
class Camera;
class Trail;
class ScoreManager;
class EnemyManager;

class Player :public Character {
public:
	Player();
	~Player();
	void Initialize();
	void Update();
	void Render();

	enum STATE {
		IDLE,
		MOVE,
		ATTACK,
		DAMAGE,
		DEATH,
	};
	void Death();
	void SetIdleState();

	//ゲッター
	STATE GetState() const { return State; }
	float GetHPRate()const { return (gaugeMax - hp) / gaugeMax; }
	//セッター
	///カメラの情報取得用
	void SetCamera(Camera* camera) { CameraObj = camera; }
	void SetMeteor(MeteorManager* meteor) { meteorObj = meteor; }
	void SetRingManager(RingManager* rManager) { ringManager = rManager; }
	void SetScoreManager(ScoreManager* manager) { scoreManager = manager; }
	void SetEnemyManager(EnemyManager* mgr) { enemyManager = mgr; }

	void EmitTrail();
	void RestoreRotation();

	void UpdateHPState();

	void CheckMeteorCollision();

	void CheckEnemyCollision();

private:
	STATE State;								// プレイヤーのステータス

	void Idle();
	void Move();
	void Rot();
	void Attack();
	void Damage();
	void ActivateInvincibility();
	void AttackEnd();

private:
	std::unique_ptr<Shader> shader;
	MeteorManager* meteorObj;
	RingManager* ringManager = nullptr;
	ScoreManager* scoreManager = nullptr;

	const Vector3 firstScale = Vector3(0.006f, 0.006f, 0.006f);
	Vector3 move;									// プレイヤーの移動量
	Vector3 velocity;								// 速度
	std::unique_ptr<Easing> easingVelocity;
	float velRate = 0.0f;							// プレイヤーの進む速さ
	float velSpeed = 0.05f;							// イージング速度
	float velMax = 1.0f;							// 最大
	float playerSpeedAdjust = 7.0f;                 // プレイヤーの速度補正
	const int decreasePoint = 2500;

	//攻撃
	Vector3 attackStartPos;
	Vector3 attackEndPos;
	float attackTime = 0.0f;
	float attackTotalTime = 1.0f;					// 攻撃アニメの長さに合わせて調整
	const float attackDistance = 2.5f;
	bool isAttacking = false;
	std::unique_ptr<Easing> attackFlontEase;
	std::unique_ptr<Easing> attackBackEase;

	const float advanceRatio = 0.8f;				// 前進と後退の時間比率
	const float flontSpeed = 0.05f;					// イージング速度
	float flontEase = 0.0f;

	float backEase = 0.0f;
	float backSpeed = 0.1f;

	//キー入力
	float axisX;
	float axisY;
	float axis;

	const Vector3 firstPosition = Vector3(0.0f, 0.0f, 0.0f);//ゲーム開始座標

	Camera* CameraObj;							//カメラオブジェクト

	//HPゲージ
	const float gaugeMax = 300.0f;				//最大値
	float hp;									//hpゲージ
	const float hpDec = 3.0f;					//減少値
	int newStep;
	const float stepUnit = 60.0f;
	int hpDecreaseCount = 0;					// HP減少回数
	const int hpDecreaseMax = 5;				// 最大回数

	Meteor::MeteorType meteorType;
	bool isFastDeath = false;

	//無敵
	const float invincibilityDuration = 2.0f;	// 無敵時間
	float invincibilityTimer = 0.0f;			// 無敵タイマー
	bool isInvincible = false;

	//移動制限
	const float minWidth = -2.0f;				// 左の上限
	const float maxWidth = 2.0f;				// 右の上限
	const float minHeight = -1.2f;				// 下の上限
	const float maxhHeight = 1.2f;				// 上の上限

	const float blendTimeNormal = 0.3f;			//モーションブレンド時間
	const float blendTimeFast = 0.05f;			//モーションブレンド時間

	//回転
	std::unique_ptr<Easing> easingRotation;
	std::unique_ptr<Easing> easingRestoreRotation;
	float rotRate = 0.0f;						// プレイヤーの回転率
	float rotSpeed = 0.05f;						// イージング速度
	float rotSpeedAdjust = 3.0f;
	Vector3 targetRotation;
	float rotRestoreRateZ = 0.0f;
	float rotRestoreSpeed = 0.1f;				// イージング速度

	float zRotSpeed;
	float xRotSpeed;
	const float radian = 3.14f / 180.0f;
	const float charaSpeed = 140.0f;			// プレイヤーの傾く速度
	const float maxRotZ = 30.0f;				// 右の上限
	const float maxRotX = 10.0f;				// 上の上限

	//カメラ
	Camera* camera;

	//トレイル
	std::unique_ptr<Trail> trailR;					//右のトレイル
	std::unique_ptr<Trail> trailL;					//左のトレイル

	EnemyManager* enemyManager = nullptr;
	const float hitRange = 1.0f;
};

