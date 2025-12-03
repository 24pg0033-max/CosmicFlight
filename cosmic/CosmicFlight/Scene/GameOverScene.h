#pragma once
#include <memory>
#include "../Manager/SceneManager.h"

class GameOver;

class GameOverScene :public SceneBase
{
public:
	bool Initialize();// ‰Šú‰»
	void Release();
	void Update();		// XV
	void Render();		// •`‰æ
	const char* GetName() const { return "GameOver"; }
private:
	GameOver* scene;
};