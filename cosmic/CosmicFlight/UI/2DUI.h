#pragma once
#include "../Render2D.h"
#include "../Easing/Easing.h"
class _2DUI
{
public:
	_2DUI(char* filename, float width, float height, Vector3 pos = Vector3(0.0f, 0.0f, 0.0f), float size = 1.0f, float sizemax = 1.0f, bool renderflag = true);
	_2DUI() {};
	~_2DUI();
	void Initialize(char* filename, float width, float height, Vector3 pos = Vector3(0.0f, 0.0f, 0.0f), float size = 1.0f, float sizemax = 1.0f, float alpha = 1.0f, float alphamax = 1.0f, bool renderflag = true);
	void Update();
	void Render();

	void AlphaChange(float a);
	float CalculateAlpha(int x, int y, float centerX, float centerY, int maxDist);
	//ゲッター
	bool GetRender_Flag() const { return renderFlag; }
	bool GetIsHit() const { return isHit; }

	//セッター
	void SetRender_Flag(bool f) { renderFlag = f; }
	void SetColor(float r, float g, float b, float a = 1.0f);
	void SetAlpha(float a);
	void SetPosition(Vector3 p);
	void SetPivotCenter();
	void SetIsSelect(bool b) { isSelect = b; }//現在選択されているかどうか
	void SetRotation(float r);
	void SetPivotRotation(float r);
	void SetUV(float u, float v, float w, float h);
	void SetSize(float width, float height);
	void SetChangingSize(bool f);
	void StartMove(Vector3 start, Vector3 end, float duration);
	void UpdateMove();
private:
	Render2D* hUI;				//UI画像
	float hUISize;				//現在の画像サイズ
	float hUISizeMax;			//画像最大サイズ
	float hUICenterX;
	float hUICenterY;
	float hUIDistance;
	float hUIWidth;				//画像幅
	float hUIHeight;				//画像高さ
	bool renderFlag = true;	//描画するかどうか
	Vector3 position;			//画像の真ん中の座標
	bool isCollision = true;	//UIにコリジョンをつけるかどうか
	bool isHit = false;			//UIがクリックされたかどうか
	bool isSelect = false;		//UIが選択されているかどうか
	bool isChanging = false;    //変化中かどうか
	Easing* changinghUI;		//拡縮イージング
	float changingRate = 1.0f;  //拡縮のイージング値保存
	const float changingSizeMax = 0.2f;//hUI_Sizeに足す大きさ
	const float changingAlphaMax = 10.0f;//hUI_Alphaに足す大きさ
	const float changingSpeed = 0.01f;//イージング速度
	float hUIAlpha;              //画像のアルファ値
	float hUIAlphaMax;           //画像最大アルファ

	const float speed = 0.1f;	//UIの値が変わる速度

	// UI移動用
	bool isMoving = false;
	Vector3 moveStartPos;
	Vector3 moveEndPos;
	float moveTime = 0.0f;
	float moveDuration = 1.0f;   // 移動時間(秒)
};

