#pragma once
#include <thread>
#include <memory>
#include "../DXShader.h"
#include "../RenderTarget.h"

class Player;
class Camera;
class Loading;
class RingManager;
class StageManager;
class UIManager;
class EnemyManager;
class PostEffect;
class Fade;
class Planet;

class InGame{
public:
	InGame();
	~InGame();

	bool Initialize();
	void Release();
	void Update();
	void Render();

	static InGame* Current;
	static bool IsLoad;

	bool isLoad = false;
	void LoadThread();

	///ƒJƒƒ‰‚Ìî•ñæ“¾—p
	void SetCamera(Camera* cameraObj) { camera = cameraObj; }

private:
	std::unique_ptr<Player> astro;
	std::unique_ptr<Shader> shader3D;
	std::unique_ptr<Loading> loading;
	std::unique_ptr<UIManager> UIMan;
	std::unique_ptr<StageManager> stage;
	std::unique_ptr<PostEffect> postEffect;
	std::unique_ptr<EnemyManager> ghost;
	std::unique_ptr<Planet> planet;

	Camera* camera;
	RenderTarget tage;
};

