#pragma once
#include "Character.h"

class TitlePlayer : public Character {
public:
	TitlePlayer();
	~TitlePlayer() {};
	void Initialize();
	void Update();
	void Render();

private:
	const Vector3 firstPos = Vector3(0.0f, 0.0f, -10);
	const Vector3 firstSca = Vector3(0.025f, 0.025f, 0.025f);
	Vector3 posVelocity = Vector3(1.2f, 1.2f, 0.0f);
	Vector3 rotVelocity = Vector3(0.0f, 0.0f, 0.6f);

	const float posLimitTop = 6.0f;
	const float posLimitBottom = -6.0f;
	const float posLimitRight = 10.0f;
	const float posLimitLeft = -10.0f;
};