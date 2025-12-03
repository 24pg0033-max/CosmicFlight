#pragma once
#include "Enemy.h"
#include <vector>
#include "../Render2D.h"
#include "../Camera/Camera.h"

class Player;

class EnemyManager {
public:
    EnemyManager();
    ~EnemyManager();

    void Update(Player* player);

    void Render(Camera* camera);

    void SpawnEnemy(float radius, float speed);
    void CheckCollision(Player* player);

    // ロックオン対象取得
    Enemy* GetClosestLockonTarget(const Vector3& playerPos, float lockonRange);

    void SetRingManager(RingManager* manager) { ringManager = manager; }
    Enemy* GetLockOnTarget() const { return lockOnTarget; }

    std::vector<std::unique_ptr<Enemy>>& GetEnemies() { return enemies; }

private:
    Shader* shader3D = nullptr;
    Shader* shaderOutline = nullptr;
    std::vector<std::unique_ptr<Enemy>> enemies;
    float spawnInterval = 5.0f;
    float timer = 0.0f;
    Vector3 spawnPos;
    Vector3 targetPos;
    float speed;
    float radius;
    const int maxEnemies = 5; // 最大出現数
    const float eAdjustSpeed = 0.04f;

    RingManager* ringManager = nullptr;

    // ロックオン
    Enemy* lockOnTarget = nullptr;
    Render2D* lockonUI = nullptr;
    Vector3 screenPos;
    const float lockonUIX = 64.0f;
    const float lockonUIY = 64.0f;
    const Vector3 lockonUIAdjust = Vector3(-2.0f, -30.0f, 0.0f);
    Vector3 playerPos;
    Vector3 enemyPos;
    const float killThreshold = 1.0f;
};
