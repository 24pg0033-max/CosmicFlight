#include "UVAnim.h"
#include "../RenderState.h"
#include "../global.h"

void UVAnim::Initialize(){
	// モデル読み込み
	Object::Load("Assets/stage/skydome/sky.fbx");
	// コンスタントバッファ初期化
	CBUVAnim = Shader::CreateConstantBuffer(sizeof(UVAnimParam));
	uvanimParam.uSpeed = 0.0f;
	uvanimParam.vSpeed = 0.2f;
	uvanimParam.time = 0.0f;
	SetScale(firstScale);
	// シェーダー読み込み
	UVAnimShader.Create(L"Assets/shader/posteffect.fx", "VSUVAnim", "PSUVAnim");
}

void UVAnim::Release(){
	CBUVAnim->Release();
}

void UVAnim::Update(){
	// UVアニメコンスタントバッファ更新
	uvanimParam.time += DeltaTime;
	DxSystem::DeviceContext->UpdateSubresource(CBUVAnim, 0, NULL, &uvanimParam, 0, 0);

	Object::Update();
}


void UVAnim::Render(){
	UVAnimShader.Activate();
	// Slot1にセット
	DxSystem::DeviceContext->VSSetConstantBuffers(1, 1, &CBUVAnim);
	DxSystem::DeviceContext->PSSetConstantBuffers(1, 1, &CBUVAnim);

	// 加算合成・両面描画・デプス書き込み無し
	RenderState::SetBlendState(RenderState::ADD);
	RenderState::SetCullMode(2);
	RenderState::DepthEnable(true, false);

	Shader::SetTransform(GetTransForm());

	Object::Render();

	// 通常の合成・表面描画・デプス書き込みあり
	RenderState::SetBlendState(RenderState::ALPHA);
	RenderState::SetCullMode(0);
	RenderState::DepthEnable(true, true);
}
