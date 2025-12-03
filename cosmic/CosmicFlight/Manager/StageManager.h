#pragma once
#include <memory>
#include "../Effect/UVAnim.h"
#include "../DXShader.h"
#include "../Object/Meteor.h"

class Player;
class RingManager;
class MeteorManager;
class Object;
class Camera;
class ScoreManager;
class Planet;
class ParticleSystem;

class StageManager {
public:
	StageManager();
	~StageManager();

	void Update();
	void Render();

	void SetTarget(Player* obj);
	void SetCamera(Camera* cam) { camera = cam; };

	void MeteorProcess();
	Meteor::MeteorType MeteorFlag(Vector3 pos);		//隕石に当たったかどうか
	bool RingFlag(Vector3 pos);		//リングに当たったかどうか
	float GenerateRandomFloat(float min, float max);
	RingManager* GetRingManager() const { return ringManager.get(); }
	ScoreManager* GetScoreManager() const { return scoreManager.get(); }

	void ParticleUpdate();

	static StageManager* current;
	std::unique_ptr<MeteorManager> meteor;

protected:
	std::unique_ptr<FBXMesh> mesh;

private:
	Player* player;

	//スカイドーム
	std::unique_ptr<Object> sky;
	UVAnim skyUVAnim;
	const Vector3 skyPosition = Vector3(0, 0, 0);//スカイドーム中心座標
	const Vector3 skySize = Vector3(1000, 1000, 1000);//スカイドームのサイズ


	//取得物
	std::unique_ptr<RingManager> ringManager;

	Camera* camera;

	std::unique_ptr<ScoreManager> scoreManager;

    std::unique_ptr<ParticleSystem> particle;
	const int textureX = 3;
	const int textureY = 3;
	const int startAnim = 0;
	const int endAnim = 8;
	const float startSize = 0.5f;
	const float endSize = 0.0f;
	Vector3 exploVel;

	//隕石
	Vector3 meteorPos;
	float meteorTime = 180.0f;                  //最初の隕石が出るまで
	Vector3 meteorPosition;						//隕石の座標
	int laneX = 5;								//横レーン
	int laneY = 3;								//縦レーン
	const float meteorFrequency = 80.0f;		//隕石の出る頻度

	float randXPos = 0.0f;                      //ランダム座標生成
	float randYPos = 0.0f;                      //ランダム座標生成
	const float minRandPos = -10.0f;
	const float maxRandPos = 10.0f;
	const float randXPosAdjust = 20.0f;
	const float randZPos = -60.0f;

};
