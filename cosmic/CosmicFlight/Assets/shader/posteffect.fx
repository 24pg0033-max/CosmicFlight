 // プログラム側から受け取る情報
cbuffer CBBaseMatrix : register(b0)
{
    matrix World;
    matrix View;
    matrix Proj;
    matrix matWVP;
};

cbuffer UVAnimParam : register(b1)
{
    float2 speed; //float u_speed, v_speed;
    float time;
    float dummy;
}

cbuffer CBPostEffect : register(b8)
{
    float bloom_threshold; //Bloom高輝度抽出しきい値
    float bloom_rate; //Bloomの合成具合
    float isBloomTarget;
    float dummy2;
};

//	テクスチャ
Texture2D BaseColor : register(t0);
Texture2D BloomBlur : register(t1);
Texture2D BloomBlur1 : register(t2);
Texture2D BloomBlur2 : register(t3);
Texture2D BloomBlur3 : register(t4);
Texture2D BloomBlur4 : register(t5);


// テクスチャサンプラー
SamplerState WrapSampler : register(s0);
SamplerState ClampSampler : register(s1);
SamplerState BorderSampler : register(s2);

//	入力頂点
struct APPtoVS
{
    float3 Position : POSITION;
    float3 TextureSize : NORMAL;
    float2 Tex : TEXCOORD;
};

struct VStoPS
{
    float4 Position : SV_POSITION;
    float3 TextureSize : NORMAL;
    float2 Tex : TEXCOORD0;
};

//--------------------------------------------
//	頂点シェーダー
//--------------------------------------------

VStoPS VSMain(APPtoVS input)
{
    VStoPS output = (VStoPS) 0;
    output.Position = float4(input.Position, 1.0);
    output.TextureSize = input.TextureSize;
    output.Tex = input.Tex;
    return output;
}

VStoPS VSUVAnim(APPtoVS input)
{
    VStoPS output = (VStoPS) 0;

    float4 P = float4(input.Position, 1.0);
    output.Position = mul(matWVP, P);
    output.TextureSize = input.TextureSize;
    output.Tex = input.Tex + speed * time;

    return output;
}

//--------------------------------------------
//	ピクセルシェーダー
//--------------------------------------------
float4 PSUVAnim(VStoPS input) : SV_TARGET0
{
    float4 tex = BaseColor.Sample(WrapSampler, input.Tex);

    float4 color = tex;
    return color;
}

float4 PSMain(VStoPS input) : SV_TARGET0
{
    float4 tex = BaseColor.Sample(ClampSampler, input.Tex);

    float4 color = tex;
    color.a = isBloomTarget;
    
    return color;
}

//--------------------------------------------
//	高輝度抽出
//--------------------------------------------
float4 PSBloomFilter(VStoPS input) : SV_TARGET0
{
    float4 tex = BaseColor.Sample(ClampSampler, input.Tex);
    tex.rgb -= bloom_threshold; //しきい値以下をなくす
    tex.rgb = max(tex.rgb, 0.0); //マイナスをなくす
    tex.a = 1.0;
    return tex;
}

//--------------------------------------------
//	ぼかし
//--------------------------------------------
float4 PSBloomBlur(VStoPS input) : SV_TARGET0
{
    float4 color = float4(0, 0, 0, 1);
	// １ピクセル分のUV
    float pixW = input.TextureSize.x * 2.0f;
    float pixH = input.TextureSize.y * 2.0f;

    float total = 0.0;
    for (int y = -1; y <= 1; y++)
    {
        for (int x = -1; x <= 1; x++)
        {
            // 距離に応じたブレンド率
            float dist = length(float2(x, y)) / 2.0f; //距離によるぼかし具合(2.0は調整値)
            float rate = saturate(1.0 - dist);
            total += rate; //総ブレンド率
            
            float2 adjust = float2(x * pixW, y * pixH) * 1.5; //範囲x1.5
            float4 tex = BaseColor.Sample(ClampSampler, input.Tex + adjust);
            color.rgb += tex.rgb * rate; //距離に応じてブレンド
        }
    }
    color.rgb /= total;

    return color;
}

//--------------------------------------------
//	合成
//--------------------------------------------
float4 PSBloom(VStoPS input) : SV_TARGET0
{
    float4 screen = BaseColor.Sample(ClampSampler, input.Tex);
    float4 bloom = BloomBlur.Sample(ClampSampler, input.Tex);
    float4 bloom1 = BloomBlur1.Sample(ClampSampler, input.Tex);
    float4 bloom2 = BloomBlur2.Sample(ClampSampler, input.Tex);
    float4 bloom3 = BloomBlur3.Sample(ClampSampler, input.Tex);
    float4 bloom4 = BloomBlur4.Sample(ClampSampler, input.Tex);
    
    // 加算合成
    screen.rgb += bloom.rgb * bloom_rate * 0.1;
    screen.rgb += bloom1.rgb * bloom_rate * 0.2;
    screen.rgb += bloom2.rgb * bloom_rate * 0.5;
    screen.rgb += bloom3.rgb * bloom_rate * 0.9;
    screen.rgb += bloom4.rgb * bloom_rate * 1.0;
    screen.a = 1;
    
    return screen;
}


