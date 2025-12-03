#pragma once

#if _DEBUG
#define USE_MEMORY_DEBUG (1)
#else
#define USE_MEMORY_DEBUG (0)
#endif

#if USE_MEMORY_DEBUG

#define NEW				new(__FILE__, __LINE__)
#define ALLOC(size) Alloc(size,__FILE__,__LINE__)

//ファイルパスや行列番号の指定可能なパス
void* operator new(size_t size, const char* pFile, int line);
void* operator new[](size_t size, const char* pFile, int line);
//メモリの確保
void* Alloc(size_t size, const char* pFile, int line);

#else
//メモリの確保
void* Alloc(size_t size);
#define NEW new
#endif //USE_MEMORY_DEBUG

//メモリの解放
void Free(void* ptr);

#if USE_MEMORY_DEBUG

//メモリ確保サイズの取得
size_t AllocSize();
//メモリリークのチェック
void ShowMemory();
void ShowMemoryToFile(const char* outputPath);
#endif