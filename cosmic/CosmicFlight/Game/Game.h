#pragma once
#include "../Manager/SceneManager.h"

//シーンマネージャー初期化
void GameInitialize();
//シーンマネージャー解放
void GameRelease();
//シーンマネージャー更新描画
void GameMain();

SceneManager* GetGameScene();