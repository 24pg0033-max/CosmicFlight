#include "Ring.h"
#include "../DXShader.h"
#include "../AudioSystem/SoundManager.h"
#include <algorithm>

void Ring::Initialize(FBXMesh* sourceMesh) {
	mesh = NEW FBXMesh();
	mesh->Clone(sourceMesh);
	scale = firstRingScale;
	rotation = firstRingRotation;
    distance = 0.0f;
    lifeTimer = 0.0f;
    isPlayerDrop = false;
	IsActive = true;
}

void Ring::Update() {
    // プレイヤーばら撒きリングの寿命処理
    if (isPlayerDrop) {
        lifeTimer += DeltaTime;
        if (lifeTimer >= maxLife) {
            IsActive = false;
            return;
        }
    }

    if (isScattering) {
        scatterTimer += DeltaTime;

        float time = min(scatterTimer / scatterDuration, 1.0f);
        Vector3 newPos = startPos * (1.0f - time) + targetPos * time;
        SetPosition(newPos);

        if (time >= 1.0f) {
            isScattering = false;
            waitScatter = true;
            waitTimer = 0.0f;
        }
    }
    else if (waitScatter) {
        waitTimer += DeltaTime;
        if (waitTimer >= waitDuration) {
            waitScatter = false;
            isAttracted = true;
        }
    }
    // 吸収処理
    else if (isAttracted && !isPlayerDrop) {
        Vector3 pos = GetPosition();
        Vector3 toTarget = attractTarget - pos;
        distance = toTarget.Length();

        if (distance < 0.05f) {
            IsActive = false;
            if (scoreManager) {
                SoundManager::SoundPlay(SoundState::Player_RingGet_SE);
                scoreManager->AddScore(getScore);
            }
        }
        else {
            // 吸収モーション（スパイラル）
            toTarget.Normalize();
            Vector3 forwardMove = toTarget * attractSpeed * DeltaTime;

            rotateAngle += rotateSpeed * DeltaTime;
            Vector3 spiralOffset(
                cosf(rotateAngle) * orbitRadius,
                0.0f,
                sinf(rotateAngle) * orbitRadius
            );

            orbitRadius = ClampMin(orbitRadius - shrinkSpeed * DeltaTime, minRadius);
            Vector3 newPos = pos + forwardMove + spiralOffset * 0.1f;
            SetPosition(newPos);
        }
    }

    Object::Update();
}



void Ring::Render(){
	if (IsActive) {
		Object::Render();
	}
}

