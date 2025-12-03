#pragma once
#include <Windows.h>
#include <XInput.h>

class Input
{
private:
	static const int MAX_CONTROLLERS = 4;
	static const int KEY_MAX = 256;
	static const int INVALID_PAD_USER_INDEX = -1;
	//	ボタン、キーのマップ
	static int BUTTON_MAP[16];
	static int KEY_MAP[16];

	//キーボードの入力情報
	struct KeyInfo {
		bool KeyState[KEY_MAX];
		bool keyStateOld[KEY_MAX];
	};
	static KeyInfo s_keyState;

	//	入力状態保存用
	struct States {
		float AxisLX;
		float AxisLY;
		float AxisRX;
		float AxisRY;

		float LeftTrigger;
		float RightTrigger;

		bool IsConnected;
		bool IsConnectedOld;

		int Buttons[16];
	};
	static States states[MAX_CONTROLLERS];
	static int padIndexList[MAX_CONTROLLERS];

	static void UpdatePadUserIndex(void);

public:
	//	ボタン名
	enum BUTTON {
		UP = 0,
		DOWN,
		LEFT,
		RIGHT,
		START,
		BACK,
		LSTICK,
		RSTICK,
		LB,
		RB,
		A,
		B,
		X,
		Y,
	};

	//	ボタンの状態
	static const int RELEASED = 0;		// 離し
	static const int PRESSED = 1;		// 押し
	static const int JUST_RELEASED = 2;	// 離した瞬間
	static const int JUST_PRESSED = 3;	// 押した瞬間


	static void Initialize();
	static void Update();


	//	状態取得/////////////////////
	//ゲームパッド
	static float GetAxisX(int index = 0);
	static float GetAxisY(int index = 0);
	static float GetAxisRX(int index = 0);
	static float GetAxisRY(int index = 0);
	static int GetPadUserIndex(int index);
	static bool GetPadConnect(int index);
	static int GetButton_Press(int index, BUTTON button);
	static bool GetButton_Push(int index, BUTTON button);
	static bool GetButton_Release(int index, BUTTON button);

	//キーボード
	static bool GetKeyPress(int key);
	static bool GetKeyPush(int key);
	static bool GetKeyRelease(int key);

	//キーボード＆ゲームパッド
	static bool Key_Pad_Press(int key, BUTTON button, int index = 0);
	static bool Key_Pad_Push(int key, BUTTON button, int index = 0);
	static bool Key_Pad_Release(int key, BUTTON button, int index = 0);

	//	振動設定
	static void SetVibration(int index, float left, float right);
};

