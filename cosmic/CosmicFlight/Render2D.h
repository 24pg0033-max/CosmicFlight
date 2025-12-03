#pragma once
#include <d3d11.h>
#include "Math/math.h"
#include "Texture.h"
#include "DxShader.h"
#include "Easing/Easing.h"
class Render2D
{
public:
	Render2D();
	~Render2D();
	void Initialize(const char* filename, Shader* shader = nullptr);
	void Render();
public:
	void SetPosition(const Vector3& pos);
	//void SetPosition(float x, float y);;
	void SetRotation(float rot) { rotation = rot; }
	void SetPivot(float x, float y);
	void SetSize(float width, float height);
	void SetUV(float u, float v, float w, float h);
	void SetColor(float r, float g, float b, float a = 1.0f);
	void SetAlpha(float alpha) { a = alpha; }

protected:
	void UpdateVertices();//頂点更新
public:
	static void Initialize();
protected:
	struct CBParam {
		Matrix proj;
	};
	//	描画用情報
	//	頂点構造体（シェーダーと一致）
	struct Vertex {
		float x, y, z;		// 座標
		float nx, ny, nz;	// 法線
		float tu, tv;		// UV
		float r, g, b, a;	// 頂点カラー
	};
	Vector3 position;		//座標
	float rotation;			//回転
	float width, height;	//画像サイズ
	float u, v, w, h;		//UV値
	float r, g, b, a;		//RGBA
	float cx, cy;           //センター
	Shader* shader;
	Texture* texture;
	Vertex* vertices;		//頂点座標
	ID3D11Buffer* VertexBuffer;//頂点バッファ
	ID3D11Buffer* IndexBuffer;//頂点バッファ
	static Shader baseShader;	//ベースシェーダー
	static ID3D11Buffer* ConstantBuffer;//コンスタントバッファ
	
};