#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include "Math/math.h"

struct float2 {
	float2() { x = 0; y = 0; }
	inline float2(float x, float y) { this->x = x, this->y = y; }
	inline float2(const float2& v) { this->x = v.x, this->y = v.y; }
	union {
		struct { float x, y; };
		struct { float r, g; };
	};

	inline float2 operator + () const { float2 ret(x, y); return ret; }
	inline float2 operator - () const { float2 ret(-x, -y); return ret; }

	inline float2 operator + (const float2& v) const { return float2(x + v.x, y + v.y); }
	inline float2 operator - (const float2& v) const { return float2(x - v.x, y - v.y); }
	inline float2 operator * (float v) const { float2 ret(x * v, y * v); return ret; }
	inline float2 operator / (float v) const { float2 ret(x / v, y / v); return ret; }

	inline float2& operator = (float v) { x = v; y = v; return *this; }
	inline float2& operator + (float v) { x += v; y += v; return *this; }
	inline float2& operator + (float2 v) { x += v.x; y += v.y; return *this; }
	inline float2& operator += (float v) { x += v; y += v; return *this; }
	inline float2& operator += (float2 v) { x += v.x; y += v.y; return *this; }
	inline float2& operator - (float v) { x -= v; y -= v; return *this; }
	inline float2& operator - (float2 v) { x -= v.x; y -= v.y; return *this; }
	inline float2& operator -= (float v) { x -= v; y -= v; return *this; }
	inline float2& operator -= (float2 v) { x -= v.x; y -= v.y; return *this; }
	inline float2& operator * (float v) { x *= v; y *= v; return *this; }
	inline float2& operator *= (float v) { x *= v; y *= v; return *this; }
};

struct float3 {
	float3() { x = 0; y = 0; z = 0; }
	inline float3(float x, float y, float z) { this->x = x, this->y = y, this->z = z; }
	inline float3(const float3& v) { this->x = v.x, this->y = v.y, this->z = v.z; }
	union {
		struct { float x, y, z; };
		struct { float r, g, b; };
	};
	float Total() { return r + g + b; }
};

struct float4 {
	float4() { x = 0; y = 0; z = 0; w = 0; }
	inline float4(float x, float y, float z, float w) { this->x = x, this->y = y, this->z = z; this->w = w; }
	inline float4(const float4& v) { this->x = v.x, this->y = v.y, this->z = v.z; this->w = v.w; }
	union {
		struct { float x, y, z, w; };
		struct { float r, g, b, a; };
		float v[4];
	};
};

class Shader
{
public:
	static bool InitializeSystem();
	static void ReleaseSystem();

	static ID3D11Buffer* CreateConstantBuffer(UINT size);

	// モデル用コンスタントバッファ
	struct CBBaseMatrixParam {
		Matrix world;
		Matrix view;
		Matrix proj;
		Matrix wvp;
		float4 ViewPosition;

		float c_x, c_y, c_z;
		float dummy;

		float3 cameraForward;
		float dummy2;
	};

	struct CBColorParam
	{
		float color[4];
	};

	static ID3D11Buffer* CBColor;
	static CBColorParam ColorParam;

	static ID3D11Buffer* CBBaseMatrix;

	static ID3D11SamplerState* SamplerStateWrap;
	static ID3D11SamplerState* SamplerStateClamp;
	static ID3D11SamplerState* SamplerStateBorder;

public:
	Shader() {
		VS = nullptr;
		PS = nullptr;
		GS = nullptr;
		VertexLayout = nullptr;
	}
	~Shader();

	bool Create(WCHAR* filename, LPCSTR VSName, LPCSTR PSName, LPCSTR GSName = NULL);
	void Activate();

	static void SetColor(float r, float g, float b, float a = 1.0f);
	static void SetColor(const float4& col);

protected:
	ID3D11VertexShader* VS; // 頂点シェーダ
	ID3D11PixelShader* PS; // ピクセルシェーダ
	ID3D11GeometryShader* GS; // ジオメトリシェーダー
	ID3D11InputLayout* VertexLayout;

	HRESULT Shader::Compile(WCHAR* filename, LPCSTR method, LPCSTR shaderModel, ID3DBlob** ppBlobOut);

public:
	static CBBaseMatrixParam MatrixParam;
	static void SetTransform(Matrix& transform);
	static void Shader::SetViewMatrix(const Matrix& view) {
		MatrixParam.view = view;
	}

	static void Shader::SetProjectionMatrix(const Matrix& proj) {
		MatrixParam.proj = proj;
	}

};