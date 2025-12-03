#pragma once
#include "../Manager/SceneManager.h"
#include "InGame.h"

class GameScene :public SceneBase
{
public:
	bool Initialize();// ‰Šú‰»
	void Release();		// ‰ğ•ú
	void Update();		// XV
	void Render();		// •`‰æ
	const char* GetName() const { return "Game"; }
private:
	InGame scene;
};

