///@file
#include <windows.h>
#include "SceneManager.h"
#include "../Memory/Memory.h"

SceneManager::SceneManager()
	:m_pScene(NULL)
{}

SceneManager::~SceneManager(){
	Release();
}

void SceneManager::Initialize(){
	m_pScene = NULL;
}

void SceneManager::Release(){
	// 最後は解放して終わる
	if (m_pScene) m_pScene->Release();
	SAFE_DELETE(m_pScene);
}

void SceneManager::Update(){
	if (m_pScene) m_pScene->Update();
}

void SceneManager::Render(){
	if (m_pScene) m_pScene->Render();
}

bool SceneManager::JumpScene(SceneBase* pScene){
	// 現在のシーンを解放する
	if (m_pScene)
	{
		m_pScene->Release();
		SAFE_DELETE(m_pScene);
	}
	//ShowMemory();

	bool ret = true;
	// 次のシーンがあれば初期化する
	if (pScene)
	{
		ret = pScene->Initialize();
	}
	m_pScene = pScene;
	return ret;
}

const char* SceneManager::GetSceneName(){
	if (m_pScene) return m_pScene->GetName();

	return "Unknown";
}
