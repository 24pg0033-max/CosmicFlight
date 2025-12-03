// プログラム側から受け取る情報
cbuffer CBBaseMatrix : register(b0)
{
	matrix  World;
	matrix	View;
	matrix	Proj;
	matrix	matWVP;
};

//	テクスチャ
Texture2D DiffuseTexture : register(t0);

// テクスチャサンプラー
SamplerState WrapSampler : register(s0);
SamplerState ClampSampler : register(s1);
SamplerState BorderSampler : register(s2);


//	入力頂点
struct APPtoVS {
	float3 Position : POSITION;
	float2 Tex      : TEXCOORD;
	float3 Normal   : NORMAL;
};

struct VStoPS
{
	float4 Position : SV_POSITION;
	float2 Tex : TEXCOORD0;
	float3 Normal : NORMAL;
	float3 WorldPosition : TEXCOORD1;
};

//--------------------------------------------
//	頂点シェーダー
//--------------------------------------------
VStoPS VSMain(APPtoVS input)
{
	VStoPS output = (VStoPS)0;

	float4 P = float4(input.Position, 1.0);
	output.Position = mul(matWVP, P);
	output.Tex = input.Tex;

	// 法線
	float4 N = float4(input.Normal, 0);
	output.Normal = mul(World, N).xyz;

	// World座標
	output.WorldPosition = mul(World, P).xyz;


	return output;
}

//--------------------------------------------
//	ピクセルシェーダー
//--------------------------------------------
float4 PSMain(VStoPS input) : SV_TARGET0
{
    float4 tex = DiffuseTexture.Sample(WrapSampler, input.Tex);

    // 透明度調整
    tex.a *= 0.1;

    // 黒除去用にRGBが小さいなら透明にする
    if (tex.r + tex.g + tex.b < 0.05)
    {
        discard;
    }

    return tex;
}

