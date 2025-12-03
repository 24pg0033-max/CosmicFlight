// Planet.fx
// プログラム側から受け取る情報
cbuffer CBBaseMatrix : register(b0)
{
    matrix World;
    matrix View;
    matrix Proj;
    matrix matWVP;
};

cbuffer CBPlanetParam : register(b6)
{
    float3 CameraPos; // カメラワールド位置
    float RimPower; // リムシャープさ
    float RimIntensity; // リムの明るさ倍率
    float3 RimColor; // リムの色
};

//	テクスチャ
Texture2D PlanetTex : register(t0);

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
//VStoPS VSMain(APPtoVS input)
//{
//    VStoPS output = (VStoPS) 0;

//    // ワールド座標変換
//    float4 worldPos = mul(float4(input.Position, 1.0f), World);
//    output.WorldPosition = worldPos.xyz;

//    // 法線をワールド空間に変換
//    output.Normal = normalize(mul((float3x3) World, input.Normal));

//    // ビュー・プロジェクション行列でクリップ座標へ
//    float4 viewPos = mul(worldPos, View);
//    output.Position = mul(viewPos, Proj);

//    // UVそのまま
//    output.Tex = input.Tex;

//    return output;
//}

VStoPS VSMain(APPtoVS input)
{
    VStoPS output = (VStoPS) 0;

    float4 P = float4(input.Position, 1.0);
    output.Position = mul(matWVP, P);
    output.Tex = input.Tex;
	// 法線変換
    float4 N = float4(input.Normal, 0.0);
    output.Normal = mul(World, N).xyz;

    return output;
}

//--------------------------------------------
//	ピクセルシェーダー
//--------------------------------------------
float4 PSMain(VStoPS input) : SV_TARGET
{
    // 法線・視線方向を正規化
    float3 normal = normalize(input.Normal);
    //float3 viewDir = normalize(CameraPos - input.WorldPosition);
    float3 viewDir = float3(View._13, View._23, View._33);

    // リムライト計算（縁が明るくなる）
    float rim = 1.0 - saturate(dot(viewDir, normal));
    float rimGlow = pow(rim, RimPower);  // RimPower を大きくすると鋭くなる

    // テクスチャカラー取得
    float3 baseColor = PlanetTex.Sample(WrapSampler, input.Tex).rgb;

    // リムライトカラー
    float3 rimColor = RimColor * rimGlow * RimIntensity;

    // 合成
    float3 finalColor = saturate(baseColor + rimColor);

    return float4(finalColor, 1.0f);
}