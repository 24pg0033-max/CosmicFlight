#pragma once
#include <memory>
#include "../UI/2DUI.h"

class Result
{
public:
	Result();
	~Result();

	void Initialize();
	void Update();
	void Render();

private:
	std::unique_ptr<_2DUI> hGameOver;
	std::unique_ptr<_2DUI> gameOverLogo;
	std::unique_ptr<_2DUI> start;
	std::unique_ptr<_2DUI> end;
	std::unique_ptr<_2DUI> selectEffect;
	std::unique_ptr<_2DUI> uSquare;
	std::unique_ptr<_2DUI> bSquare;

	bool playFlag = true;					// スタートを選んでいるかどうか

	const float selectH = 60.0f;
	const float selectW = 60.0f;
	const float selectX = 450.0f;
	const float selectX2 = 450.0f;
	const float selectY = 408.0f;
	const float selectY2 = 552.0f;

	const float startBottomX = 520.0f;		// スタートボタンX座標
	const float startBottomY = 400.0f;		// スタートボタンY座標
	const float	startSizeX = 250.0f;		// スタートボタンサイズ
	const float	startSizeY = 75.0f;			// スタートボタンサイズ
	const float endBottomX = 545.0f;		// エンドボタンX座標
	const float endBottomY = 550.0f;		// エンドボタンY座標
	const float	endSizeX = 250.0f;			// エンドボタンサイズx
	const float	endSizeY = 75.0f;			// エンドボタンサイズy

	const float sSize = 1.5f;
	const float squareX = 435.0f;
	const float squareY = 393.0f;
	const float squareX2 = 435.0f;
	const float squareY2 = 537.0f;

	bool pFlag = true;					// スタートを選んでいるかどうか

	const float gameOverW = 500.0f;
	const float gameOverH = 180.0f;
};