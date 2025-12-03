#pragma once
#define SAFE_DELETE(x) { if(x) { delete (x); (x) = NULL; } }

// シーンベースクラス
class SceneBase
{
public:
	virtual bool Initialize() = 0;	// 初期化
	virtual void Release() = 0;		// 解放
	virtual void Update() = 0;		// 更新
	virtual void Render() = 0;		// 描画

	// シーン名の取得
	virtual const char* GetName() const = 0;
};


//シーン管理
class SceneManager
{
public:
	SceneManager();
	~SceneManager();

	void Initialize();	// 初期化
	void Release();		// 解放
	void Update();		// 更新
	void Render();		// 描画

	// シーン遷移
	bool JumpScene(SceneBase* pScene);
	// シーン名取得
	const char* GetSceneName();

private:
	SceneBase* m_pScene;
};

