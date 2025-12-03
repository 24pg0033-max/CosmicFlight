#pragma once
#include "../DxSystem.h"
#include "../RenderTarget.h"
#include "../Object/Object.h"

class Easing;
class Fade;

class PostEffect {
public:
    void Initialize();
    void Release();
    void Update();
    void Render();

    void Activate();
    void Deactivate();

    void BloomActivate();

    void SetFadeOut(float value);
    void UpdateFadeInOut(bool isOut, float time, float deltaTime);
    float GetFadeOut()const { return transi.fadeOut; }
    bool IsFadeCompleted() const { return fadeCompleted; }

    struct CBPostEffectParam {
        float bloomThreshold; // Bloom高輝度しきい値
        float bloom;
        float isBloomTarget;
        float dummy;
    };CBPostEffectParam posteffectParam;

    ID3D11Buffer* postEffectConstantBuffer = nullptr;
private:

    struct TransitionBuffer
    {
        float fadeOut;
        float time;
        float2 dummy;
    };  TransitionBuffer transi;

    std::unique_ptr<Shader> posteffectShader;   // メインポストエフェクト
    std::unique_ptr<RenderTarget> screen;       // ベース描画用RT

    //Fade用
    ID3D11Buffer* pTransitionBuffer = nullptr;
    std::unique_ptr<Shader> fade;               // フェード描画用

    std::unique_ptr<Easing> easingWhiteOut;     // フェード用Easing
    bool isFading = false;
    bool fadeOutMode = false;
    bool fadeCompleted = false;
    float speedPerSecond;
    float start;

    //ブルーム
    std::unique_ptr<RenderTarget> bloomSource;  // 輝度抽出元
    std::unique_ptr<RenderTarget> bloomWork;    // 輝度抽出
    std::unique_ptr<Shader> bloomFilter;        // 高輝度抽出シェーダ

    std::vector<std::unique_ptr<RenderTarget>> bloomWork2;
    std::unique_ptr<Shader> bloomBlur;          // ぼかしシェーダ

    std::unique_ptr<RenderTarget> bloomComposit;// 合成RT
    std::unique_ptr<Shader> bloom;              // ブルーム合成シェーダ
};