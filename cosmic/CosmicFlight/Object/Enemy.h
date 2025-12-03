#pragma once
#include "Character.h"
#include "../DxShader.h"
#include "../RenderTarget.h"
#include "../global.h"

class RingManager;

class Enemy : public Character {
public:
    Enemy(Vector3 position, Vector3 target, float speed, float radius);
    Enemy(const Enemy& other);

    void Initialize();
    void SetShader(Shader* normal, Shader* outline);
    void Update();
    void Render();

    void AfterDeathMovement();
    void AppearanceMove();
    void ChangeInvincible();
    void SetAttackable(bool v);

    Matrix GetTransForm() const;
    void OnDefeated();

    Vector3 position;
    Vector3 GetPosition() const { return position; }

    Vector3 GetStartPosition() const { return startPos; }
    Vector3 GetTargetPosition() const { return target; }

    enum EnemyState {
        Alive,
        Invincible,       // 攻撃するとプレイヤーがダメージ
        Dying,
        Dead
    };

    EnemyState GetState() const { return state; }

    void SetRingManager(RingManager* rManager) { ringManager = rManager; }

    static std::unique_ptr<Enemy> enemyTempGhost;
    static std::unique_ptr<Enemy> enemyTempGhost2;
    static void CreateTemplate();

    void SetStartAndTarget(Vector3 start, Vector3 end);
    void SetSpeed(float s);
    void SetRadius(float r);

    bool isLockon = false;            // ロックオンされているかどうか
private:
    Shader* shader3D = nullptr;
    Shader* shaderOutline = nullptr;
    float side;
    Vector3 target;
    float speed;
    float radius;
    Vector3 direction;

    float time = 0.0f;
    float shrinkTime;                   // 縮まるまでの時間
    Vector3 deathRot = Vector3(0.0f, 0.2f, 0.0f);
    Vector3 startPos;
    Vector3 controlPoint;
    Vector3 endPos;

    float oscillationTimer = 0.0f;

    EnemyState state = Alive;
    float deathTimer = 0.0f;
    float deathDuration = 1.0f;         // アニメーションの長さ

    RingManager* ringManager = nullptr;

    float frequency = 3.0f;
    float amplitude = 0.5f;
    float scaleProgress;

    float modeChangeTimer = 0.0f;
    float aliveDuration = 7.0f;       // 攻撃可能時間
    float invincibleDuration = 7.0f;  // 無敵状態時間
    const int ringAmount = 5;
};
