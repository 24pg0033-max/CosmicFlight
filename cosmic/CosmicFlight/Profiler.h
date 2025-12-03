#pragma once
#include <Windows.h>
#include "Stopwatch.h"
#include "Render2D.h"

#if _DEBUG
#define USE_PROFILER	(1)
#else
#define USE_PROFILER	(0)
#endif

#if USE_PROFILER
//プロファイラ
class Profiler {
public:
	//表示カラー
	enum class Color {
		White,
		Black,
		Red,
		Green,
		Blue,
	};
	//計測用ログ
	struct Log {
		Log* pParent;	//親階層のログ
		Log* pBrother;	//同階層のログ
		Log* pChild;	//子階層のログ

		float BeginTime;	//計測開始時間
		float EndTime;		//計測終了時間
		UINT TempCount;		//計測回数
		float TempTotalTime;//合計処理時間
		float TempMinTime;	//最小処理時間
		float TempMaxTime;	//最大処理時間

		const char* pName;	//名前
		Color Color;		//表示色
		UINT Count;			//計測回数
		float ToTalTime;	//合計処理時間
		float AvarageTime;	//平均処理時間
		float MinTime;		//最小処理時間
		float MaxTime;		//最大処理時間
	};

public:
	void Initialize();//初期化
	void Finalize();//終了処理
	void Start();//フレーム開始通知
	void Begin(const char* pName, Color color);//計測開始
	void End(const char* pName);//計測終了
	void DrawProcessBar();//プロセスバーの描画
	const Log* GetRootLog()const { return m_pRoot; }//ログの取得

private:
	Profiler();
	~Profiler();
	//ログの情報生成
	Log* CreateLog(const char* pName, Log* pParent);
	//ログの更新
	void UpdateLog(Log* pLog);
	//プロセスバーの描画
	void DrawProcessBar(const Log* pLog, float x, float y);

public:
	static Profiler& Instance() {
		static Profiler instance;
		return instance;
	}
private:
	Stopwatch m_Stopwatch;	//時間計測用
	Log* m_pRoot;			//全体の管理用ルートログ
	Log* m_pCurrent;		//現在の階層保持用ログ	
	UINT m_LogCount;		//ログの利用数

	Render2D* m_Sprite;		//処理バー描画用
	Shader m_Shader;		//処理バー用シェーダ
};

//スコープ管理用クラス
class ScopeProfiler {
public:
	ScopeProfiler(const char* pName, Profiler::Color color)
		:m_pName(pName)
	{
		Profiler::Instance().Begin(pName, color);
	}
	~ScopeProfiler() {
		Profiler::Instance().End(m_pName);
	}
private:
	const char* m_pName;
};
#define PROFILER()			Profiler::Instance()
#define PROFILER_INIT()		PROFILER().Initialize()
#define PROFILER_FIN()		PROFILER().Finalize()
#define PROFILER_START()	PROFILER().Start()
#define PROFILER_DRAW()		PROFILER().DrawProcessBar()

#define SCOPE_PROFILER(name,color)	\
	ScopeProfiler name##Profiler(#name,color)

#else
#define PROFILER()			((void)0)
#define PROFILER_INIT()		((void)0)
#define PROFILER_FIN()		((void)0)
#define PROFILER_START()	((void)0)
#define PROFILER_DRAW()		((void)0)

#define SCOPE_PROFILER(...)	((void)0)

#endif