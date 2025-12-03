#pragma once

#include <d3d11.h>
#include "Math/math.h"

class DxSystem {
public:
	static int ScreenWidth;
	static int ScreenHeight;
	static ID3D11RenderTargetView* RenderTargetView;
	static ID3D11DepthStencilView* DepthStencilView;
private:
	static IDXGISwapChain*			SwapChain;

	static ID3D11Texture2D*				DepthStencilTexture;
	static ID3D11ShaderResourceView*	ShaderResourceView;

	static HRESULT CreateDevice(HWND hWnd);
	static bool CreateDepthStencil();
	static bool InitializeRenderTarget();

public:
	static ID3D11Device*			Device;
	static ID3D11DeviceContext*		DeviceContext;

	static bool Initialize(HWND hWnd, int width, int height);
	static void Release();
	static void Clear();
	static void Flip();

	static Matrix ViewMatrix;
	static Matrix ProjectionMatrix;
};
