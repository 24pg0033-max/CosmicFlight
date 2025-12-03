#include "../GameSystem.h"
#include "Particle.h"
#include "../global.h"
#include "../Memory/Memory.h"

void ParticleSystem::Initialize(){
	animStart = 0.0f;
	animEnd = 0.0f;

	// 発生関連初期化
	emitRate = 0.0f;
	emitWork = 0.0f;

	// 情報初期化
	params.resize(maxParticles);
	points.resize(maxParticles);
	for (int i = 0; i < 10000; i++)
	{
		points[i].size = 0.0f; // 初期サイズ = 0

		params[i].isActive = false;
		params[i].position.x = (rand() % 6000 - 3000) * 0.01f;
		params[i].position.y = (rand() % 3000) * 0.01f;
		params[i].position.z = (rand() % 6000 - 3000) * 0.01f - 10.0f;

		params[i].velocity.x = (rand() % 6000 - 3000) * 0.01f;
		params[i].velocity.y = (rand() % 3000) * 0.01f;
		params[i].velocity.z = (rand() % 6000 - 3000) * 0.01f;
	}

	// 頂点バッファ生成
	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(Point) * 10000;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;
		// サブリソースの設定.
		D3D11_SUBRESOURCE_DATA initData;
		ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
		initData.pSysMem = points.data();
		HRESULT hr = DxSystem::Device->CreateBuffer(&bd, &initData, &vertBuffer);
	}
	// シェーダー読み込み
	particleShader = std::make_unique<Shader>();
	particleShader->Create(L"Assets/shader/Particle.fx","VSMain", "PSMain", "GSMain");
}

void ParticleSystem::Update(){
	// エミット
	emitWork += DeltaTime;
	// 1発分以上の時間がたまっていたら出現させる
	while (emitWork >= (1.0f / emitRate))
	{
		params[currentIndex].isActive = true;
		params[currentIndex].life = 0;
		params[currentIndex].lifetime = 1.0f;

		params[currentIndex].position = Vector3(0, 0, -10);
		params[currentIndex].velocity.x = (rand() % 200 - 100) * 0.01f;
		params[currentIndex].velocity.y = (rand() % 100) * 0.01f;
		params[currentIndex].velocity.z = (rand() % 200 - 100) * 0.01f;

		// 発生待ちを次へ
		currentIndex++;
		if (currentIndex >= 10000) currentIndex = 0;
		emitWork -= (1.0f / emitRate);
	}

	// 全パーティクル更新
	for (int i = 0; i < 10000; i++)
	{
		if (params[i].isActive == false) continue;

		// 寿命処理
		params[i].life += DeltaTime;
		if (params[i].life >= params[i].lifetime)
		{
			params[i].isActive = false;
			points[i].size = 0;
			continue;
		}
		// 寿命に対する割合(発生0.0|----|1.0寿命)
		float liferate = params[i].life / params[i].lifetime;

		// 重力処理
		params[i].velocity += Vector3(0.0f, 0.0f, -5.0f) * DeltaTime;

		// 座標更新
		params[i].position += params[i].velocity * DeltaTime;

		//描画用情報(points)更新
		points[i].x = params[i].position.x;
		points[i].y = params[i].position.y;
		points[i].z = params[i].position.z;
		// サイズ指定
		points[i].size = (sizeEnd - sizeStart) * liferate + sizeStart;
		points[i].tile_w = (1.0f / tile_x);
		points[i].tile_h = (1.0f / tile_y);

		// アニメーション
		// liferate 0.0|-----------|1.0
		// ptn    start|-----------|end
		int ptn = (int)((animEnd - animStart + 1) * liferate + animStart);
		points[i].u = (ptn % tile_x) * (1.0f / tile_x);
		points[i].v = (ptn / tile_x) * (1.0f / tile_y);

		// カラー指定
		points[i].r = 1.0f;
		points[i].g = 1.0f;
		points[i].b = 1.0f;
		points[i].a = 1.0f;
	}
}

void ParticleSystem::Render(){
	// 加算合成
	RenderState::SetBlendState(blend_type);
	// Depth書き込みOFF
	RenderState::DepthEnable(true, false);

	// slot0にテクスチャセット
	texture.Set(0);

	// 頂点バッファ更新
	DxSystem::DeviceContext->UpdateSubresource(vertBuffer, 0, NULL, points.data(), 0, 0);

	// 頂点バッファ設定
	UINT stride = sizeof(Point);
	UINT offset = 0;
	DxSystem::DeviceContext->IASetVertexBuffers(0, 1, &vertBuffer, &stride, &offset);
	DxSystem::DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	// 描画
	particleShader->Activate();
	DxSystem::DeviceContext->Draw(10000, 0);

	// 通常のブレンド
	RenderState::SetBlendState(RenderState::ALPHA);
	// Depth書き込みON
	RenderState::DepthEnable(true, true);
}

void ParticleSystem::SetBlend(RenderState::BlendType blend){
	blend_type = blend;
}

void ParticleSystem::SetSize(float start, float end){
	sizeStart = start;
	sizeEnd = end;
}

void ParticleSystem::SetAnimation(float start, float end){
	animStart = start;
	animEnd = end;
}

void ParticleSystem::SetTexture(const char* filename, int x, int y){
	// テクスチャ読み込み
	texture.Load(filename);
	tile_x = x;
	tile_y = y;
}

void ParticleSystem::Set(float lifetime, Vector3 position, Vector3 velocity){
	params[currentIndex].isActive = true;
	params[currentIndex].life = 0;
	params[currentIndex].lifetime = lifetime;
	params[currentIndex].position = position;
	params[currentIndex].velocity = velocity;

	// 発生待ちを次へ
	currentIndex++;
	if (currentIndex >= 10000) currentIndex = 0;
	emitWork -= (1.0f / emitRate);
}
