#include "GameOverScene.h"
#include "GameOver.h"
#include "../Game/Game.h"
#include "../Memory/Memory.h"

//‰Šú‰»
bool GameOverScene::Initialize() {
    scene = NEW GameOver();
    scene->Initialize();
    return true;
}

void GameOverScene::Release(){
    delete scene;
}

//XV
void GameOverScene::Update() {
    scene->Update();
}

//•`‰æ
void GameOverScene::Render() {
    scene->Render();
}