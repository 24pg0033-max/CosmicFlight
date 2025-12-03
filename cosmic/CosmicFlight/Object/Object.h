#pragma once
#include "../DxSystem.h"
#include "../FbxMesh.h"
#include "../Memory/Memory.h"

class Object
{
public: //誰でもアクセス可能
	Object();
	~Object();
	void Initialize();
	void Load(const std::string& filename);
	void Release();
	void Update();
	void Render();

	void CloneModel(FBXMesh* source);

	//ゲッター
	Matrix GetTransForm();//３Dの行列取得
	Vector3 GetPosition();
	Vector3 GetScale();
	Vector3 GetRotation() { return rotation; }
	FBXMesh* GetMesh() { return mesh; }
	bool GetIsActive() { return IsActive; }							// 生存情報取得


	//セッター
	void SetScale(Vector3 val) { scale = val; }
	void SetRotation(Vector3 val) { rotation = val; }
	void SetPosition(Vector3 val) { position = val; }

protected://継承先からアクセス可能
	FBXMesh* mesh;			//3Dモデル
	Vector3 position;		//座標
	Vector3 rotation;		//回転
	Vector3 scale;			//拡縮
	bool IsActive;				// 生きているかどうか
	const float Suitable = 100000.0f;		// 適当な数
private:

};

