//Assets/shadowmap.fx
// プログラム側から受け取る情報
cbuffer CBBaseMatrix : register(b0)
{
    matrix World;
    matrix View;
    matrix Proj;
    matrix matWVP;
};

cbuffer CBShadowMap : register(b10)
{
	matrix LightViewProj;
};


//	テクスチャ
Texture2D BaseColor : register(t0);

// テクスチャサンプラー
SamplerState WrapSampler : register(s0);
SamplerState ClampSampler : register(s1);
SamplerState BorderSampler : register(s2);

//	入力頂点
struct APPtoVS
{
    float3 Position : POSITION;
    float2 Tex : TEXCOORD;
};

struct VStoPS
{
    float4 Position : SV_POSITION;
    float Distance : TEXCOORD0;
    //float2 Tex : TEXCOORD0;
};

//--------------------------------------------
//	頂点シェーダー
//--------------------------------------------
VStoPS VSMain(APPtoVS input)
{
	VStoPS output = (VStoPS) 0;

	float4 P = float4(input.Position, 1.0);
	// 光からの行列で変換
	P = mul(World, P);
	output.Position = mul(LightViewProj, P);

	output.Distance = output.Position.z; //画面のZ座標≒距離
    //output.Tex = input.Tex;

	return output;
}

//--------------------------------------------
//	ピクセルシェーダー
//--------------------------------------------
float4 PSMain(VStoPS input) : SV_TARGET0
{
	float d = input.Distance;
	return float4(d, d, d, 1.0); // 距離を不透明で描画
}

