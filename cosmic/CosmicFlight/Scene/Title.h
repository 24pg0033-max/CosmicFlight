#pragma once
#include <memory>
#include "../DXShader.h"

class Render2D;
class _2DUI;
class Easing;
class TitlePlayer;
class PostEffect;
class Camera;

class Title {
public:
	Title();
	~Title();
	bool Initialize();
	void Update();
	void Render();
private:
	std::unique_ptr<Render2D> titleBG;
	std::unique_ptr<_2DUI> logo;          // ロゴ
	std::unique_ptr<_2DUI> start;         // スタートボタン
	std::unique_ptr<_2DUI> end;           // 終了ボタン
	std::unique_ptr<_2DUI> selectEffect;  // 選択UI
	std::unique_ptr<_2DUI> uSquare;
	std::unique_ptr<_2DUI> bSquare;

	Shader shader;
	std::unique_ptr<TitlePlayer> tPlayer;
	std::unique_ptr<PostEffect> whiteOut;
	std::unique_ptr<Camera> camera;

	const Vector3 tSkySize = Vector3(1000.0f, 1000.0f, 1000.0f);//スカイドームのサイズ

	const float logoX			= 15.0f;		// ロゴX座標
	const float logoY			= 60.0f;		// ロゴY座標
	const float logoSizeX		= 1250.0f;		// ロゴXサイズ
	const float logoSizeY		= 250.0f;		// ロゴXサイズ
	const float startBottomX	= 520.0f;		// スタートボタンX座標
	const float startBottomY	= 400.0f;		// スタートボタンY座標
	const float startSizeX		= 250.0f;		// スタートボタンサイズ
	const float startSizeY		= 75.0f;		// スタートボタンサイズ
	const float endBottomX		= 545.0f;		// エンドボタンX座標
	const float endBottomY		= 550.0f;		// エンドボタンY座標
	const float endSizeX		= 250.0f;		// エンドボタンサイズx
	const float endSizeY		= 75.0f;		// エンドボタンサイズy
	const float selectX			= 450.0f;
	const float selectX2		= 450.0f;
	const float selectY			= 408.0f;
	const float selectY2		= 552.0f;
	const float selectH			= 60.0;
	const float selectW			= 60.0f;

	bool playFlag = true;					// スタートを選んでいるかどうか
	const float speed = 0.05f;
	float max = 0.5f;
	float rate = 0.0f;

	const int sNum = 2;
	const float sSize = 1.5f;
	const float squareX = 435.0f;
	const float squareY = 393.0f;
	const float squareX2 = 435.0f;
	const float squareY2 = 537.0f;

	float currentAlpha = 0.0f;
};
