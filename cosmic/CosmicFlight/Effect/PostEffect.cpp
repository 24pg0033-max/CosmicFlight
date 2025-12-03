#include "PostEffect.h"
#include "../DXShader.h"
#include "../Easing/Easing.h"
#include "../Memory/Memory.h"
#include "../Scene/InGame.h"

void PostEffect::Initialize() {
	// フェード
	pTransitionBuffer = Shader::CreateConstantBuffer(sizeof(transi));
	fade = std::make_unique<Shader>();
	fade->Create(L"Assets/shader/Fade.fx", "VSMain", "PSMain");

	easingWhiteOut = nullptr;
	transi.fadeOut = 1.0f;

	// ブルーム
	postEffectConstantBuffer = Shader::CreateConstantBuffer(sizeof(posteffectParam));

	bloomSource = std::make_unique<RenderTarget>();
	bloomSource->Initialize();

	bloomWork = std::make_unique<RenderTarget>();
	bloomWork->Initialize(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, DXGI_FORMAT_R16G16B16A16_FLOAT);
	bloomFilter = std::make_unique<Shader>();
	bloomFilter->Create(L"Assets/shader/posteffect.fx", "VSMain", "PSBloomFilter");

	bloomWork2.clear();
	bloomWork2.reserve(5);

	float scale = 0.5f;
	for (int i = 0; i < 5; i++) {
		auto rt = std::make_unique<RenderTarget>();
		rt->Initialize((int)(1280 * scale), (int)(720 * scale), DXGI_FORMAT_R16G16B16A16_FLOAT);
		bloomWork2.push_back(std::move(rt));
		scale *= 0.4f;
	}

	bloomBlur = std::make_unique<Shader>();
	bloomBlur->Create(L"Assets/shader/posteffect.fx", "VSMain", "PSBloomBlur");

	bloomComposit = std::make_unique<RenderTarget>();
	bloomComposit->Initialize(0, 0, DXGI_FORMAT_R16G16B16A16_FLOAT);

	bloom = std::make_unique<Shader>();
	bloom->Create(L"Assets/shader/posteffect.fx", "VSMain", "PSBloom");

	screen = std::make_unique<RenderTarget>();
	screen->Initialize();

	posteffectShader = std::make_unique<Shader>();
	posteffectShader->Create(L"Assets/shader/posteffect.fx", "VSMain", "PSMain");

	posteffectParam.bloomThreshold = 0.7f;
	posteffectParam.bloom = 2.0f;
}

void PostEffect::Release() {

	fade.reset();
	bloomWork.reset();
	bloomFilter.reset();
	bloomBlur.reset();
	bloomComposit.reset();
	bloom.reset();

	if (postEffectConstantBuffer) {
		postEffectConstantBuffer->Release();
		postEffectConstantBuffer = nullptr;
	}

	screen.reset();
	posteffectShader.reset();

	if (pTransitionBuffer) {
		pTransitionBuffer->Release();
		pTransitionBuffer = nullptr;
	}
}

void PostEffect::Update(){
	DxSystem::DeviceContext->UpdateSubresource(pTransitionBuffer, 0, NULL, &transi, 0, 0);
	DxSystem::DeviceContext->PSSetConstantBuffers(4, 1, &pTransitionBuffer);
}

void PostEffect::Render() {
	// 高輝度抽出
	bloomWork->Activate();
	bloomFilter->Activate();
	bloomSource->Render();
	bloomWork->Deactivate();

	// ぼかし(高輝度抽出bloom_workをblurで描画)
	bloomBlur->Activate();	//ぼかしシェーダーセット
	bloomWork->Render();

	bloomWork2[0]->Activate(); // サイズ半分のターゲット
	bloomWork->Render(); // 明るい部分をぼかして描画

	bloomWork2[1]->Activate(); //さらに半分のターゲット
	bloomWork2[0]->Render(); // ぼかしたテクスチャをさらにぼかす

	bloomWork2[2]->Activate(); //さらに半分のターゲット
	bloomWork2[1]->Render(); // ぼかしたテクスチャをさらにぼかす

	bloomWork2[3]->Activate(); //さらに半分のターゲット
	bloomWork2[2]->Render(); // ぼかしたテクスチャをさらにぼかす

	bloomWork2[4]->Activate(); //さらに半分のターゲット
	bloomWork2[3]->Render(); // ぼかしたテクスチャをさらにぼかす

	// compositにbloomシェーダーで加算合成
	bloomComposit->Activate();
	bloom->Activate();
	// screenとbloom_work2を加算合成
	bloomWork2[0]->Set(1); //slot1に設定
	bloomWork2[1]->Set(2); //slot2に設定
	bloomWork2[2]->Set(3); //slot3に設定
	bloomWork2[3]->Set(4); //slot4に設定
	bloomWork2[4]->Set(5); //slot5に設定

	screen->Render();
	bloomComposit->Deactivate();

	// ビューポートを全画面に
	D3D11_VIEWPORT vp = { 0,0,DxSystem::ScreenWidth,DxSystem::ScreenHeight,0,1 };
	DxSystem::DeviceContext->RSSetViewports(1, &vp);

	bloomFilter->Activate();
	bloomWork->Render();

	posteffectShader->Activate();
	fade->Activate();
	bloomComposit->Render();

	DxSystem::DeviceContext->OMSetRenderTargets(1, &DxSystem::RenderTargetView, DxSystem::DepthStencilView);

	vp = { 0,0,(float)DxSystem::ScreenWidth,(float)DxSystem::ScreenHeight,0.0f,1.0f };
	DxSystem::DeviceContext->RSSetViewports(1, &vp);

	DxSystem::DeviceContext->ClearDepthStencilView(DxSystem::DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void PostEffect::Activate() {
	DxSystem::DeviceContext->UpdateSubresource(postEffectConstantBuffer, 0, NULL, &posteffectParam, 0, 0);
	DxSystem::DeviceContext->PSSetConstantBuffers(8, 1, &postEffectConstantBuffer);
	screen->Activate();
}

void PostEffect::Deactivate() {
	screen->Deactivate();
}

void PostEffect::BloomActivate(){
	bloomSource->Activate();
}

void PostEffect::SetFadeOut(float value) {
	transi.fadeOut = value;
}

void PostEffect::UpdateFadeInOut(bool isOut, float time, float deltaTime) {
	// フェード中なら Easing による更新
	if (isFading && easingWhiteOut) {
		float value = easingWhiteOut->RateUpdate();
		transi.fadeOut = value;

		// 完了判定
		if ((fadeOutMode && easingWhiteOut->GetIsMaxRate()) ||
			(!fadeOutMode && easingWhiteOut->GetIsMinRate())) {
			isFading = false;
			fadeCompleted = true;
		}
		return;
	}

	// すでに限界値なら新規フェード開始しない
	if ((isOut && transi.fadeOut >= 1.0f) ||
		(!isOut && transi.fadeOut <= 0.0f)) {
		return;
	}

	fadeCompleted = false;

	// 新規フェード開始
	fadeOutMode = isOut;
	isFading = true;

	speedPerSecond = 1.0f / time;

	start = transi.fadeOut;
	bool isForward = isOut;

	// Easing インスタンス生成
	easingWhiteOut = std::make_unique<Easing>(Easing::STATE::LINEAR, speedPerSecond * deltaTime, start, isForward);
}