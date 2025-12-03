#include "2DUI.h"
#include "../Input/Input.h"
#include "../RenderState.h"
#include "../Memory/Memory.h"



//初期化コンストラクタ
_2DUI::_2DUI(char* filename, float width, float height, Vector3 pos, float size, float sizemax, bool renderflag)
	:hUIWidth(width), hUIHeight(height), hUISizeMax(sizemax){
	Render2D::Initialize();
	hUI = NEW Render2D();
	hUI->Initialize(filename);
	position = pos;
	hUISize = size;
	hUI->SetSize(hUIWidth * hUISize, hUIHeight * hUISize);
	hUI->SetPosition(position);
	renderFlag = renderflag;
	changinghUI = NEW Easing(Easing::STATE::EASEOUTCUBIC, changingSpeed, 0, false);
}

//デストラクタ
_2DUI::~_2DUI(){
	if (hUI != nullptr){delete hUI; hUI = nullptr;}
	if (changinghUI != nullptr){delete changinghUI; changinghUI = nullptr;}
}

//初期化
void _2DUI::Initialize(char* filename, float width, float height, Vector3 pos, float size, float sizemax, float alpha,float alphamax, bool renderflag){
	hUIWidth = width;
	hUIHeight = height;
	hUISize = size;
	hUISizeMax = sizemax;
	hUI = NEW Render2D();
	hUI->Initialize(filename);
	position = pos;
	hUIAlpha = alpha;
	hUIAlphaMax = alphamax;
	hUI->SetSize(hUIWidth * hUISize, hUIHeight * hUISize);
	hUI->SetPosition(position);
	renderFlag = renderflag;
	changinghUI = NEW Easing(Easing::STATE::EASEOUTCUBIC, changingSpeed, 0, false);
}

//更新
void _2DUI::Update() {
	//座標更新
	UpdateMove();
	SetPosition(position);
	//画像サイズ変更
	hUI->SetSize((hUISize + changingSizeMax * changingRate) * hUIWidth, (hUISize + changingSizeMax * changingRate) * hUIHeight);

	//UIのRender_Flagがfalseならサイズを０にしていく
	if (!renderFlag) {
		hUISize -= speed;
		if (hUISize < 0)hUISize = 0;

	}
	else {// UIのRender_Flagがtrueならサイズを1にしていく
		hUISize += speed;
		if (isSelect) {// UIが選択されていたらSizeMaxまで拡大
			if (hUISize > hUISizeMax) {
				hUISize = hUISizeMax;
			}
		}
		else {
			if (hUISize > 1.0f)hUISize = 1.0f;
		}
	}
	// 選択中の拡大用
	if (isChanging) {
		changingRate = changinghUI->RateUpdate();
		if (changinghUI->GetIsMaxRate() || changinghUI->GetIsMinRate()) {
			changinghUI->ChangeIsOn();
		}
	}
	else {
		changingRate = changinghUI->RateUpdate();
	}
}

void _2DUI::AlphaChange(float a){

	//UIのRender_Flagがfalseならアルファ値を０にしていく
	if (!renderFlag) {
		hUIAlpha -= speed;
		if (hUIAlpha < 0)hUIAlpha = 0;
	}
	else {//UIのRender_Flagがtrueならアルファ値を1にしていく
		hUIAlpha += speed;
		if (hUIAlpha > hUIAlphaMax) {
			hUIAlpha = hUIAlphaMax;
		}
		else {
			if (hUIAlpha > 1.0f)hUIAlpha = 1.0f;
		}
	}
	//選択中の変更用
	if (isChanging) {
		changingRate = changinghUI->RateUpdate();
		if (changinghUI->GetIsMaxRate() || changinghUI->GetIsMinRate()) {
			changinghUI->ChangeIsOn();
		}
	}
	else {
		changingRate = changinghUI->RateUpdate();
	}

	SetAlpha((hUIAlpha + changingAlphaMax * changingRate * DeltaTime) * a);
}

float _2DUI::CalculateAlpha(int x, int y, float centerX, float centerY, int maxDist){
	// ピクセルの座標から中心までの距離を計算
	hUICenterX = x - centerX;
	hUICenterY = y - centerY;
	float distance = sqrtf(hUICenterX * hUICenterX + hUICenterY * hUICenterY);

	// 距離に応じてアルファ値を計算 (0:完全透明、1:完全不透明)
	hUIAlpha = 1.0f - (distance / maxDist);

	// 0未満にならないようにクリップ
	return max(0.0f, hUIAlpha);
}

void _2DUI::Render(){
	if (!renderFlag) return;
	RenderState::SetBlendState(RenderState::ALPHA);
	RenderState::DepthEnable(true, false);
	hUI->Render2D::Render();
	RenderState::SetBlendState(RenderState::NONE);
	RenderState::DepthEnable(true);

}

void _2DUI::SetColor(float r, float g, float b, float a){
	hUI->Render2D::SetColor(r, g, b, a);
}

void _2DUI::SetAlpha(float a){
	hUI->Render2D::SetAlpha(a);
}

void _2DUI::SetPosition(Vector3 p){
	//サイズをによってRender2Dにポジションをセット
	position = p;
	Vector3 tmpP = position;
	hUI->SetPosition(tmpP);
}

void _2DUI::SetPivotCenter(){
	float w = hUIWidth * hUISize;
	float h = hUIHeight * hUISize;
	hUI->Render2D::SetPivot(w * 0.5f, h * 0.5f);
}

void _2DUI::SetRotation(float r){
	hUI->Render2D::SetRotation(r);
}

void _2DUI::SetPivotRotation(float r){
	// Pivot を画像中心にする
	float w = hUIWidth * hUISize;
	float h = hUIHeight * hUISize;

	hUI->Render2D::SetPivot(w * 0.5f, h * 0.5f);

	// 回転をセット
	hUI->Render2D::SetRotation(r);
}

void _2DUI::SetUV(float u, float v, float w, float h){
	hUI->Render2D::SetUV(u, v, w, h);
}

void _2DUI::SetSize(float width, float height){
	hUI->Render2D::SetSize(width, height);
}

void _2DUI::SetChangingSize(bool f){
	if (isChanging == f)return;
	isChanging = f;
	if (!isChanging) {
		changinghUI->SetIsOn(false);
	}
	else {
		changinghUI->SetnowFrame(0);
		changinghUI->SetIsOn(false);
	}
}

void _2DUI::StartMove(Vector3 start, Vector3 end, float duration)
{
	moveStartPos = start;
	moveEndPos = end;
	moveDuration = duration;
	moveTime = 0.0f;
	isMoving = true;
}

void _2DUI::UpdateMove()
{
	if (!isMoving) return;

	moveTime += DeltaTime;
	float time = moveTime / moveDuration;

	if (time >= 1.0f)
	{
		time = 1.0f;
		isMoving = false;
		isChanging = false;
	}

	// 直線補間だけ
	float x = moveStartPos.x * (1 - time) + moveEndPos.x * time;
	float y = moveStartPos.y * (1 - time) + moveEndPos.y * time;

	position = Vector3(x, y, moveStartPos.z);
}