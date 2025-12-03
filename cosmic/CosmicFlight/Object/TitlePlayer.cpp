#include "TitlePlayer.h"
#include "../global.h"

TitlePlayer::TitlePlayer(){
	Initialize();
}

void TitlePlayer::Initialize(){
	Load("Assets/astro/Astrounalts.fbx");
	LoadAnimation("fly", "Assets/astro/Swimming.fbx");
	position = firstPos;
	scale = firstSca;
	Animation("fly", true, 0.0f);
}

void TitlePlayer::Update(){
	position += posVelocity * DeltaTime;
	rotation += rotVelocity * DeltaTime;

	if (position.x <= posLimitLeft || position.x >= posLimitRight) {
		posVelocity.x = -posVelocity.x;
	}
	if (position.y <= posLimitBottom || position.y >= posLimitTop) {
		posVelocity.y = -posVelocity.y;
	}


	Character::Update();
}

void TitlePlayer::Render(){
	Object::Render();
}
