// outline.fx
// プログラム側から受け取る情報
cbuffer CBBaseMatrix : register(b0)
{
    matrix World;
    matrix View;
    matrix Proj;
    matrix matWVP;
    
    float3 ViewPosition;
};

cbuffer CBOutlineParam : register(b6)
{
    float outlineThickness; // アウトラインの太さ
    float3 dummy;
};

//	テクスチャ
Texture2D Tex : register(t0);
Texture2D g_depthTexture : register(t7);

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
    float4 posInProj : TEXCOORD1;
};


VStoPS VSMain(APPtoVS input)
{
    VStoPS output = (VStoPS) 0;
    float4 P = float4(input.Position, 1.0);
    output.Position = mul(World, P); // モデルの頂点をワールド座標系に変換
    output.Position = mul(View, output.Position); // ワールド座標系からカメラ座標系に変換
    output.Position = mul(Proj, output.Position); // カメラ座標系からスクリーン座標系に変換
    output.Tex = input.Tex;

    return output;
}

float4 PSMain(VStoPS input) : SV_TARGET0
{
    // step-5 近傍8テクセルの深度値を計算して、エッジを抽出する
    // 正規化スクリーン座標系からUV座標系に変換する
    float2 uv = input.posInProj.xy * float2(0.5f, -0.5f) + 0.5f;

    // 近傍8テクセルへのUVオフセット
    float2 uvOffset[8] =
    {
        float2(0.0f, 1.0f / 720.0f), //上
        float2(0.0f, -1.0f / 720.0f), //下
        float2(1.0f / 1280.0f, 0.0f), //右
        float2(-1.0f / 1280.0f, 0.0f), //左
        float2(1.0f / 1280.0f, 1.0f / 720.0f), //右上
        float2(-1.0f / 1280.0f, 1.0f / 720.0f), //左上
        float2(1.0f / 1280.0f, -1.0f / 720.0f), //右下
        float2(-1.0f / 1280.0f, -1.0f / 720.0f) //左下
    };

    // このピクセルの深度値を取得
    float depth = g_depthTexture.Sample(ClampSampler, uv).x;

    // 近傍8テクセルの深度値の平均値を計算する
    float depth2 = 0.0f;
    for (int i = 0; i < 8; i++)
    {
        depth2 += g_depthTexture.Sample(ClampSampler, uv + uvOffset[i]).x;
    }
    depth2 /= 8.0f;

    // 自身の深度値と近傍8テクセルの深度値の差を調べる
    if (abs(depth - depth2) > 0.00005f)
    {
        // 深度値が結構違う場合はピクセルカラーを黒にする
        return float4(0.0f, 0.0f, 0.0f, 1.0f);
    }

    // 普通にテクスチャを
    return Tex.Sample(ClampSampler, input.Tex);
}
