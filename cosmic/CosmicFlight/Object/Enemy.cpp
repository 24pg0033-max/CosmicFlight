#include "Enemy.h"
#include "../Manager/RingManager.h"
#include "../AudioSystem/SoundManager.h"
#include "../RenderState.h"

// テンプレート初期化
std::unique_ptr<Enemy> Enemy::enemyTempGhost = nullptr;
std::unique_ptr<Enemy> Enemy::enemyTempGhost2 = nullptr;

Enemy::Enemy(Vector3 position, Vector3 target, float speed, float radius)
    : position(position), target(target), speed(speed), radius(radius), time(0.0f), state(Alive), deathTimer(0.0f) {
    Initialize();
    startPos = position;
    endPos = target;
    side = (rand() % 2 == 0) ? 1.0f : -1.0f;
    controlPoint = (startPos + endPos) * 0.5f + Vector3(side * radius, radius * 0.5f, 0.0f);
}

Enemy::Enemy(const Enemy& other) {
    Initialize();
    CloneModel(other.mesh);
    rotation = other.rotation;
    ringManager = other.ringManager;
}

void Enemy::Initialize() {
    radius = 0.0f;
    speed = 0.0f;
    side = 0.0f;
    state = Alive;
    deathTimer = 0.0f;
    time = 0.0f;
}

void Enemy::SetShader(Shader* normal, Shader* outline) {
    shader3D = normal;
    shaderOutline = outline;
}

void Enemy::CreateTemplate() {
    if (enemyTempGhost != nullptr) return;
    if (enemyTempGhost2 != nullptr) return;

    // 通常タイプのテンプレ
    enemyTempGhost = std::make_unique<Enemy>(Vector3(), Vector3(), 0.0f, 0.0f);
    enemyTempGhost->Load("Assets/enemy/Ghost.fbx");
    enemyTempGhost->LoadAnimation("Idle", "Assets/enemy/Ghost_Idle.fbx");
    enemyTempGhost->LoadAnimation("Damage", "Assets/enemy/Ghost_Damege.fbx");
    enemyTempGhost->LoadAnimation("Change", "Assets/enemy/Ghost_Change.fbx");

    enemyTempGhost->Animation("Idle", true, 0.0f);
    enemyTempGhost->rotation = Vector3(-PI / 2.0f, 0.0f, 0.0f);

    // 無敵タイプのテンプレ
    enemyTempGhost2 = std::make_unique<Enemy>(Vector3(), Vector3(), 0.0f, 0.0f);
    enemyTempGhost2->Load("Assets/enemy/Ghost2.fbx");
    enemyTempGhost2->LoadAnimation("Idle", "Assets/enemy/Ghost_Idle.fbx");
    enemyTempGhost2->LoadAnimation("Damage", "Assets/enemy/Ghost_Damege.fbx");
    enemyTempGhost2->LoadAnimation("Change", "Assets/enemy/Ghost_Change.fbx");

    enemyTempGhost2->Animation("Idle", true, 0.0f);
    enemyTempGhost2->rotation = Vector3(-PI / 2.0f, 0.0f, 0.0f);
}

void Enemy::SetStartAndTarget(Vector3 start, Vector3 end) {
    this->startPos = this->position = start;
    this->endPos = end;
    this->time = 0.0f;

    side = (rand() % 2 == 0) ? 1.0f : -1.0f;
    controlPoint = (start + end) * 0.5f + Vector3(side * radius, radius * 0.5f, 0.0f);
}

void Enemy::SetSpeed(float s) {
    this->speed = s;
}

void Enemy::SetRadius(float r) {
    this->radius = r;
}

void Enemy::Update() {
    if (state == Dying) {
        deathTimer += DeltaTime;

        if (deathTimer < 0.2f) {        // 少し待機（アニメーションのみ再生）
            Character::Update();
            return;
        }

        AfterDeathMovement();

        Character::Update();
        return;
    }

    ChangeInvincible();
    AppearanceMove();

    Character::Update();
}

void Enemy::Render(){
    Shader::SetTransform(GetTransForm());

    // アウトライン（法線拡大）
    if(isLockon){
        RenderState::SetCullMode(1);
        shaderOutline->Activate();
        Object::Render();
        RenderState::SetCullMode(2);
    }

    shader3D->Activate();
    Object::Render();
}

void Enemy::AfterDeathMovement(){
    // 縮小・回転を開始
    shrinkTime = deathTimer - 0.2f;
    scaleProgress = shrinkTime / (deathDuration - 0.2f);
    scale = Vector3::Lerp(Vector3(1.0f, 1.0f, 1.0f), Vector3(0.0f, 0.0f, 0.0f), scaleProgress);
    rotation += deathRot;

    if (deathTimer >= deathDuration) {
        state = Dead;
    }
}

void Enemy::AppearanceMove(){
    // 画面外からの遷移
    if (time < 1.0f) {
        time += speed * DeltaTime;

        float u = 1.0f - time;
        Vector3 term1 = startPos * (u * u);
        Vector3 term2 = controlPoint * (2.0f * u * time);
        Vector3 term3 = endPos * (time * time);

        position = term1 + term2 + term3;
        oscillationTimer = 0.0f;
    }
    else {
        oscillationTimer += DeltaTime;

        position = endPos;
        position.y += sinf(oscillationTimer * frequency) * amplitude;
    }
}

void Enemy::ChangeInvincible() {
    // 無敵切替
    modeChangeTimer += DeltaTime;

    // Alive → Invincible
    if (state == Alive && modeChangeTimer >= aliveDuration) {
        state = Invincible;
        modeChangeTimer = 0.0f;
        CloneModel(enemyTempGhost2->mesh);
        rotation = Vector3(-PI / 2, 0, 0);
        Animation("Change", false, 0.0f);
        return;
    }

    // Invincible → Alive
    if (state == Invincible && modeChangeTimer >= invincibleDuration) {
        state = Alive;
        modeChangeTimer = 0.0f;
        CloneModel(enemyTempGhost->mesh);
        rotation = Vector3(-PI / 2, 0, 0);
        Animation("Change", false, 0.0f);
        return;
    }
}


void Enemy::SetAttackable(bool v) {
    if (v) {            // 攻撃可能状態へ
        state = Alive;
        modeChangeTimer = 0.0f;

        CloneModel(enemyTempGhost->mesh);
        rotation = Vector3(-PI / 2, 0, 0);
        Animation("Change", false, 0.0f);
    }
    else {              // 無敵状態へ
        state = Invincible;
        modeChangeTimer = 0.0f;

        CloneModel(enemyTempGhost2->mesh);
        rotation = Vector3(-PI / 2, 0, 0);
        Animation("Change", false, 0.0f);
    }
}

Matrix Enemy::GetTransForm() const {
    Matrix mat;
    mat.TRS((Vector3&)position, (Vector3&)rotation, (Vector3&)scale);
    return mat;
}

void Enemy::OnDefeated() {
    //エネミー撃破後
    if (state != Alive) return;

    state = Dying;
    deathTimer = 0.0f;
    Animation("Damage", false, 0.0f);
    rotation.x -= PI/4;

    if (ringManager) {
        SoundManager::SoundPlay(SoundState::Enemy_Hit_SE);
        ringManager->ScatterRings(position, ringAmount, false);
    }
}