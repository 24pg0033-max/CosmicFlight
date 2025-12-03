// WinMain.cpp
#include <Windows.h>
#include <tchar.h>
#include "../DxSystem.h"
#include "../Scene/InGame.h"
#include "../Input/Input.h"
#include "../DXShader.h"
#include "../Game/Game.h"
#include "../AudioSystem/SoundSystem.h"
#include "../RenderState.h"
#include "../Manager/StageManager.h"
#include "../GameSystem.h"
#include "../Memory/Memory.h"
#include "../AudioSystem/SoundManager.h"

#if USE_IMGUI
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_dx11.h"
#include "../imgui/imgui_impl_win32.h"
#endif

#if _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#pragma comment(lib, "winmm.lib")

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_CLOSE:
		PostMessage(hWnd, WM_DESTROY, 0, 0);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return(DefWindowProc(hWnd, message, wParam, lParam));
}


// 時間管理用
float DeltaTime = 40.0f;
LARGE_INTEGER g_Frequency;
LARGE_INTEGER g_LastTime;

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	srand(static_cast<unsigned int>(time(nullptr)));
//#if _DEBUG
//	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
//#endif
	// ウィンドウ生成
	TCHAR szWindowClass[] = TEXT("COSMIC FLIGHT");
	WNDCLASS wcex;
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	RegisterClass(&wcex);

	DWORD style = WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
	bool IsFullScreen = true;
	IsFullScreen = MessageBox(nullptr, L"フルスクリーンモードで起動しますか？", L"COSMIC FLIGHT", MB_YESNO) == IDYES;
	RECT WindowSize;
	if (IsFullScreen)
	{
		style = WS_POPUP;
		nCmdShow = SW_MAXIMIZE;
		AdjustWindowRect(&WindowSize, style, FALSE);
	}
	else {
		WindowSize = { 0,0,1280,720 };
		AdjustWindowRect(&WindowSize, style, FALSE);
	}

	int width = WindowSize.right - WindowSize.left;
	int height = WindowSize.bottom - WindowSize.top;

	HWND hWnd;
	hWnd = CreateWindow(szWindowClass,
		TEXT("CosmicFlight"),
		WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		0, 0, 1280, 720,
		NULL,
		NULL,
		hInstance,
		NULL);

	ShowWindow(hWnd, nCmdShow);

	QueryPerformanceFrequency(&g_Frequency);
	QueryPerformanceCounter(&g_LastTime);

	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	// デバイス初期化
	if (!DxSystem::Initialize(hWnd, 1280, 720))
	{
		return 0;
	}

#if USE_IMGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(DxSystem::Device, DxSystem::DeviceContext);
#endif

	Input::Initialize();
	Shader::InitializeSystem();
	RenderState::Initialize();
	SoundSystem::InitializeSound();
	SoundManager::Build();
	SoundManager::Initialize();
	SoundManager::BGMPlay(Title_BGM);


	// シーン初期化
	GameInitialize();	

	//メインループ
	MSG hMsg = { 0 };
	DWORD timeOld = timeGetTime();

	while (hMsg.message != WM_QUIT) {

		DxSystem::Clear();

		if (PeekMessage(&hMsg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&hMsg);
			DispatchMessage(&hMsg);
		}
		else
		{
			LARGE_INTEGER currentTime;
			QueryPerformanceCounter(&currentTime);
			DeltaTime = static_cast<float>(currentTime.QuadPart - g_LastTime.QuadPart) / g_Frequency.QuadPart;
			g_LastTime = currentTime;

			// 落ちすぎ防止
			const float MAX_DELTA_TIME = 1.0f / 60.0f * 5;
			if (DeltaTime > MAX_DELTA_TIME)
				DeltaTime = MAX_DELTA_TIME;
#if USE_IMGUI
			// imgui update
			ImGuiIO& io = ImGui::GetIO();
			io.DeltaTime = DeltaTime;
			io.DisplaySize.x = (float)DxSystem::ScreenWidth;
			io.DisplaySize.y = (float)DxSystem::ScreenHeight;
			io.MouseDown[0] = GetKeyState(VK_LBUTTON) & 0x8000;
			io.MouseDown[1] = GetKeyState(VK_RBUTTON) & 0x8000;
			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();
#endif

			Input::Update();
			SoundManager::Update();

			// 更新・描画
			GameMain();

#if USE_IMGUI
			// Render dear imgui into screen
			ImGui::Render();
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif

			// 画面更新
			DxSystem::Flip();
		}
	}

	GameRelease();
	SoundManager::Instance().Finalize();
	SoundManager::Release();
	Shader::ReleaseSystem();
	RenderState::Release();
	DxSystem::Release();
	CoUninitialize();

#if USE_IMGUI
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
#endif

#if USE_MEMORY_DEBUG
	ShowMemory();
#endif

	return 0;
}
