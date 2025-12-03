#pragma once
#include "../Manager/SceneManager.h"
#include "Title.h"

class TitleScene :public SceneBase
{
public:
	bool Initialize();// ‰Šú‰»
	void Release();		// ‰ğ•ú
	void Update();		// XV
	void Render();		// •`‰æ
	const char* GetName() const { return "Title"; }
private:
	Title* scene = nullptr;
};

