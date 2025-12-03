#pragma once
#include "../Object/Object.h"
#include "../DXShader.h"

class UVAnim : public Object{
public:
	void Initialize();
	void Release();
	void Update();
	void Render();

private:
	Shader UVAnimShader;
	struct UVAnimParam {
		float uSpeed, vSpeed;
		float time;
		float dummy; //4ŒÂ‹æØ‚è‚Ìˆ×
	};
	UVAnimParam uvanimParam{};
	ID3D11Buffer* CBUVAnim = nullptr;

	const Vector3 firstScale = Vector3(1000.0f, 1000.0f, 1000.0f);
};
