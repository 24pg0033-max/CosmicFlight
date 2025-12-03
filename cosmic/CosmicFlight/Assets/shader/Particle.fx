// プログラム側から受け取る情報
cbuffer CBBaseMatrix : register(b0)
{
    matrix World;
    matrix View;
    matrix Proj;
    matrix matWVP;
};


//	テクスチャ
Texture2D BaseColor : register(t0);

// テクスチャサンプラー
SamplerState WrapSampler : register(s0);
SamplerState ClampSampler : register(s1);
SamplerState BorderSampler : register(s2);

//	入力頂点
// ゲーム側から受け取る値
struct APPtoVS
{
    float3 Position : POSITION;
    float3 Param : NORMAL; // xにsize
    float2 Tex : TEXCOORD;
    float4 Color : COLOR;
};

// 頂点シェーダーからジオメトリシェーダーへ
struct VStoGS
{
    float4 Position : SV_POSITION;
    float3 size : TEXCOORD0;
    float2 Tex : TEXCOORD1;
    float4 Color : COLOR;
};

// ジオメトリシェーダーからピクセルシェーダーへ
struct GStoPS
{
    float4 Position : SV_POSITION;
    float2 Tex : TEXCOORD0;
    float4 Color : COLOR;
};

//--------------------------------------------
//	頂点シェーダー
//--------------------------------------------
VStoGS VSMain(APPtoVS input)
{
    VStoGS output = (VStoGS) 0;
	// 受け取ったPositionをそのままGSへ
    output.Position = float4(input.Position, 1.0);
    output.size = input.Param; // サイズと1タイルの幅高を渡す
    output.Tex = input.Tex; // テクスチャ座標をそのままGSへ
    output.Color = input.Color; //カラーをそのまま渡す
    return output;
}

//--------------------------------------------
//	ジオメトリシェーダー
//--------------------------------------------
[maxvertexcount(4)] // 最大４頂点化
void GSMain(
	point VStoGS input[1], //１頂点受け取る
	inout TriangleStream<GStoPS> output
)
{
	// 頂点座標をカメラ空間に変換
    float4 P = mul(View, input[0].Position);
	// 四角形化
    GStoPS v = (GStoPS) 0;
	
	// テクスチャ座標関連
    float2 uv_base = input[0].Tex;
    float2 uv_wh = input[0].size.yz;
	
	// サイズ受け取り
    float size = input[0].size.x * 0.5;
	
	// 右上
    float4 v1 = P + float4(size, size, 0, 0);
    v.Position = mul(Proj, v1); //カメラ空間→画面空間に
    v.Tex = uv_base + float2(uv_wh.x, 0.0);
    v.Color = input[0].Color;
    output.Append(v); //頂点追加

	// 左上
    float4 v2 = P + float4(-size, size, 0, 0);
    v.Position = mul(Proj, v2); //カメラ空間→画面空間に
    v.Tex = uv_base;
    v.Color = input[0].Color;
    output.Append(v); //頂点追加

	// 右下
    float4 v3 = P + float4(size, -size, 0, 0);
    v.Position = mul(Proj, v3); //カメラ空間→画面空間に
    v.Tex = uv_base + uv_wh;
    v.Color = input[0].Color;
    output.Append(v); //頂点追加
	
	// 左下
    float4 v4 = P + float4(-size, -size, 0, 0);
    v.Position = mul(Proj, v4); //カメラ空間→画面空間に
    v.Tex = uv_base + float2(0.0, uv_wh.y);
    v.Color = input[0].Color;
    output.Append(v); //頂点追加
	
	// 四角形完了
    output.RestartStrip();

}

//--------------------------------------------
//	ピクセルシェーダー
//--------------------------------------------
float4 PSMain(GStoPS input) : SV_TARGET0
{
	// テクスチャ
    float4 tex = BaseColor.Sample(ClampSampler, input.Tex);
	// 出力カラー決定
    float4 color = tex;
	// カラー適用
    float rate = saturate(pow(saturate(color.r), 3.0));
    color.rgb *= lerp(input.Color.rgb, 1.0, rate);

    color.a *= input.Color.a;
	//clip(color.a - 0.001);
    return color;
}

