#pragma once
#include <d3d11.h>
#include "DXShader.h"

class RenderTarget
{
public:
	ID3D11ShaderResourceView* GetSRV() const {
		return ShaderResourceView;
	}
	ID3D11ShaderResourceView* GetGBufferSRV() const {
		return GBufferShaderResourceView;
	}
public:
	int Width, Height;

	RenderTarget();
	~RenderTarget();
	void Initialize(int width = 0, int height = 0, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM);
	void SetupGBuffer(DXGI_FORMAT format = DXGI_FORMAT_R16G16B16A16_FLOAT);
	void Activate();
	void Deactivate();
	void Render();
	void Set(int slot);

protected:
	ID3D11Texture2D* Texture;
	ID3D11Texture2D* GBufferTexture;			// 深度や法線の情報を保持するバッファ
	ID3D11Texture2D* Depth;
	ID3D11RenderTargetView* RenderTargetView;
	ID3D11RenderTargetView* GBufferTargetView;
	ID3D11ShaderResourceView* ShaderResourceView;
	ID3D11ShaderResourceView* GBufferShaderResourceView;
	ID3D11DepthStencilView* DepthStencilView;

	struct Vertex {
		float x, y, z;		// 座標
		float nx, ny, nz;	// 法線
		float tu, tv;		// UV
		float r, g, b, a;	// 頂点カラー
	};
	ID3D11Buffer *VertexBuffer;
};

