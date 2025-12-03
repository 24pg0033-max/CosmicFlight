#pragma once
#include "Object.h"
#include "../DXShader.h"

class Camera;

class Planet : public Object {
public:
	Planet();
	~Planet();

	void Initialize();
	void Update();
	void Render(); 

	void SetCamera(Camera* cameraObj) { camera = cameraObj; }

	struct CBRimParam {
		Vector3 cameraPos;
		float rimPower;
		float rimIntensity;
		Vector3 rimColor;
	} rimParam;

private:
	Shader* rimShader = nullptr;
	ID3D11Buffer* rimCB = nullptr;

	Camera* camera;

	const Vector3 planetPosition = Vector3(35.0f, -15.0f, -50.0f);
	const Vector3 planetSize = Vector3(25.0f, 25.0f, 25.0f);
	const Vector3 planetRot = Vector3(0.0f, 0.08f, 0.0f);
};