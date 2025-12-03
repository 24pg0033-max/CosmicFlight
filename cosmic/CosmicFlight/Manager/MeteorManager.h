#pragma once
#include <list>
#include "../Object/Meteor.h"

class ScoreManager;

class MeteorManager {
public:
	MeteorManager();
	~MeteorManager();

	bool Initialize();
	void Update();
	void Render();

	void Add(std::unique_ptr<Meteor> obj, Vector3 pos);
	void AddMeteorType(Meteor* obj);
	void AddMotionType(Meteor* obj);
	void Clear();

	bool GetActive();				// 生存フラグ取得
	void SetActive(bool active);	// 生存フラグセット

	Meteor::MeteorType MeteorJudge(Vector3 pos);

	void SetScoreManager(ScoreManager* manager) { scoreManager = manager; }
	std::list<std::unique_ptr<Meteor>>& GetMeteorList() { return meteorList; }

private:
	std::list<std::unique_ptr<Meteor>> meteorList;
	std::unique_ptr<FBXMesh> mesh1;
	std::unique_ptr<FBXMesh> mesh2;
	FBXMesh* selectedMesh;

	float minScale = 2.5f;
	float maxScale = 4.0f;
	float rotMin = -2.0f;
	float rotMax = 2.0f;
	float meteorScale = 0.0f;
	const float meteorRange = 100.0f;
	float radius = 0.0f;

	Vector3 meteorPosition;
	Vector3 firstMeteorPosition = Vector3(1000.0f, 1000.0f, 1000.0f);
	const float meteorDistanceAdjust = 0.5f;		//隕石の範囲
	ScoreManager* scoreManager = nullptr;
};