#include "Character.h"
#include "../global.h"
#include "../Manager/StageManager.h"

void Character::Initialize() {
	Object::Initialize();
}

void Character::Update() {
	Object::Update();
	mesh->Animate(DeltaTime);
}

//方向取得
Vector3 Character::GetDirection() {
	return Direction;
}

//アニメーション読み込み
void Character::LoadAnimation(std::string id, char* filename){
	mesh->AddMotion(id, filename);
}

//アニメーション変更
void Character::Animation(std::string id, bool isLoop, float BlendTime){
	if (mesh->MotionName == id)return;
	mesh->Play(id, isLoop, BlendTime);
}

void Character::Play(std::string name)
{
	// 既に同じモーション再生中なら何もしない
	if (mesh->MotionName == name) return;
	// 再生
	mesh->Play(name);
}

bool Character::CheckCollision(){
	return true;
}

