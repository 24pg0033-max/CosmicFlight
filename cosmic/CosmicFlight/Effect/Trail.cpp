#include "Trail.h"
#include "../DxSystem.h"
#include "../RenderState.h"
#include "../Memory/Memory.h"
#include "../global.h"
#include "../Object/Player.h"

void Trail::Initialize() {
    trailTexture = std::make_unique<Texture>();
    trailTexture->Load("Assets/Effect/trail.png");

    trailShader = std::make_unique<Shader>();
    trailShader->Create(L"Assets/shader/trail.fx", "VSMain", "PSMain");

    trailModel = std::make_unique<Model>();
    trailModel->Initialize();
}

void Trail::Update(const Vector3& basePosition) {
    currentPos = basePosition;

    // トレイルをZ方向に流す
    for (auto& trailPos : trailPositions) {
        trailPos.z += 0.1f;
    }

    // 密度設定
    if (!trailPositions.empty()) {
        prevPos = trailPositions.front();

        for (int i = 1; i <= density; ++i) {
            float t = static_cast<float>(i) / (density + 1);
            Vector3 interp = prevPos * (1.0f - t) + currentPos * t;
            trailPositions.push_front(interp);
        }
    }

    // 現在位置も追加（先端）
    trailPositions.push_front(currentPos);

    // 上限超えた分を削除
    while (trailPositions.size() > MAX_TRAIL) {
        trailPositions.pop_back();
    }
}




void Trail::Draw() {
    if (!trailShader || !trailTexture || !trailModel) return;

    trailShader->Activate();
    RenderState::SetBlendState(RenderState::ADD);
    RenderState::DepthEnable(true, false);

    trailTexture->Set(0);

    // カメラ方向の取得
    Matrix invView = DxSystem::ViewMatrix.Inverted();
    Vector3 camForward = invView.GetForward();

    // カメラに向ける向き（前面）
    forward = -camForward;
    forward.y = 0.0f;
    forward.Normalize();
    angleY = atan2f(forward.x, forward.z);

    for (size_t i = 0; i < trailPositions.size(); ++i)
    {
        Vector3 pos = trailPositions[i];
        Vector3 rot(PI / 2, angleY, 0.0f);
        Vector3 scale(0.5f, 0.5f, 0.5f);

        Matrix world;
        world.TRS(pos, rot, scale);

        trailShader->SetTransform(world);
        trailModel->Draw();
    }
        RenderState::SetBlendState(RenderState::ALPHA);
        RenderState::DepthEnable(true, true);
}