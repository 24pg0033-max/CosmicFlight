#pragma once
#include "Object.h"

class Character:public Object{
public:
	void Initialize();
	void Update();

	void LoadAnimation(std::string id, char* filename);
	void Animation(std::string id, bool isLoop, float BlendTime);

	Vector3 GetDirection();
	Vector3 Direction;

protected:
	void Play(std::string name);
	bool CheckCollision();
};

