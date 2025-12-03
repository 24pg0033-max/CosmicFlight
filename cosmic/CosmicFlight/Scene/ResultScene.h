#pragma once
#include "../Manager/SceneManager.h"
#include "Result.h"
#include "../Object/Player.h"

class ResultScene :public SceneBase
{
public:
	bool Initialize();// ‰Šú‰»
	void Release();		// ‰ğ•ú
	void Update();		// XV
	void Render();		// •`‰æ
	const char* GetName() const { return "Result"; }
private:
	Result* scene = nullptr;
};