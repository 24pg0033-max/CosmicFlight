#pragma once

#include <windows.h>


// ストップウォッチ
class Stopwatch
{
public:
	// コンストラクタ
	Stopwatch(void);

	// リセット
	void Reset(void);
	// 経過時間の取得
	double GetElapsedTime(void);

private:
	LARGE_INTEGER	m_Freq;		// 周波数
	LARGE_INTEGER	m_Start;	// 開始時間
};
