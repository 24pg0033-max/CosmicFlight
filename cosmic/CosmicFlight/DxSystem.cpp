#include "DxSystem.h"

#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "d3dcompiler.lib" )

ID3D11Device*			DxSystem::Device;
IDXGISwapChain*			DxSystem::SwapChain;
ID3D11DeviceContext*	DxSystem::DeviceContext;
ID3D11RenderTargetView* DxSystem::RenderTargetView;

ID3D11Texture2D*            DxSystem::DepthStencilTexture;
ID3D11DepthStencilView*     DxSystem::DepthStencilView;
ID3D11ShaderResourceView*   DxSystem::ShaderResourceView;

int DxSystem::ScreenWidth = 1280;
int DxSystem::ScreenHeight = 720;

Matrix DxSystem::ViewMatrix;
Matrix DxSystem::ProjectionMatrix;

//****************************************************************
//
//	初期化
//
//****************************************************************
bool DxSystem::Initialize(HWND hWnd, int width, int height)
{
	CreateDevice(hWnd);
	InitializeRenderTarget();

	D3D11_RASTERIZER_DESC rasterizerDesc;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.FrontCounterClockwise = true;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.SlopeScaledDepthBias = 0;
	rasterizerDesc.DepthClipEnable = true;
	rasterizerDesc.ScissorEnable = false;
	rasterizerDesc.MultisampleEnable = false;
	rasterizerDesc.AntialiasedLineEnable = false;
	rasterizerDesc.DepthBiasClamp = 0;
	rasterizerDesc.SlopeScaledDepthBias = 0;

	ID3D11RasterizerState * pState;
	Device->CreateRasterizerState(&rasterizerDesc, &pState);
	DeviceContext->RSSetState(pState);
	pState->Release();
	return true;
}

//****************************************************************
//
//	デバイス生成
//
//****************************************************************
HRESULT DxSystem::CreateDevice( HWND hWnd )
{
	HRESULT hr = S_OK;

	UINT createDeviceFlags = 0;
	//createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;

	// 機能レベル
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	UINT numFeatureLevels = sizeof(featureLevels) / sizeof(featureLevels[0]);

	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_10_0;

	// スワップチェインの設定
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = ScreenWidth;
	sd.BufferDesc.Height = ScreenHeight;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	// デバイス生成
	hr = D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		createDeviceFlags,
		featureLevels,
		numFeatureLevels,
		D3D11_SDK_VERSION,
		&sd,
		&SwapChain,
		&Device,
		&featureLevel,
		&DeviceContext);
	
	if (FAILED(hr))
	{
		return hr;
	}

	return S_OK;
}

void DxSystem::Release()
{
	if (DeviceContext) { DeviceContext->ClearState(); }
	if (RenderTargetView) { RenderTargetView->Release(); RenderTargetView = NULL; }
	if (SwapChain) { SwapChain->Release(); SwapChain = NULL; }
	if (DeviceContext) { DeviceContext->Release(); DeviceContext = NULL; }
	if (Device) { Device->Release(); Device = NULL; }
}

//****************************************************************
//
//	レンダーターゲット関連
//
//****************************************************************
//------------------------------------------------
//	初期化
//------------------------------------------------
bool DxSystem::InitializeRenderTarget()
{
	// バックバッファ取得
	ID3D11Texture2D* BackBuffer = NULL;
	HRESULT hr = SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&BackBuffer);
	if (FAILED(hr))
	{
		return false;
	}

	// レンダーターゲットビュー生成
	hr = Device->CreateRenderTargetView(BackBuffer, NULL, &RenderTargetView);
	BackBuffer->Release();
	BackBuffer = NULL;
	if (FAILED(hr))
	{
		return false;
	}

	CreateDepthStencil();

	// レンダーターゲットビュー設定
	DeviceContext->OMSetRenderTargets(1, &RenderTargetView, DepthStencilView);

	// ビューポート設定
	D3D11_VIEWPORT vp;
	vp.Width = (float)ScreenWidth;
	vp.Height = (float)ScreenHeight;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	DeviceContext->RSSetViewports(1, &vp);

	return true;
}

//------------------------------------------------
//      深度ステンシルバッファ生成
//------------------------------------------------
bool DxSystem::CreateDepthStencil()
{
	// 深度ステンシル設定
	D3D11_TEXTURE2D_DESC td;
	ZeroMemory(&td, sizeof(D3D11_TEXTURE2D_DESC));
	td.Width = ScreenWidth;
	td.Height = ScreenHeight;
	td.MipLevels = 1;
	td.ArraySize = 1;
	td.Format = DXGI_FORMAT_R24G8_TYPELESS;
	td.SampleDesc.Count = 1;
	td.SampleDesc.Quality = 0;
	td.Usage = D3D11_USAGE_DEFAULT;
	td.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	td.CPUAccessFlags = 0;
	td.MiscFlags = 0;

	// 深度ステンシルテクスチャ生成
	HRESULT hr = Device->CreateTexture2D(&td, NULL, &DepthStencilTexture);
	if (FAILED(hr))
	{
		return false;
	}

	// 深度ステンシルビュー設定
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
	ZeroMemory(&dsvd, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	dsvd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvd.Texture2D.MipSlice = 0;

	// 深度ステンシルビュー生成
	hr = Device->CreateDepthStencilView(DepthStencilTexture, &dsvd, &DepthStencilView);
	if (FAILED(hr))
	{
		return false;
	}

	// シェーダリソースビュー設定
	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	ZeroMemory(&srvd, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvd.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;

	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MostDetailedMip = 0;
	srvd.Texture2D.MipLevels = 1;

	// シェーダリソースビュー生成
	hr = Device->CreateShaderResourceView(DepthStencilTexture, &srvd, &ShaderResourceView);
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}


//------------------------------------------------
//	クリア
//------------------------------------------------
void DxSystem::Clear()
{
	float clearColor[4] = { 0, 0, 0, 1.0f };
	DeviceContext->ClearRenderTargetView(RenderTargetView, clearColor);
	DeviceContext->ClearDepthStencilView(DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

//------------------------------------------------
//	フリップ
//------------------------------------------------
void DxSystem::Flip()
{
	// フリップ処理
	SwapChain->Present(1, 0);
}
