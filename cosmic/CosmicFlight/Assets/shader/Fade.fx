cbuffer CBTransitionShader : register(b4)
{
    float fadeOut;
    float time;
    float2 dummy;
}

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
    float2 Tex : TEXCOORD0;
};

//--------------------------------------------
//	頂点シェーダー
//--------------------------------------------
VStoPS VSMain(APPtoVS input)
{
    VStoPS output = (VStoPS) 0;

    output.Position = float4(input.Position, 1.0);
    output.Tex = input.Tex;

    return output;
}

//--------------------------------------------
	//ピクセルシェーダー
//--------------------------------------------
float4 PSMain(VStoPS input) : SV_TARGET0
{
    float4 color = BaseColor.Sample(WrapSampler, input.Tex);

    // 画面フェード：fadeOut が 1.0 に近いほど黒くする
    color.rgb = lerp(color.rgb, float3(0.0, 0.0, 0.0), fadeOut);

    return color;
}






