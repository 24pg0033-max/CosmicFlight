#include "Profiler.h"
#include "Memory.h"

#if USE_PROFILER
//ログの最大管理数
static const UINT LogMax = 1000;

Profiler::Profiler()
	:m_pRoot(nullptr),
	m_pCurrent(nullptr),
	m_LogCount(0)
	, m_Sprite(nullptr)
{
}

Profiler::~Profiler() = default;

void Profiler::Initialize() {
	//管理用ログのワークを生成
	m_pRoot = NEW Log[LogMax];
	memset(m_pRoot, 0, sizeof(Log) * LogMax);

	//ルートログの生成
	m_pCurrent = CreateLog("Root", nullptr);

	//処理バー用の２D描画を初期化
	m_Sprite = NEW Render2D;
	m_Shader.Create(L"Assets/2D.fx", "VSMain", "PSMain");
	m_Sprite->Initialize("Assets/Profiler/bar.png");
}

void Profiler::Finalize() {
	if (m_Sprite) {
		delete m_Sprite;
		m_Sprite = nullptr;
	}
	delete m_Sprite;
	if (m_pRoot) {
		delete[] m_pRoot;
		m_pRoot = nullptr;
	}
	m_pCurrent = nullptr;
}

void Profiler::Start() {
	//前フレームのログ情報を更新
	if (m_pRoot->pChild) {
		UpdateLog(m_pRoot->pChild);
	}
	m_Stopwatch.Reset();
}

void Profiler::Begin(const char* pName, Color color) {
	Log* pParent = m_pCurrent;
	Log* pBrother = nullptr;

	Log* pLog = pParent->pChild;
	while (pLog) {
		if (strcmp(pLog->pName, pName) == 0)break;
		pBrother = pLog;
		pLog = pLog->pBrother;
	}
	if (!pLog) {
		//指定のログの生成
		pLog = CreateLog(pName, pParent);
		pLog->Color = color;

		//親子関係の設定
		if (!pParent->pChild) {
			pParent->pChild = pLog;
		}
		else {
			pBrother->pBrother = pLog;
		}
	}
	pLog->BeginTime = m_Stopwatch.GetElapsedTime();
	m_pCurrent = pLog;
}

void Profiler::End(const char* pName) {
	if (strcmp(m_pCurrent->pName, pName)) {
		OutputDebugStringA("誤った終了タイミングです\n");
		return;
	}
	m_pCurrent->EndTime = m_Stopwatch.GetElapsedTime();
	float time = m_pCurrent->EndTime - m_pCurrent->BeginTime;
	//現状の計測時間を仮反映させる
	m_pCurrent->TempCount++;
	m_pCurrent->TempTotalTime += time;
	m_pCurrent->TempMinTime = min(time, m_pCurrent->TempMinTime);
	m_pCurrent->TempMaxTime = max(time, m_pCurrent->TempMaxTime);

	//終了したので親の階層に移動する
	m_pCurrent = m_pCurrent->pParent;
}

Profiler::Log* Profiler::CreateLog(const char* pName, Log* pParent) {
	if (m_LogCount == LogMax) {
		OutputDebugStringA("ログが不足しています\n");
		return nullptr;
	}
	Log* pLog = &m_pRoot[m_LogCount++];

	pLog->pParent = pParent;
	pLog->pName = pName;
	pLog->Color = pParent ? pParent->Color : Color::White;
	return pLog;
}

void Profiler::UpdateLog(Log* pLog) {
	while (pLog) {
		//実際に表示するデータに前回のフレームの結果を代入する
		pLog->Count = pLog->TempCount;
		pLog->ToTalTime = pLog->TempTotalTime;
		pLog->AvarageTime = pLog->Count != 0 ? (pLog->TempTotalTime / pLog->Count) : 0;
		pLog->MinTime = pLog->TempMinTime;
		pLog->MaxTime = pLog->TempMaxTime;
		//前回のフレーム情報をリセット
		pLog->BeginTime = 0;
		pLog->EndTime = 0;
		pLog->TempCount = 0;
		pLog->TempTotalTime = 0;

		//子階層も再帰的に実行していく
		if (pLog->pChild) {
			UpdateLog(pLog->pChild);
		}
		pLog = pLog->pBrother;
	}
}

//処理バーの目標値

const static float BarLength = 500.0f;

void Profiler::DrawProcessBar() {
	Log* pLog = m_pRoot;

	float x = 1.0f;
	float y = 1.0f;
	float height = 5.0f;



	//処理落ちとなる基準となるバーの描画
	m_Sprite->SetPositon({ x,y,0 });
	m_Sprite->SetSize(BarLength, height);
	m_Sprite->SetColor(1.0f, 1.0f, 1.0f);
	m_Sprite->Render();

	y += height;
	//各処理の処理バーを描画していく
	if (pLog->pChild) {
		DrawProcessBar(pLog->pChild, x, y);
	}
}

void Profiler::DrawProcessBar(const Log* pLog, float x, float y) {
	const float TargetFPS = 60.0f;
	const float TargetSec = 1.0f / TargetFPS;

	while (pLog) {
		//処理負荷の個別のバーの長さを計算して描画していく
		float time = pLog->ToTalTime;
		float length = BarLength * (time / TargetSec);
		float r, g, b;
		r = g = b = 0.0f;
		//カラーを取得
		switch (pLog->Color) {
		case Color::White:r = g = b = 1.0f;
			break;
		case Color::Black:
			break;
		case Color::Red:
			r = 1.0f;
			break;
		case Color::Green:
			g = 1.0f;
			break;
		case Color::Blue:
			b = 1.0f;
			break;
		}
		//処理の色とサイズに合わせてバーを描画

		//処理落ちとなる基準となるバーの描画
		m_Sprite->SetPositon({ x,y,0 });
		m_Sprite->SetSize(length, 5.0f);
		m_Sprite->SetColor(r, g, b);
		m_Sprite->Render();

		if (pLog->pChild) {
			DrawProcessBar(pLog->pChild, x, y);
		}
		//子階層を上から書いた後にずらす
		x += length;
		pLog = pLog->pBrother;

	}
}

#endif