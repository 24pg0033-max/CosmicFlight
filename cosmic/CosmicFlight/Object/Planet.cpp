#include "Planet.h"
#include "../DXShader.h"
#include "../RenderState.h"
#include "../global.h"
#include "../Camera/Camera.h"

Planet::Planet(){
	Initialize();
	Load("Assets/stage/skydome/planet.fbx");

	rimShader = NEW Shader();
	rimShader->Create(L"Assets/shader/Planet.fx", "VSMain", "PSMain");
	rimCB = Shader::CreateConstantBuffer(sizeof(CBRimParam));
}

Planet::~Planet(){
	delete rimShader;
	if (rimCB) rimCB->Release();
}

void Planet::Initialize(){
	position = planetPosition;
	scale = planetSize;

	rimParam.rimPower = 3.0f;
	rimParam.rimIntensity = 1.2f;
	rimParam.rimColor = Vector3(0.4f, 0.6f, 1.0f);
}

void Planet::Update(){
	rotation += planetRot * DeltaTime;
	position = planetPosition;
	Object::Update();
}

void Planet::Render() {
	rimParam.cameraPos = camera->GetPosition();
	DxSystem::DeviceContext->UpdateSubresource(rimCB, 0, NULL, &rimParam, 0, 0);
	DxSystem::DeviceContext->PSSetConstantBuffers(6, 1, &rimCB);

	rimShader->Activate();
	Object::Render();
}
