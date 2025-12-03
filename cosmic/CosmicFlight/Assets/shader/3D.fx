////Assets/3D.fx
// プログラム側から受け取る情報
cbuffer CBBaseMatrix : register(b0)
{
	matrix World;
	matrix View;
	matrix Proj;
	matrix matWVP;
    
    float3 ViewPosition;
};

//	テクスチャ
Texture2D BaseColor : register(t0);
Texture2D NormalMap : register(t6);

// テクスチャサンプラー
SamplerState WrapSampler : register(s0);
SamplerState ClampSampler : register(s1);
SamplerState BorderSampler : register(s2);

//	入力頂点
struct APPtoVS 
{
    float3 Position : POSITION;
    float2 Tex : TEXCOORD;
    float3 Normal : NORMAL; // 法線
    float3 Tangent : TANGENT; // 接線
    float3 Binormal : BINORMAL; // 従法線
};

struct VStoPS
{
    float4 Position : SV_POSITION;
    float2 Tex : TEXCOORD0;
    float3 Normal : NORMAL; // 法線
    float3 Tangent : TANGENT; // 接線
    float3 Binormal : BINORMAL; // 従法線

    float3 WorldPosition : TEXCOORD1; // ワールド座標
    float4 ScreenPosition : TEXCOORD2; // スクリーン座標

    float3 NormalInView : TEXCOORD3;
};

//--------------------------------------------
//	頂点シェーダー
//--------------------------------------------
VStoPS VSMain(APPtoVS input)
{
    VStoPS output = (VStoPS) 0;

    float4 P = float4(input.Position, 1.0);
    output.Position = mul(matWVP, P);
    output.Tex = input.Tex;
	// ワールド座標
    output.WorldPosition = mul(World, P).xyz;
	// スクリーン座標
    output.ScreenPosition = output.Position;
    
	// 法線変換
    float4 N = float4(input.Normal, 0.0);
    output.Normal = mul(World, N).xyz;
	// 接線・従法線
    float4 T = float4(input.Tangent, 0.0);
    output.Tangent = mul(World, T).xyz;
    float4 B = float4(-input.Binormal, 0.0);
    output.Binormal = mul(World, B).xyz;
	
    //output.NormalInView = mul(View, N);
	
    return output;
}

//--------------------------------------------
//	ピクセルシェーダー
//--------------------------------------------
// レンダーターゲットへの出力
struct PS_OUTPUT
{
    float4 Color : SV_Target0;
    float4 GBuffer : SV_Target1;
};

PS_OUTPUT PSMain(VStoPS input) : SV_TARGET0
{
    float4 tex = BaseColor.Sample(WrapSampler, input.Tex);
    float4 color = tex;
	// 法線
    float3 N = normalize(input.Normal); //受け取った法線正規化(長さ=1)
	// 平行光
    float3 L = float3(1, -1, -1); //ライト方向
    float3 LColor = float3(1.0, 1.0, 0.9); //ライトカラー
    L = normalize(L); //正規化(長さ=1に)
	// 内積でライトの当たり具合計算
    float I = dot(-L, N);
    I = saturate(I); // cos -1.0～1.0 saturate 0.0～1.0 0以下を0に 1以上を１に
	
	// ライト適用
    float3 Light = LColor * I;
	
	// 半球ライティング
	// 法線.y  下-1.0|-----0.0-----|1.0上
	// 空具合     0.0|-----0.5-----|1.0
    float sky_rate = N.y * 0.5 + 0.5;
    float3 sky_color = float3(0.3, 0.45, 0.5);
    Light += sky_color * sky_rate;

    float ground_rate = 1.0 - sky_rate; //空の反対 -N.y * 0.5 + 0.5;
    float3 ground_color = float3(0.2, 0.05, 0.0);
    Light += ground_color * ground_rate;
    color.rgb *= Light;
    
    const float depthScale = 50.0f;
    float depth = input.ScreenPosition.z / depthScale;
    
	// 出力を複数枚出力可能に変更
    PS_OUTPUT output = (PS_OUTPUT) 0;
    output.Color = color;
    output.GBuffer = float4(depth, 0, 0, 1);
    
    return output;
}

//--------------------------------------------
//	頂点シェーダー
//--------------------------------------------
VStoPS VSOutline(APPtoVS input)
{
    VStoPS output = (VStoPS) 0;

    float3 n = normalize(input.Normal);
    float4 P = float4(input.Position + n * 0.03, 1.0);
    output.Position = mul(matWVP, P);
    output.Tex = input.Tex;
	// ワールド座標
    output.WorldPosition = mul(World, P).xyz;
	// スクリーン座標
    output.ScreenPosition = output.Position;
    
	// 法線変換
    float4 N = float4(input.Normal, 0.0);
    output.Normal = mul(World, N).xyz;
		
    return output;
}

//--------------------------------------------
//	ピクセルシェーダー
//--------------------------------------------

float4 PSOutline(VStoPS input) : SV_TARGET0
{    
    float4 tex = BaseColor.Sample(WrapSampler, input.Tex);
    float4 color = tex;
	// 法線
    float3 N = normalize(input.Normal); //受け取った法線正規化(長さ=1)
	// 平行光
    float3 L = float3(1, -1, -1); //ライト方向
    float3 LColor = float3(1.0, 1.0, 0.9); //ライトカラー
    L = normalize(L); //正規化(長さ=1に)
	// 内積でライトの当たり具合計算
    float I = dot(-L, N);
    I = saturate(I); // cos -1.0～1.0 saturate 0.0～1.0 0以下を0に 1以上を１に
	
	// ライト適用
    float3 Light = LColor * I;
	
    color.rgb *= Light;
    
    color = float4(1.0, 1.0, 1.0, 1);
    
    return color;
}
