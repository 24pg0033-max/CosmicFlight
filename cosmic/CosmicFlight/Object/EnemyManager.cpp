#include "EnemyManager.h"
#include "Enemy.h"
#include "../global.h"
#include "Player.h"
#include "../Camera/Camera.h"

EnemyManager::EnemyManager()
    : spawnInterval(3.0f), timer(0.0f) {
    speed = 0.0f;
    radius = 0.0f;
    Enemy::CreateTemplate();

    shader3D = NEW Shader();
    shader3D->Create(L"Assets/shader/3D.fx", "VSMain", "PSMain");

    shaderOutline = NEW Shader();
    shaderOutline->Create(L"Assets/shader/3D.fx", "VSOutline", "PSOutline");

    Enemy::enemyTempGhost->SetShader(shader3D, shaderOutline);
    Enemy::enemyTempGhost2->SetShader(shader3D, shaderOutline);

    lockonUI = NEW Render2D();
    lockonUI->Initialize("Assets/UI/lockon.png", nullptr);
    lockonUI->SetSize(lockonUIX, lockonUIY);
}
EnemyManager::~EnemyManager() {
    delete shader3D;
    delete shaderOutline;
    enemies.clear();
    lockOnTarget = nullptr;
}

void EnemyManager::Update(Player* player) {
    timer += DeltaTime;

    if (timer >= spawnInterval && enemies.size() < maxEnemies) {
        timer = 0.0f;

        radius = 10.0f + (rand() % 5);
        speed = 0.24f + (rand() % 10) * 0.04f;

        SpawnEnemy(radius, speed);
    }

    for (auto& enemy : enemies) {
        enemy->Update();
        playerPos = player->GetPosition();
        enemyPos = enemy->GetPosition();

        float dx = playerPos.x - enemyPos.x;
        float dy = playerPos.y - enemyPos.y;

        float dist = sqrtf(dx * dx + dy * dy);
        enemy->isLockon = (dist < 1.0f);
    }
    CheckCollision(player);
}

void EnemyManager::Render(Camera* camera) {
    for (auto& enemy : enemies) {
        enemy->Render();

        if (lockonUI && enemy->GetState() == Enemy::EnemyState::Alive && enemy->isLockon) {
            screenPos = camera->WorldToScreen(enemy->GetPosition(), DxSystem::ViewMatrix, DxSystem::ProjectionMatrix);

            screenPos.x -= lockonUIX * 0.5f;
            screenPos.y -= lockonUIY * 0.5f;

            lockonUI->SetPosition(screenPos + lockonUIAdjust);
            lockonUI->Render();
        }
    }
}

void EnemyManager::SpawnEnemy(float radius, float speed) {
    if (enemies.size() >= maxEnemies) return;

    for (int attempts = 0; attempts < 10; ++attempts) {
        // スポーン位置
        float x = (rand() % 2 == 0) ? -30.0f : 30.0f;
        float y = 0.0f;
        float z = ((rand() % 40) - 40);
        Vector3 spawnPos(x, y, z);

        // ターゲット位置
        float tx = ((rand() % 10) - 5.0f);
        float ty = ((rand() % 4) - 2.0f);
        float tz = -4.0f - (rand() % 200) / 100.0f;
        Vector3 target(tx, ty, tz);

        auto enemy = std::make_unique<Enemy>(*Enemy::enemyTempGhost);
        enemy->SetRadius(radius);
        enemy->SetSpeed(speed);
        enemy->SetStartAndTarget(spawnPos, target);

        enemy->SetRingManager(ringManager);

        enemy->SetShader(shader3D, shaderOutline);

        Enemy* newEnemy = enemy.get();

        enemies.push_back(std::move(enemy));

        if (!lockOnTarget || lockOnTarget->GetState() == Enemy::EnemyState::Dead) {
            lockOnTarget = newEnemy;
        }
        return;
    }
}

void EnemyManager::CheckCollision(Player* player) {
    for (auto it = enemies.begin(); it != enemies.end(); ) {
        Enemy* enemy = it->get();

        // 死亡済み → 削除
        if (enemy->GetState() == Enemy::EnemyState::Dead) {
            it = enemies.erase(it);
            continue;
        }

        // プレイヤーが攻撃中かつX/Y軸が一致
        if (player->GetState() == Player::STATE::ATTACK && enemy->GetState() == Enemy::EnemyState::Alive) {
            playerPos = player->GetPosition();
            enemyPos = enemy->GetPosition();

            float dx = fabsf(playerPos.x - enemyPos.x);
            float dy = fabsf(playerPos.y - enemyPos.y);

            if (dx < killThreshold && dy < killThreshold) {
                if (enemy == lockOnTarget) {
                    lockOnTarget = nullptr;
                }
                enemy->OnDefeated();
            }
        }
        ++it;
    }
}

Enemy* EnemyManager::GetClosestLockonTarget(const Vector3& playerPos, float lockonRange) {
    // 一番近いものをターゲット
    Enemy* closest = nullptr;
    float minDistSq = lockonRange * lockonRange;

    for (auto& enemy : enemies) {
        if (enemy->GetState() != Enemy::EnemyState::Alive && (enemy->isLockon)) {

            Vector3 enemyPos = enemy->GetPosition();
            float dx = playerPos.x - enemyPos.x;
            float dy = playerPos.y - enemyPos.y;
            float dz = playerPos.z - enemyPos.z;

            float distSq = dx * dx + dy * dy + dz * dz;
            if (distSq < minDistSq) {
                minDistSq = distSq;
                closest = enemy.get();
            }
        }
    }
    return closest;
}