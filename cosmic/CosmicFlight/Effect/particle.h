#pragma once
#include "../DxSystem.h"
#include "../DxShader.h"
#include "../Texture.h"
#include "../RenderState.h"

class ParticleSystem{
public:
	struct Point {
		float x, y, z;		// 座標
		float size, tile_w, tile_h;
		float u, v;			// テクスチャ座標
		float r, g, b, a;	// カラー
	};

	struct Param {
		bool isActive;
		float life;			// 生存時間
		float lifetime;		// 寿命
		Vector3 position;	// 位置
		Vector3 velocity;	// 速度
	};

	void Initialize();
	void Update();
	void Render();

	void SetTexture(const char* filename, int x, int y);
	void Set(float lifetime, Vector3 position, Vector3 velocity);
	void SetBlend(RenderState::BlendType blend);
	void SetBlendAdd() { SetBlend(RenderState::ADD); }
	void SetBlendAlpha() { SetBlend(RenderState::ALPHA); }
	void SetSize(float start, float end);
	void SetAnimation(float start, float end);

private:
	std::unique_ptr<Shader> particleShader;
	Texture texture;
	int tile_x;
	int tile_y;

	std::vector<Point> points;
	std::vector<Param> params;

	const int maxParticles = 10000;

	ID3D11Buffer* vertBuffer;	// 頂点バッファ

	int currentIndex = 0;		// 現在の発射インデックス
	float emitRate;				// 1秒当たりの発射数
	float emitWork;				// 発射用カウンタ

	float animStart;
	float animEnd;

	float sizeStart;
	float sizeEnd;

	RenderState::BlendType blend_type;
};
