#include <windows.h>
#include <stdio.h>
#include <mutex>
#include "Memory.h"

#if USE_MEMORY_DEBUG
//メモリ管理情報
struct MemoryInfo {
	size_t size;		//確保サイズ
	char filename[252];	//ファイル名
	int line;			//ライン

	MemoryInfo* pNext;	//次のメモリ情報
	MemoryInfo* pPrev;	//前のメモリ情報
};
//確保リスト用
MemoryInfo* g_pHead = nullptr;
MemoryInfo* g_pTail = nullptr;
//メモリの確報サイズ
size_t g_TotalSize = 0;
#endif

#if USE_MEMORY_DEBUG
void* Alloc(size_t size, const char* pFile, int line)
#else
void* Alloc(size_t size)
#endif
{
#if USE_MEMORY_DEBUG
	void* ptr = malloc(size + sizeof(MemoryInfo));
	if (ptr) {
		MemoryInfo* pInfo = reinterpret_cast<MemoryInfo*>(ptr);
		//メモリ情報を先頭に書き込んでいく
		pInfo->size = size;
		pInfo->pNext = nullptr;
		pInfo->pPrev = g_pTail;
		//確保場所を保存しておく
		strcpy_s(pInfo->filename, pFile);
		pInfo->line = line;

		//先頭がない場合
		if (g_pHead == nullptr) {
			g_pHead = pInfo;
		}
		//末尾が存在する場合
		if (g_pTail) {
			g_pTail->pNext = pInfo;
		}
		//確保した場所を末尾とする
		g_pTail = pInfo;

		//合計確保数を加算する
		g_TotalSize += size;
		//実際の利用可能なメモリ位置にずらす（MemoryInfoの後ろのアドレスを返す）
		ptr = pInfo + 1;

	}
#else
	void* ptr = malloc(size);
#endif
	return ptr;
}

//メモリの解放
void Free(void* ptr) {
#if USE_MEMORY_DEBUG
	if (ptr) {
		//確保した先頭アドレスの位置にずらす
		MemoryInfo* pInfo = reinterpret_cast<MemoryInfo*>(ptr) - 1;
		//確保サイズを減らす
		g_TotalSize -= pInfo->size;

		//前後のリンクリストをつなぎ合わせる
		if (pInfo->pNext) {
			pInfo->pNext->pPrev = pInfo->pPrev;
		}
		if (pInfo->pPrev) {
			pInfo->pPrev->pNext = pInfo->pNext;
		}
		//先頭と末端が変化するかチェックしておく
		if (g_pHead == pInfo) {
			g_pHead = pInfo->pNext;
		}
		if (g_pTail == pInfo) {
			g_pTail = pInfo->pPrev;
		}
		//解放するアドレスの位置は引数のptrじゃなくて、ずらしたpInfo
		free(pInfo);

	}
#else
	free(ptr);
#endif

}

//確保済みのメモリサイズを取得
size_t AllocSize() {
#if USE_MEMORY_DEBUG
	return g_TotalSize;
#else
	return 0;
#endif
}

#if USE_MEMORY_DEBUG
//メモリリークのチェック
void ShowMemory() {
	OutputDebugStringA("==================================================\n");
	MemoryInfo* pInfo = g_pHead;
	while (pInfo) {
		char temp[256];
		sprintf_s(temp, "%s(%d): size:%d\n", pInfo->filename, pInfo->line, pInfo->size);
		//出力ウィンドウに出す
		OutputDebugStringA(temp);

		pInfo = pInfo->pNext;
	}
	OutputDebugStringA("==================================================\n");
}

void ShowMemoryToFile(const char* outputPath) {
	FILE* fp = nullptr;
	fopen_s(&fp, outputPath, "w");
	if (!fp) return;

	MemoryInfo* pInfo = g_pHead;
	while (pInfo) {
		fprintf(fp, "%s(%d): size:%zu\n", pInfo->filename, pInfo->line, pInfo->size);
		pInfo = pInfo->pNext;
	}

	fclose(fp);
}

#endif


#if USE_MEMORY_DEBUG
void* operator new(size_t size, const char* pFile, int line) {
	return Alloc(size, pFile, line);
}

void* operator new[](size_t size, const char* pFile, int line) {
	return Alloc(size, pFile, line);
}
void* operator new(size_t size)
{
	return Alloc(size, __FILE__, __LINE__);
}
void* operator new[](size_t size)
{
	return Alloc(size, __FILE__, __LINE__);
}
#else
void* operator new(size_t size)
{
	return Alloc(size);
}
void* operator new[](size_t size)
{
	return Alloc(size);
}
#endif
void operator delete(void* ptr) {
	Free(ptr);
}

void operator delete[](void* ptr) {
	Free(ptr);
}

#if USE_MEMORY_DEBUG
void operator delete(void* ptr, const char* pFile, int line) {
	Free(ptr);
}

void operator delete[](void* ptr, const char* pFile, int line) {
	Free(ptr);
}
#endif

#if USE_MEMORY_DEBUG
void operator delete(void* ptr, size_t size) {
	Free(ptr);
}
void operator delete[](void* ptr, size_t size) {
	Free(ptr);
}
#endif

