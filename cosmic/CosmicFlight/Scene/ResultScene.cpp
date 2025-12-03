#include "ResultScene.h"
#include "../Game/Game.h"

//‰Šú‰»
bool ResultScene::Initialize() {
    scene = NEW Result();
    scene->Initialize();
    return true;
}

void ResultScene::Release() {
    delete scene;
}

//XV
void ResultScene::Update() {
    scene->Update();
}

//•`‰æ
void ResultScene::Render() {
    scene->Render();
}