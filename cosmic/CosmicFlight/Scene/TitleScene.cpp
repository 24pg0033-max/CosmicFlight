#include "TitleScene.h"
#include "../Game/Game.h"
#include "GameScene.h"
#include "../Manager/SceneManager.h"
#include "../Input/Input.h"
#include "../Memory/Memory.h"

//‰Šú‰»
bool TitleScene::Initialize(){
    scene = NEW Title();
    scene->Initialize();
    return true;
}

void TitleScene::Release(){
    delete scene;
}

//XV
void TitleScene::Update(){
    scene->Update();
}

//•`‰æ
void TitleScene::Render(){
    scene->Render();
}
