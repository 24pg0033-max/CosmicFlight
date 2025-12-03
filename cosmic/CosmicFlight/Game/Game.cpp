#include "Game.h"
#include "../Scene/GameScene.h"
#include "../Scene/TitleScene.h"
#include "../Memory/Memory.h"

SceneManager Scene;//シーンマネージャー

void GameInitialize(){
	Scene.Initialize();//シーンマネージャー初期化
	Scene.JumpScene(NEW TitleScene);//初めに起動するシーン設定
}

void GameRelease(){
	Scene.Release();
}

void GameMain(){
	Scene.Update();
	Scene.Render();
}

SceneManager* GetGameScene()
{
	return &Scene;
}
