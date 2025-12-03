#include "Player.h"
#include "../Input/Input.h"
#include "../Manager/StageManager.h"
#include "../Manager/RingManager.h"
#include "../Manager/UIManager.h"
#include "../Game/Game.h"
#include "../Math/Quaternion.h"
#include "../Camera/Camera.h"
#include "../Effect/Trail.h"
#include "../Manager/MeteorManager.h"
#include "../Scene/GameOverScene.h"
#include "../AudioSystem/SoundManager.h"
#include "../Object/EnemyManager.h"

Player::Player(){
	Initialize();
	Load("Assets/astro/Astrounalts.fbx");
	LoadAnimation("Idle", "Assets/astro/astro_idle.fbx");
	LoadAnimation("Attack", "Assets/astro/astro_attack.fbx");
	LoadAnimation("Damage", "Assets/astro/astro_damage.fbx");
	LoadAnimation("Death", "Assets/astro/Death.fbx");

	scale = firstScale;
	rotation = Vector3(0.0f, 0.0f, 0.0f);
	position = firstPosition;
	Animation("Idle", true, 0.0f);

	easingVelocity = std::make_unique<Easing>(Easing::STATE::EASEINCUBIC, velSpeed, 0.0f, true);
	easingRotation = std::make_unique<Easing>(Easing::STATE::LINEAR, rotSpeed, 0.0f, true);
	easingRestoreRotation = std::make_unique<Easing>(Easing::STATE::EASEOUTEXPO, rotRestoreSpeed, 0.0f, true);

	shader = std::make_unique<Shader>();
	shader->Create(L"Assets/shader/3D.fx", "VSMain", "PSMain");

	trailR = std::make_unique<Trail>();
	trailR->Initialize();
	trailR->SetPlayer(this);

	trailL = std::make_unique<Trail>();
	trailL->Initialize();
	trailL->SetPlayer(this);

	meteorObj = nullptr;
	ringManager = nullptr;
	CameraObj = nullptr;

	hp = 0.0f;
}

Player::~Player(){
}

void Player::Initialize(){
	State = IDLE;
}

void Player::Update() {

	// 無敵タイマー処理
	if (isInvincible) {
		invincibilityTimer -= DeltaTime;
		if (invincibilityTimer <= 0.0f) {
			isInvincible = false;
		}
	}

	switch (State) {
	case IDLE:   Idle();   break;
	case MOVE:   Move();   break;
	case ATTACK: Attack(); break;
	case DAMAGE: Damage(); break;
	case DEATH:  break;
	}

	CheckEnemyCollision();
	CheckMeteorCollision();

	Character::Update();
}

void Player::SetIdleState() {
	State = IDLE;
	Animation("Idle", true, blendTimeNormal);
	rotation = Vector3(0.0f, 0.0f, 0.0f);
}

void Player::Render(){
	shader->Activate();
	Object::Render();
	EmitTrail();
}

void Player::Idle() {
	float axisX = Input::GetAxisX();
	float axisY = Input::GetAxisY();

	if (Input::Key_Pad_Push('E', Input::BUTTON::B)) {
		State = ATTACK;
	}

	float axis = sqrtf(axisX * axisX + axisY * axisY);
	if (axis > DeadZone) {
		State = MOVE;
	}
	else {
		easingRotation->Reset();
		easingVelocity->Reset();
	}
		RestoreRotation();
}

void Player::Move(){
	//力で速度が変化
	axisX = Input::GetAxisX();
	axisY = Input::GetAxisY();

	Matrix m = DxSystem::ViewMatrix;
	//カメラ方向ベクトル
	Vector3 forward = -Vector3(m._13, 0.0f, m._33);
	forward.Normalize();
	Vector3 up = Vector3(m._12, m._22, 0.0f);
	up.Normalize();
	Vector3 right = Vector3(m._11, 0.0f, m._31);
	right.Normalize();

	velRate = easingVelocity->RateUpdate();

	Vector3 targetVelocity = (right * axisX + up * axisY) * velMax * velRate;
	velocity = targetVelocity;

	if (position.x > maxWidth) {
		position.x = maxWidth;
	}
	if (position.x < minWidth) {
		position.x = minWidth;
	}
	if (position.y > maxhHeight) {
		position.y = maxhHeight;
	}
	if (position.y < minHeight) {
		position.y = minHeight;
	}

	//速度で位置が変化
	move += velocity * DeltaTime * playerSpeedAdjust;
	move.z = 0.0f;
	position = move;

	Rot();

	if (Input::Key_Pad_Push('E', Input::BUTTON::B)) {
		State = ATTACK;
	}

	float axis = sqrtf(axisX * axisX + axisY * axisY);
	if (axis <= DeadZone) {
		State = IDLE;
	}
}

void Player::Rot() {
	float axisX = Input::GetAxisX();
	float axisY = Input::GetAxisY();
	rotRate = easingRotation->RateUpdate();

	targetRotation.z = axisX * 5 * rotRate;
	targetRotation.x = axisY * 5 * rotRate;

	// 軸回転を調整
	rotation.z -= targetRotation.z * DeltaTime;
	rotation.x += targetRotation.x * DeltaTime;

	// 回転の最大値を制限
	if (rotation.z > radian * maxRotZ) { rotation.z = radian * maxRotZ; }
	if (rotation.z < -radian * maxRotZ) { rotation.z = -radian * maxRotZ; }
	if (rotation.x > radian * maxRotX) { rotation.x = radian * maxRotX; }
	if (rotation.x < -radian * maxRotX) { rotation.x = -radian * maxRotX; }
	easingRestoreRotation->Reset();
}

void Player::Attack() {
	// 攻撃初期化
	if (!isAttacking) {
		isAttacking = true;
		attackTime = 0.0f;
		Animation("Attack", false, 0.0f);
		rotation = Vector3(PI / 2.0f, PI, 0.0f);
		SoundManager::SoundPlay(SoundState::Player_Attack_SE);

		attackStartPos = position;

		// カメラ前方方向取得
		Matrix camMat = DxSystem::ViewMatrix;
		Vector3 forward = -Vector3(camMat._13, 0.0f, camMat._33);
		forward.Normalize();

		attackEndPos = attackStartPos + forward * attackDistance;

		// イージング初期化
		attackFlontEase = std::make_unique<Easing>(Easing::STATE::EASEOUTQUINT, flontSpeed, 0.0f, true);
		attackBackEase = std::make_unique<Easing>(Easing::STATE::LINEAR, backSpeed, 0.0f, true);
	}
	attackTime += DeltaTime;
	if (attackTime <= attackTotalTime * advanceRatio) {
		// 前進中
		flontEase = attackFlontEase->RateUpdate();
		position = Vector3::Lerp(attackStartPos, attackEndPos, flontEase);
	}
	else {
		// 後退中
		backEase = attackBackEase->RateUpdate();
		position = Vector3::Lerp(attackEndPos, attackStartPos, backEase);
	}


	// 終了処理
	if (attackTime >= attackTotalTime || mesh->IsMotionFinished) {
		AttackEnd();
	}
}

void Player::AttackEnd() {
	position = attackStartPos;
	move = attackStartPos;
	State = IDLE;
	isAttacking = false;
	Animation("Idle", true, 0.0f);
	rotation = Vector3(0.0f, 0.0f, 0.0f);
	attackFlontEase.reset();
	attackBackEase.reset();
}

void Player::Damage() {
	Animation("Damage", false, blendTimeNormal);
	rotation = Vector3(PI, PI, 0.0f);
	UpdateHPState();
}

void Player::Death() {
	Animation("Death", false, blendTimeNormal);
	rotation = Vector3(PI, PI, 0.0f);
	if (mesh->IsMotionFinished) {
		GetGameScene()->JumpScene(NEW GameOverScene);
		SoundManager::BGMPlay(GameOver_BGM);
	}
}

void Player::ActivateInvincibility(){
	isInvincible = true;
	invincibilityTimer = invincibilityDuration; // 無敵時間
}

void Player::EmitTrail() {
	// 右手のボーン座標を取得
	Matrix mr = mesh->GetBoneMatrix(25);//ボーンname：RightHand
	mr *= mesh->transform;
	Vector3 RHand = Vector3(mr._41, mr._42, mr._43);

	// 左手のボーン座標を取得
	Matrix ml = mesh->GetBoneMatrix(11);//LeftHand
	ml *= mesh->transform;
	Vector3 LHand = Vector3(ml._41, ml._42, ml._43);

	// トレイル更新・描画
	trailR->Update(RHand);
	trailR->Draw();

	trailL->Update(LHand);
	trailL->Draw();
}

void Player::RestoreRotation() {
	rotRestoreRateZ = easingRestoreRotation->RateUpdate();
	//Z軸回転を戻す（左右の傾き）
	zRotSpeed = radian * charaSpeed;
	if (rotation.z > 0.0f) {
		rotation.z -= zRotSpeed * DeltaTime;
		if (rotation.z < 0.0f) rotation.z = 0.0f;
	}
	else if (rotation.z < 0.0f) {
		rotation.z += zRotSpeed * DeltaTime;
		if (rotation.z > 0.0f) rotation.z = 0.0f;
	}

	//X軸回転を戻す（上下の傾き）
	xRotSpeed = radian * charaSpeed;
	if (rotation.x > 0.0f) {
		rotation.x -= xRotSpeed * DeltaTime;
		if (rotation.x < 0.0f) rotation.x = 0.0f;
	}
	else if (rotation.x < 0.0f) {
		rotation.x += xRotSpeed * DeltaTime;
		if (rotation.x > 0.0f) rotation.x = 0.0f;
	}
}

void Player::UpdateHPState() {
	hp += hpDec;
	int newStep = (int)(hp / stepUnit);  // stepUnit分のHPで1段階進む

	if (newStep > hpDecreaseCount) {
		hpDecreaseCount = newStep;

		// 死亡
		if (hpDecreaseCount >= hpDecreaseMax) {
			State = DEATH;
		}
		else {
			SetIdleState();
		}
	}
}

void Player::CheckMeteorCollision() {
	if (isInvincible) return;

	meteorType = StageManager::current->MeteorFlag(position);
	if (meteorType == Meteor::MeteorType::Type2) {
		if (State == ATTACK && isAttacking) {
			AttackEnd();
		}

		if (scoreManager) {
			scoreManager->AddScore(-decreasePoint);
			ringManager->ScatterRings(position,5, true);
			SoundManager::SoundPlay(SoundState::Player_Damage_SE);
			SoundManager::SoundPlay(SoundState::Player_RingGet_SE);
			if (!isInvincible) {
				ActivateInvincibility();
			}
			State = DAMAGE;
		}
	}
	else if (meteorType == Meteor::MeteorType::Type1) {
		if (State == ATTACK && isAttacking) {
			AttackEnd();
		}
		SoundManager::SoundPlay(SoundState::Player_Damage_SE);
		State = DAMAGE;
		if (!isInvincible) {
			ActivateInvincibility();
		}
	}
}

void Player::CheckEnemyCollision() {
	if (!enemyManager) return;

	const auto& enemies = enemyManager->GetEnemies();

	for (const auto& enemyPtr : enemies) {
		Enemy* enemy = enemyPtr.get();
		if (!enemy) continue;

		if (enemy->GetState() == Enemy::Invincible && State == ATTACK) {

			Vector3 epos = enemy->GetPosition();
			Vector3 ppos = GetPosition();

			float dx = ppos.x - epos.x;
			float dy = ppos.y - epos.y;

			if (fabsf(dx) < hitRange && fabsf(dy) < hitRange) {
				SoundManager::SoundPlay(SoundState::Player_Damage_SE);
				State = DAMAGE;
				return;
			}
		}
	}
}
