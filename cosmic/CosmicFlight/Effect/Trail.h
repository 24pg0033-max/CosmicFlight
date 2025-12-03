#pragma once
#include <deque>
#include <memory>
#include "../DxShader.h"
#include "../Object/Model.h"
#include "../Texture.h"

class Player;

class Trail
{
public:
    void Initialize();
    void Update(const Vector3& position);
    void Draw();

    void SetPlayer(Player* p) { player = p; }

private:
    static const int MAX_TRAIL = 2400;
    std::deque<Vector3> trailPositions;

    std::unique_ptr<Texture> trailTexture;
    std::unique_ptr<Shader>  trailShader;
    std::unique_ptr<Model>   trailModel;

    Vector3 forward;
    float angleY;
    Vector3 currentPos;
    Vector3 prevPos;

    Player* player = nullptr;
    Vector3 playerRotation;

    const int density = 3; // ï‚ä‘ì_ÇÃêî
};
