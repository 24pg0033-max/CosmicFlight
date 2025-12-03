#include "DxSystem.h"
#include "DXShader.h"
#include <cassert>
//****************************************************************
//
//	シェーダーシステム初期化・解放
//
//****************************************************************
ID3D11SamplerState* Shader::SamplerStateWrap = nullptr;
ID3D11SamplerState* Shader::SamplerStateClamp = nullptr;
ID3D11SamplerState* Shader::SamplerStateBorder = nullptr;
ID3D11Buffer* Shader::CBBaseMatrix = nullptr;
ID3D11Buffer* Shader::CBColor = nullptr;
Shader::CBColorParam Shader::ColorParam = { {1.0f, 1.0f, 1.0f, 1.0f} };
bool Shader::InitializeSystem()
{
	HRESULT hr;
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	// Slot0 Wrap
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	hr = DxSystem::Device->CreateSamplerState(&sampDesc, &SamplerStateWrap);
	if (FAILED(hr)) return false;
	DxSystem::DeviceContext->PSSetSamplers(0, 1, &SamplerStateWrap);
	DxSystem::DeviceContext->VSSetSamplers(0, 1, &SamplerStateWrap);
	// Slot1 Clamp
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	hr = DxSystem::Device->CreateSamplerState(&sampDesc, &SamplerStateClamp);
	if (FAILED(hr)) return false;
	DxSystem::DeviceContext->PSSetSamplers(1, 1, &SamplerStateClamp);
	DxSystem::DeviceContext->VSSetSamplers(1, 1, &SamplerStateClamp);
	// Slot2 Border
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.BorderColor[0] = D3D11_FLOAT32_MAX;
	sampDesc.BorderColor[1] = D3D11_FLOAT32_MAX;
	sampDesc.BorderColor[2] = D3D11_FLOAT32_MAX;
	sampDesc.BorderColor[3] = D3D11_FLOAT32_MAX;
	hr = DxSystem::Device->CreateSamplerState(&sampDesc, &SamplerStateBorder);
	if (FAILED(hr)) return false;
	DxSystem::DeviceContext->PSSetSamplers(2, 1, &SamplerStateBorder);
	DxSystem::DeviceContext->VSSetSamplers(2, 1, &SamplerStateBorder);
	// 基本コンスタントバッファ生成
	CBBaseMatrix = CreateConstantBuffer(sizeof(CBBaseMatrixParam));
	CBColor = CreateConstantBuffer(sizeof(CBColorParam));
	return true;
}
void Shader::ReleaseSystem()
{
	SamplerStateWrap->Release();
	SamplerStateClamp->Release();
	SamplerStateBorder->Release();
	CBBaseMatrix->Release();
	CBColor->Release();
}
ID3D11Buffer* Shader::CreateConstantBuffer(UINT size)
{
	ID3D11Buffer* cb;
	// 定数バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
	bd.ByteWidth = size;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	DxSystem::Device->CreateBuffer(&bd, NULL, &cb);
	return cb;
}
//****************************************************************
//
//
//
//****************************************************************
Shader::~Shader()
{
	if (VertexLayout) { VertexLayout->Release(); VertexLayout = NULL; }
	if (VS) { VS->Release(); VS = NULL; }
	if (PS) { PS->Release(); PS = NULL; }
	if (GS) { GS->Release(); GS = NULL; }
}
//------------------------------------------------
//	シェーダー単体コンパイル
//------------------------------------------------
HRESULT Shader::Compile(WCHAR* filename, LPCSTR method, LPCSTR shaderModel, ID3DBlob** blobOut)
{
	DWORD ShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
	//ShaderFlags |= D3DCOMPILE_DEBUG;
	//ShaderFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
	ID3DBlob* BlobError = NULL;
	// コンパイル
	HRESULT hr = D3DCompileFromFile(
		filename,
		NULL,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		method,
		shaderModel,
		ShaderFlags,
		0,
		blobOut,
		&BlobError
	);
	// エラー出力
	if (BlobError != NULL)
	{
		OutputDebugStringA("\n\n\n----------------------------\n");
		OutputDebugStringA((char*)BlobError->GetBufferPointer());
		OutputDebugStringA("----------------------------\n\n\n");
		BlobError->Release();
		BlobError = NULL;
		//assert(false);
	}
	return hr;
}
//------------------------------------------------
//	シェーダーセットコンパイル
//------------------------------------------------
bool Shader::Create(WCHAR* filename, LPCSTR VSFunc, LPCSTR PSFunc, LPCSTR GSName)
{
	if (VS) { VS->Release(); VS = nullptr; }
	if (PS) { PS->Release(); PS = nullptr; }
	if (GS) { GS->Release(); GS = nullptr; }

	if (VertexLayout) { VertexLayout->Release(); VertexLayout = nullptr; }

	HRESULT hr = S_OK;
	// ジオメトリシェーダ
	GS = NULL;
	ID3DBlob* GSBlob = NULL;
	if (GSName != NULL) {
		hr = Compile(filename, GSName, "gs_5_0", &GSBlob);
		if (FAILED(hr))
		{
			return false;
		}
		// ジオメトリシェーダ生成
		hr = DxSystem::Device->CreateGeometryShader(
			GSBlob->GetBufferPointer(), GSBlob->GetBufferSize(), NULL, &GS);
		if (FAILED(hr))
		{
			GSBlob->Release();
			return false;
		}
	}
	// 頂点シェーダ
	ID3DBlob* VSBlob = NULL;
	hr = Compile(filename, VSFunc, "vs_5_0", &VSBlob);
	if (FAILED(hr))
	{
		return false;
	}
	// 頂点シェーダ生成
	hr = DxSystem::Device->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), NULL, &VS);
	if (FAILED(hr))
	{
		VSBlob->Release();
		return false;
	}
	// 入力レイアウト
	D3D11_INPUT_ELEMENT_DESC layout3D[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 4 * 3, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 4 * 6, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 4 * 8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
						4 * 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
						4 * 15, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE(layout3D);
	// 入力レイアウト生成
	hr = DxSystem::Device->CreateInputLayout(
		layout3D,
		numElements,
		VSBlob->GetBufferPointer(),
		VSBlob->GetBufferSize(),
		&VertexLayout
	);
	VSBlob->Release();
	if (FAILED(hr))
	{
		return false;
	}
	// 入力レイアウト設定
	DxSystem::DeviceContext->IASetInputLayout(VertexLayout);
	// ピクセルシェーダ
	ID3DBlob* PSBlob = NULL;
	hr = Compile(filename, PSFunc, "ps_5_0", &PSBlob);
	if (FAILED(hr))
	{
		return false;
	}
	// ピクセルシェーダ生成
	hr = DxSystem::Device->CreatePixelShader(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), NULL, &PS);
	PSBlob->Release();
	if (FAILED(hr))
	{
		return false;
	}
	return true;
}
//****************************************************************
//
//
//
//****************************************************************
//------------------------------------------------
//	有効化
//------------------------------------------------
void Shader::Activate()
{
	// 入力レイアウト設定
	DxSystem::DeviceContext->IASetInputLayout(VertexLayout);
	DxSystem::DeviceContext->VSSetShader(VS, NULL, 0);
	DxSystem::DeviceContext->PSSetShader(PS, NULL, 0);
	DxSystem::DeviceContext->GSSetShader(GS, NULL, 0);
}
//------------------------------------------------
//	行列情報更新
//-----------------------------------MatrixParam-------------
Shader::CBBaseMatrixParam Shader::MatrixParam;
//void Shader::SetTransform(Matrix& transform)
//{
//	MatrixParam.view = DxSystem::ViewMatrix;
//	MatrixParam.proj = DxSystem::ProjectionMatrix;
//	Matrix MatVP = MatrixParam.view * MatrixParam.proj;
//	MatrixParam.world = transform;
//	MatrixParam.wvp = MatrixParam.world * MatVP;
//	DxSystem::DeviceContext->UpdateSubresource(Shader::CBBaseMatrix, 0, NULL, &MatrixParam, 0, 0);
//}

void Shader::SetTransform(Matrix& transform)
{
	MatrixParam.view = DxSystem::ViewMatrix;
	MatrixParam.proj = DxSystem::ProjectionMatrix;
	Matrix MatVP = MatrixParam.view * MatrixParam.proj;

	// カメラ座標セット
	Matrix v = DxSystem::ViewMatrix;
	v.inverse(); // カメラ(逆行列)の逆行列 = 普通の行列
	MatrixParam.c_x = v._41;
	MatrixParam.c_y = v._42;
	MatrixParam.c_z = v._43;

	MatrixParam.cameraForward.x = v._31;
	MatrixParam.cameraForward.y = v._32;
	MatrixParam.cameraForward.z = v._33;

	MatrixParam.world = transform;
	MatrixParam.wvp = MatrixParam.world * MatVP;
	DxSystem::DeviceContext->UpdateSubresource(Shader::CBBaseMatrix, 0, NULL, &MatrixParam, 0, 0);
}

void Shader::SetColor(float r, float g, float b, float a)
{
	ColorParam.color[0] = r;
	ColorParam.color[1] = g;
	ColorParam.color[2] = b;
	ColorParam.color[3] = a;

	DxSystem::DeviceContext->UpdateSubresource(Shader::CBColor, 0, NULL, &ColorParam, 0, 0);

	DxSystem::DeviceContext->PSSetConstantBuffers(1, 1, &CBColor);
}

void Shader::SetColor(const float4& col)
{
	SetColor(col.x, col.y, col.z, col.w);
}
