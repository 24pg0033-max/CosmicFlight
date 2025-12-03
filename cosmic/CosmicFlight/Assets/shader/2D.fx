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
    float4 Normal : NORMAL;
    float2 Tex : TEXCOORD;
    float4 Color : COLOR;
};

struct VStoPS
{
    float4 Position : SV_POSITION;
    float2 Tex : TEXCOORD0;
    float4 Color : COLOR;
};

//--------------------------------------------
//	頂点シェーダー
//--------------------------------------------
VStoPS VSMain(APPtoVS input)
{
    VStoPS output = (VStoPS) 0;
	//	スクリーン座標変換
    float4 P = float4(input.Position, 1.0);
    output.Position = P;
    output.Tex = input.Tex;
    output.Color = input.Color;
    return output;
}

//--------------------------------------------
//	ピクセルシェーダー
//--------------------------------------------
float4 PSBase(VStoPS input) : SV_TARGET0
{
    float4 color = input.Color;
    return color;
}

//--------------------------------------------
//	ピクセルシェーダー
//--------------------------------------------
float4 PSTex(VStoPS input) : SV_TARGET0
{
    float4 tex = BaseColor.Sample(WrapSampler, input.Tex);
    float4 color = tex;
    
    color.rgb *= input.Color.rgb;
    color.a *= input.Color.a;
    return color;
}
//float4 PSTex2(VStoPS input) : SV_TARGET0
//{
//    float4 color = input.Color;
    
//    float2 center = float2(0.5, 0.5);
    
//    float len = length(center - input.Tex);
//    len *= 2;
//    //len = pow(len, 2);
//    color.rgb = lerp(float3(1, 1, 1), float3(0.8, 0, 0.4), len);
//    color.a = color.g;
    
//    return color;
//}
