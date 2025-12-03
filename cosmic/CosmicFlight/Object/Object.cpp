#include "Object.h"
#include "../Memory/Memory.h"
#include "../DXShader.h"

Object::Object(){
	Initialize();
}

Object::~Object(){
}
//更新
void Object::Initialize(){
	position = Vector3(0.0f, 0.0f, 0.0f);
	rotation = Vector3(0.0f, 0.0f, 0.0f);
	scale = Vector3(1.0f, 1.0f, 1.0f);
}

//モデル読み込み
void Object::Load(const std::string& filename) {
	mesh = NEW FBXMesh();
	mesh->Create(filename.c_str());
	position = Vector3(0.0f, 0.0f, 0.0f);
}

//解放
void Object::Release() {
	if (mesh) {
		delete mesh;
		mesh = nullptr;
	}
}
//更新
void Object::Update(){
	mesh->position = position;
	mesh->rotation = rotation;
	mesh->scale = scale;
	mesh->Update();
}

//描画
void Object::Render(){
	mesh->Render();
}

void Object::CloneModel(FBXMesh* source)
{
	mesh = NEW FBXMesh();
	mesh->Clone(source);

	position = Vector3(0.0f, 0.0f, 0.0f);
	rotation = Vector3(0.0f, 0.0f, 0.0f);
	scale = Vector3(1.0f, 1.0f, 1.0f);
}
//行列取得
Matrix Object::GetTransForm(){
	return mesh->transform;
}

//座標取得
Vector3 Object::GetPosition(){
	return position;
}

Vector3 Object::GetScale(){
	return scale;
}
