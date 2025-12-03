#pragma once
#include <vector>
#include "../DxSystem.h"

class _2DUI;
class Easing;

class ScoreManager {
public:
    ScoreManager();
    ~ScoreManager();

    void Initialize();
    void Update();
    void Render();

    void AddScore(int value);
    int GetScore() const;

    enum class Phase {
        FIRST,
        SECOND,
        FINAL,
        COMPLETE
    };

    Phase GetCurrentPhase() const;

private:
    int score;
    _2DUI* digit;

    const int firstPhase = 5000;
    const int secondPhase = 10000;
    const int finalTarget = 15000;

    std::vector<_2DUI*> currentScoreUI; // 現在スコア用
    std::vector<_2DUI*> targetScoreUI;  // 目標スコア用

    const float digitWidth = 32;
    const float digitHeight = 64;
    const float digitX = 1000.0f;
    const float digitY = 20.0f;
    const float maxDigits = 6.0f;

    void UpdateNumberUI(std::vector<_2DUI*>& uiList, int value, float offsetX);

    // スラッシュ用UI
    _2DUI* slashUI = nullptr;

    //クリア用
    _2DUI* clearUI = nullptr;
    const float clearUIX = 600.0f;
    const float clearUIY = 150.0f;
    Vector3 clearUIPos = Vector3(380.0f, 300.0f, 0.0f);
    bool isCleared = false;
    Easing* clearUIAlphaEasing = nullptr;

    float baseX;
    const float baseXAdjust = 10.0f;
    float y;
    float slashX;
    const float slashXAdjust = 16.0f;
    float slashY;
    float rate;
};
