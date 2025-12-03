//RenderTarget.cpp
#include "DxSystem.h"
#include "RenderTarget.h"

RenderTarget::RenderTarget()
	: GBufferTexture(nullptr)
	, GBufferTargetView(nullptr)
	, GBufferShaderResourceView(nullptr)
{
}

RenderTarget::~RenderTarget()
{
	if (Texture) { Texture->Release(); Texture = nullptr; }
	if (Depth) { Depth->Release(); Depth = nullptr; }
	if (RenderTargetView) { RenderTargetView->Release(); RenderTargetView = nullptr; }
	if (ShaderResourceView) { ShaderResourceView->Release(); ShaderResourceView = nullptr; }
	if (DepthStencilView) { DepthStencilView->Release(); DepthStencilView = nullptr; }

	if (GBufferTexture) { GBufferTexture->Release(); GBufferTexture = nullptr; }
	if (GBufferTargetView) { GBufferTargetView->Release(); GBufferTargetView = nullptr; }
	if (GBufferShaderResourceView) { GBufferShaderResourceView->Release(); GBufferShaderResourceView = nullptr; }

	if (VertexBuffer) { VertexBuffer->Release(); VertexBuffer = nullptr; }
}
void RenderTarget::Initialize(int width, int height, DXGI_FORMAT format)
{
	Width = width;
	Height = height;
	if (Width <= 0) Width = DxSystem::ScreenWidth;
	if (Height <= 0) Height = DxSystem::ScreenHeight;

	ID3D11Device* Device = DxSystem::Device;

	// 未使用の法線にサイズを入れる
	float x = 1.0f / Width;
	float y = 1.0f / Height;

	Vertex v[4] = {
		{  1, 1,0, x,y,1, 1,0 },
		{ -1, 1,0, x,y,1, 0,0 },
		{  1,-1,0, x,y,1, 1,1 },
		{ -1,-1,0, x,y,1, 0,1 }
	};
	// 頂点バッファの生成.
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(Vertex) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
	initData.pSysMem = v;
	Device->CreateBuffer(&bd, &initData, &VertexBuffer);

	{
		// レンダーターゲット設定
		D3D11_TEXTURE2D_DESC td;
		ZeroMemory(&td, sizeof(D3D11_TEXTURE2D_DESC));
		td.Width = Width;
		td.Height = Height;
		td.MipLevels = 1;
		td.ArraySize = 1;
		td.Format = format;
		td.SampleDesc.Count = 1;
		td.SampleDesc.Quality = 0;
		td.Usage = D3D11_USAGE_DEFAULT;
		td.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		td.CPUAccessFlags = 0;
		td.MiscFlags = 0;
		// テクスチャ生成
		HRESULT hr = Device->CreateTexture2D(&td, NULL, &Texture);
		if (FAILED(hr)) return;

		//	レンダーターゲットビュー
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
		memset(&rtvDesc, 0, sizeof(rtvDesc));
		rtvDesc.Format = format;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		hr = Device->CreateRenderTargetView(Texture, &rtvDesc, &RenderTargetView);

		// シェーダリソースビューの生成
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		memset(&srvDesc, 0, sizeof(srvDesc));
		srvDesc.Format = rtvDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		hr = Device->CreateShaderResourceView(Texture, &srvDesc, &ShaderResourceView);
		if (FAILED(hr)) return;
	}


	// 深度ステンシル設定
	{
		// 深度ステンシル設定
		D3D11_TEXTURE2D_DESC td;
		ZeroMemory(&td, sizeof(D3D11_TEXTURE2D_DESC));
		td.Width = Width;
		td.Height = Height;
		td.MipLevels = 1;
		td.ArraySize = 1;
		td.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		td.SampleDesc.Count = 1;
		td.SampleDesc.Quality = 0;
		td.Usage = D3D11_USAGE_DEFAULT;
		td.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		td.CPUAccessFlags = 0;
		td.MiscFlags = 0;

		// 深度ステンシルテクスチャ生成
		HRESULT hr = Device->CreateTexture2D(&td, NULL, &Depth);
		if (FAILED(hr)) return;

		// 深度ステンシルビュー設定
		D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
		ZeroMemory(&dsvd, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
		dsvd.Format = td.Format;
		dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsvd.Texture2D.MipSlice = 0;

		// 深度ステンシルビュー生成
		hr = Device->CreateDepthStencilView(Depth, &dsvd, &DepthStencilView);
		if (FAILED(hr)) return;
	}

	SetupGBuffer();
}

void RenderTarget::SetupGBuffer(DXGI_FORMAT format)
{
	ID3D11Device* Device = DxSystem::Device;

	{
		// レンダーターゲット設定
		D3D11_TEXTURE2D_DESC td;
		ZeroMemory(&td, sizeof(D3D11_TEXTURE2D_DESC));
		td.Width = Width;
		td.Height = Height;
		td.MipLevels = 1;
		td.ArraySize = 1;
		td.Format = format;
		td.SampleDesc.Count = 1;
		td.SampleDesc.Quality = 0;
		td.Usage = D3D11_USAGE_DEFAULT;
		td.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		td.CPUAccessFlags = 0;
		td.MiscFlags = 0;
		// テクスチャ生成
		HRESULT hr = Device->CreateTexture2D(&td, NULL, &GBufferTexture);
		if (FAILED(hr)) return;

		//	レンダーターゲットビュー
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
		memset(&rtvDesc, 0, sizeof(rtvDesc));
		rtvDesc.Format = format;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		hr = Device->CreateRenderTargetView(GBufferTexture, &rtvDesc, &GBufferTargetView);

		// シェーダリソースビューの生成
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		memset(&srvDesc, 0, sizeof(srvDesc));
		srvDesc.Format = rtvDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		hr = Device->CreateShaderResourceView(GBufferTexture, &srvDesc, &GBufferShaderResourceView);
		if (FAILED(hr)) return;
	}
}

void RenderTarget::Activate()
{
	// GBufferがある場合は2枚をターゲットとしてセットする
	if (GBufferTargetView) {
		ID3D11RenderTargetView* targets[] = {
			RenderTargetView,
			GBufferTargetView
		};

		// レンダーターゲットビュー設定
		DxSystem::DeviceContext->OMSetRenderTargets(2, targets, DepthStencilView);
		// GBufferのクリア
		float clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		DxSystem::DeviceContext->ClearRenderTargetView(GBufferTargetView, clearColor);
	} else {
		ID3D11RenderTargetView* targets[] = {
			RenderTargetView,
		};

		// レンダーターゲットビュー設定
		DxSystem::DeviceContext->OMSetRenderTargets(1, targets, DepthStencilView);
	}
	float clearColor[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
	DxSystem::DeviceContext->ClearRenderTargetView(RenderTargetView, clearColor);
	DxSystem::DeviceContext->ClearDepthStencilView(DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// ビューポート設定
	D3D11_VIEWPORT vp = { 0,0,(float)Width,(float)Height,0,1 };
	DxSystem::DeviceContext->RSSetViewports(1, &vp);
}

void RenderTarget::Deactivate()
{
	DxSystem::DeviceContext->OMSetRenderTargets(1, &DxSystem::RenderTargetView, DxSystem::DepthStencilView);
}

void RenderTarget::Render()
{
	DxSystem::DeviceContext->PSSetShaderResources(0, 1, &ShaderResourceView);


	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	DxSystem::DeviceContext->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);
	DxSystem::DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	DxSystem::DeviceContext->Draw(4, 0);
	DxSystem::DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void RenderTarget::Set(int slot)
{
	DxSystem::DeviceContext->PSSetShaderResources(slot, 1, &ShaderResourceView);
}