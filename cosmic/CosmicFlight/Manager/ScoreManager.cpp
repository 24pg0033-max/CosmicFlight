#include "ScoreManager.h"
#include <string>
#include "../Memory/Memory.h"
#include "../Easing/Easing.h"
#include "../UI/2DUI.h"
#include "../imgui/imgui.h"
#include "../Input/Input.h"

ScoreManager::ScoreManager() : score(0) {

}

ScoreManager::~ScoreManager() {
    for (auto ui : currentScoreUI) delete ui;
    for (auto ui : targetScoreUI) delete ui;
    currentScoreUI.clear();
    targetScoreUI.clear();
    delete slashUI; slashUI = nullptr;
    delete clearUI; clearUI = nullptr;
    if (clearUIAlphaEasing) { delete clearUIAlphaEasing; clearUIAlphaEasing = nullptr; }
}

void ScoreManager::Initialize() {
    currentScoreUI.clear();
    targetScoreUI.clear();

    baseX = digitX - (digitWidth + baseXAdjust) * maxDigits;
    y = digitY;

    // 現在スコア用
    for (int i = 0; i < maxDigits; ++i) {
        digit = NEW _2DUI();
        digit->Initialize("Assets/UI/number.png", digitWidth, digitHeight);
        digit->SetSize(digitWidth, digitHeight);
        digit->SetPosition(Vector3(baseX + digitWidth * i, y, 0));
        digit->SetRender_Flag(false);
        currentScoreUI.push_back(digit);
    }

    // 目標スコア用（"/"の右側）
    for (int i = 0; i < maxDigits; ++i) {
        digit = NEW _2DUI();
        digit->Initialize("Assets/UI/number.png", digitWidth, digitHeight);
        digit->SetSize(digitWidth, digitHeight);
        digit->SetPosition(Vector3(baseX + digitWidth * (i + maxDigits + 1), y, 0)); // 少し右にずらす
        digit->SetRender_Flag(false);
        targetScoreUI.push_back(digit);
    }

    // ("/")これ
    slashUI = NEW _2DUI();
    slashUI->Initialize("Assets/UI/slash.png", digitWidth, digitHeight);
    slashUI->SetSize(digitWidth * 1.5f, digitHeight * 1.5f);

    // スラッシュの位置
    slashX = baseX + digitWidth * maxDigits + slashXAdjust;
    slashY = 5.0f;
    slashUI->SetPosition(Vector3(slashX, slashY, 0));

    clearUI = NEW _2DUI();
    clearUI->Initialize("Assets/UI/complete.png", clearUIX, clearUIY, clearUIPos);
    clearUI->SetRender_Flag(false);  // 最初は非表示

    clearUIAlphaEasing = NEW Easing(Easing::STATE::EASEOUTCUBIC, 0.02f, 0.0f, false);

    this->Update();

    UpdateNumberUI(currentScoreUI, score, 0);
    UpdateNumberUI(targetScoreUI, finalTarget, maxDigits + 1);
}

void ScoreManager::Update() {
    if (!isCleared && GetCurrentPhase() == Phase::COMPLETE) {
        // UI切り替え処理
        for (auto digit : currentScoreUI) digit->SetRender_Flag(false);
        for (auto digit : targetScoreUI) digit->SetRender_Flag(false);
        if (slashUI) slashUI->SetRender_Flag(false);
        if (clearUI) clearUI->SetRender_Flag(true);
    }

    for (auto digit : currentScoreUI) digit->Update();
    for (auto digit : targetScoreUI) digit->Update();

    if (clearUI && clearUI->GetRender_Flag()) {
        float rate = clearUIAlphaEasing->RateUpdate();
        clearUI->SetAlpha(rate);
        if (clearUIAlphaEasing->GetIsMaxRate() || clearUIAlphaEasing->GetIsMinRate()) {
            clearUIAlphaEasing->ChangeIsOn();
        }
    }
}

void ScoreManager::Render() {
    for (auto digit : currentScoreUI) digit->Render();
    for (auto digit : targetScoreUI) digit->Render();
    if (slashUI && slashUI->GetRender_Flag()) slashUI->Render();
    if (clearUI && clearUI->GetRender_Flag()) {
        clearUI->Render();
    }
}

void ScoreManager::AddScore(int value) {
    // スコア加算
    if (isCleared) return;
    score += value;
    if (score < 0) score = 0;
    UpdateNumberUI(currentScoreUI, score, 0);
}

int ScoreManager::GetScore() const {
    return score;
}

ScoreManager::Phase ScoreManager::GetCurrentPhase() const {
    if (score < firstPhase) return Phase::FIRST;
    if (score < secondPhase) return Phase::SECOND;
    if (score < finalTarget) return Phase::FINAL;
    return Phase::COMPLETE;
}

void ScoreManager::UpdateNumberUI(std::vector<_2DUI*>& uiList, int value, float offsetX) {
    std::string str = std::to_string(value);
    int len = (int)str.length();

    int startIndex = maxDigits - len; // 右詰め開始位置

    for (int i = 0; i < maxDigits; ++i) {
        if (i < startIndex) {
            uiList[i]->SetRender_Flag(false); // 左側は非表示
        }
        else {
            int num = str[i - startIndex] - '0';
            float u = (float)num / 10.0f;
            uiList[i]->SetUV(u, 0.0f, 1.0f / 10.0f, 1.0f);
            uiList[i]->SetRender_Flag(true);
        }
    }
}

