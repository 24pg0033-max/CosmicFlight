#include "Stopwatch.h"



// コンストラクタ
Stopwatch::Stopwatch(void)
{
	QueryPerformanceFrequency(&m_Freq);
	Reset();
}

// リセット
void Stopwatch::Reset(void)
{
	QueryPerformanceCounter(&m_Start);
}

// 経過時間の取得
double Stopwatch::GetElapsedTime(void)
{
	LARGE_INTEGER End;
	QueryPerformanceCounter(&End);

	const LONGLONG TICK_DELTA = 1000000000;
	const double INV_TICK_DELTA = 1.0 / (double)TICK_DELTA;

	LONGLONG elapsed = (End.QuadPart - m_Start.QuadPart);
	return (double)((elapsed * TICK_DELTA) / m_Freq.QuadPart) * INV_TICK_DELTA;
}
