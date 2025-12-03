// Input.cpp
#include <Windows.h>
#include <math.h>
#include "Input.h"

#pragma comment( lib,"xinput.lib")

_XINPUT_STATE		XInput_State;
SHORT val;

Input::States Input::states[MAX_CONTROLLERS];
Input::KeyInfo Input::s_keyState;
int Input::BUTTON_MAP[16];
int Input::KEY_MAP[16];
int Input::padIndexList[MAX_CONTROLLERS];


void Input::UpdatePadUserIndex(void)
{
	//切断
	for (int userIndex = 0; userIndex < MAX_CONTROLLERS; userIndex++) {
		if (!states[userIndex].IsConnected && states[userIndex].IsConnectedOld) {
			for (int index = 0; index < MAX_CONTROLLERS; index++) {
				if (padIndexList[index] == userIndex) {
					padIndexList[index] = INVALID_PAD_USER_INDEX;
					break;
				}
			}
		}
	}

	//接続
	for (int userIndex = 0; userIndex < MAX_CONTROLLERS; userIndex++) {
		if (states[userIndex].IsConnected && !states[userIndex].IsConnectedOld) {
			for (int index = 0; index < MAX_CONTROLLERS; index++) {
				if (padIndexList[index] == INVALID_PAD_USER_INDEX) {
					padIndexList[index] = userIndex;
					break;
				}
			}
		}
	}


}

void Input::Initialize()
{
	int init_map[16] = {
	XINPUT_GAMEPAD_DPAD_UP,
	XINPUT_GAMEPAD_DPAD_DOWN,
	XINPUT_GAMEPAD_DPAD_LEFT,
	XINPUT_GAMEPAD_DPAD_RIGHT,
	XINPUT_GAMEPAD_START,
	XINPUT_GAMEPAD_BACK,
	XINPUT_GAMEPAD_LEFT_THUMB,
	XINPUT_GAMEPAD_RIGHT_THUMB,
	XINPUT_GAMEPAD_LEFT_SHOULDER,
	XINPUT_GAMEPAD_RIGHT_SHOULDER,
	XINPUT_GAMEPAD_A,
	XINPUT_GAMEPAD_B,
	XINPUT_GAMEPAD_X,
	XINPUT_GAMEPAD_Y,
	};

	int init_keymap[16] = {
	VK_UP,		//UP,
	VK_DOWN,	//DOWN,
	VK_LEFT,	//LEFT,
	VK_RIGHT,	//RIGHT,
	VK_RETURN,	//START,
	VK_BACK,	//BACK,
	VK_SHIFT,	//LSTICK,
	VK_TAB,		//RSTICK,
	VK_CONTROL,	//LB,
	VK_SPACE,	//RB,
	'Z',		//A,
	'X',		//B,
	'C',		//X,
	'V',		//Y,
	};

	for (int i = 0; i < 16; i++) {
		BUTTON_MAP[i] = init_map[i];
		KEY_MAP[i] = init_keymap[i];
	}

	for (int i = 0; i < MAX_CONTROLLERS; i++) {
		states[i].IsConnected = false;
		states[i].IsConnectedOld = false;
		padIndexList[i] = INVALID_PAD_USER_INDEX;
	}
}

void Input::Update()
{
	for (DWORD i = 0; i < MAX_CONTROLLERS; i++) {

		//ステートの更新
		states[i].IsConnectedOld = states[i].IsConnected;

		_XINPUT_STATE state;
		ZeroMemory(&state, sizeof(_XINPUT_STATE));
		DWORD dwResult = XInputGetState(i, &state);
		if (dwResult == ERROR_SUCCESS) {
			//	左スティック
			{
				states[i].AxisLX = state.Gamepad.sThumbLX / 32767.0f;
				states[i].AxisLY = state.Gamepad.sThumbLY / 32767.0f;
				// 1209 キーボード
				if (GetKeyState('W') < 0) states[i].AxisLY += 1;
				if (GetKeyState('A') < 0) states[i].AxisLX += -1;
				if (GetKeyState('S') < 0) states[i].AxisLY += -1;
				if (GetKeyState('D') < 0) states[i].AxisLX += 1;

				float d = sqrtf(states[i].AxisLX * states[i].AxisLX + states[i].AxisLY * states[i].AxisLY);
				if (d > 1.0f) {
					states[i].AxisLX /= d;
					states[i].AxisLY /= d;
				}
				//	デッドゾーン
				//float d = sqrtf(states[i].AxisLX*states[i].AxisLX + states[i].AxisLY*states[i].AxisLY);
				if (d < 0.2f) {
					states[i].AxisLX = 0;
					states[i].AxisLY = 0;
				}
			}
			//	右スティック
			{
				states[i].AxisRX = state.Gamepad.sThumbRX / 32767.0f;
				states[i].AxisRY = state.Gamepad.sThumbRY / 32767.0f;

				if (GetKeyState(VK_NUMPAD8) < 0) states[i].AxisRY += 1;
				if (GetKeyState(VK_NUMPAD4) < 0) states[i].AxisRX += -1;
				if (GetKeyState(VK_NUMPAD2) < 0) states[i].AxisRY += -1;
				if (GetKeyState(VK_NUMPAD6) < 0) states[i].AxisRX += 1;

				float d = sqrtf(states[i].AxisRX * states[i].AxisRX + states[i].AxisRY * states[i].AxisRY);
				if (d > 1.0f) {
					states[i].AxisRX /= d;
					states[i].AxisRY /= d;
				}

				//	デッドゾーン
				//float d = sqrtf(states[i].AxisRX*states[i].AxisRX + states[i].AxisRY*states[i].AxisRY);
				if (d < 0.2f) {
					states[i].AxisRX = 0;
					states[i].AxisRY = 0;
				}
			}
			//　トリガー
			{
				states[i].LeftTrigger = state.Gamepad.bLeftTrigger / 255.0f;
				//if (GetKeyState('Q') < 0) states[i].LeftTrigger = 1;
				//	デッドゾーン
				if (states[i].LeftTrigger < 0.2f) {
					states[i].LeftTrigger = 0;
				}

				states[i].RightTrigger = state.Gamepad.bRightTrigger / 255.0f;
				//if (GetKeyState('E') < 0) states[i].RightTrigger = 1;
				//	デッドゾーン
				if (states[i].RightTrigger < 0.2f) {
					states[i].RightTrigger = 0;
				}
			}

			//	ボタン
			for (int b = 0; b < 16; b++) {
				int old = states[i].Buttons[b] & 0x01;
				//1209
				states[i].Buttons[b] = 0;
				if (state.Gamepad.wButtons & BUTTON_MAP[b]) states[i].Buttons[b] = 1;
				//if (GetKeyState(KEY_MAP[b]) < 0) states[i].Buttons[b] = 1;

				//	瞬間判定
				if (states[i].Buttons[b] != old) states[i].Buttons[b] |= 0x02;
			}

			states[i].IsConnected = true;
		}
		else {
			//	非接続の場合、０番のみキーボードで代用
			if (i == 0) {
				//	左スティック
				{
					states[i].AxisLX = 0;
					states[i].AxisLY = 0;
					if (GetKeyState('W') < 0) states[i].AxisLY += 1;
					if (GetKeyState('A') < 0) states[i].AxisLX += -1;
					if (GetKeyState('S') < 0) states[i].AxisLY += -1;
					if (GetKeyState('D') < 0) states[i].AxisLX += 1;
					float d = sqrtf(states[i].AxisLX * states[i].AxisLX + states[i].AxisLY * states[i].AxisLY);
					if (d > 1.0f) {
						states[i].AxisLX /= d;
						states[i].AxisLY /= d;
					}
				}
				//	右スティック
				{
					states[i].AxisRX = 0;
					states[i].AxisRY = 0;
					if (GetKeyState(VK_NUMPAD8) < 0) states[i].AxisRY += 1;
					if (GetKeyState(VK_NUMPAD4) < 0) states[i].AxisRX += -1;
					if (GetKeyState(VK_NUMPAD2) < 0) states[i].AxisRY += -1;
					if (GetKeyState(VK_NUMPAD6) < 0) states[i].AxisRX += 1;
					float d = sqrtf(states[i].AxisRX * states[i].AxisRX + states[i].AxisRY * states[i].AxisRY);
					if (d > 1.0f) {
						states[i].AxisRX /= d;
						states[i].AxisRY /= d;
					}
				}

				//	トリガー
				{
					states[i].LeftTrigger = 0;
					states[i].RightTrigger = 0;
					if (GetKeyState('Q') < 0) states[i].LeftTrigger = 1;
					if (GetKeyState('E') < 0) states[i].RightTrigger = 1;
				}

				//	ボタン
				for (int b = 0; b < 16; b++) {
					int old = states[i].Buttons[b] & 0x01;
					if (GetKeyState(KEY_MAP[b]) < 0) states[i].Buttons[b] = 1;
					else  states[i].Buttons[b] = 0;
					//	瞬間判定
					if (states[i].Buttons[b] != old) states[i].Buttons[b] |= 0x02;
				}
			}
			states[i].IsConnected = false;
		}
	}

	//キーボードの入力情報更新
	BYTE keyboardState[256]{};
	if (!GetKeyboardState(keyboardState))return;
	for (int i = 0; i < KEY_MAX; i++) {
		//入力されている場合はキーのビットを立てる
		bool isInput = (keyboardState[i] & 0x80) != 0;

		//ステートの更新
		s_keyState.keyStateOld[i] = s_keyState.KeyState[i];
		s_keyState.KeyState[i] = isInput;

	}

	UpdatePadUserIndex();

}

float Input::GetAxisX(int index)
{
	return states[index].AxisLX;
}

float Input::GetAxisY(int index)
{
	return states[index].AxisLY;
}

float Input::GetAxisRX(int index)
{
	return states[index].AxisRX;
}

float Input::GetAxisRY(int index)
{
	return states[index].AxisRY;
}

int Input::GetPadUserIndex(int index)
{
	if (index<0 || index>MAX_CONTROLLERS)return -1;
	return padIndexList[index];
}

bool Input::GetPadConnect(int index)
{
	int userIndex = GetPadUserIndex(index);
	return states[index].IsConnected;
}

int Input::GetButton_Press(int index, BUTTON button)
{
	return states[index].Buttons[button];
}

bool Input::GetButton_Push(int index, BUTTON button)
{
	if (states[index].Buttons[button] == JUST_PRESSED)return true;
	return false;
}

bool Input::GetButton_Release(int index, BUTTON button)
{
	if (states[index].Buttons[button] == JUST_RELEASED) return true;
	return false;
}

bool Input::GetKeyPress(int key)
{
	return s_keyState.KeyState[key];
}

bool Input::GetKeyPush(int key)
{
	return s_keyState.KeyState[key] && !s_keyState.keyStateOld[key];
}

bool Input::GetKeyRelease(int key)
{
	return !s_keyState.KeyState[key] && s_keyState.keyStateOld[key];
}

bool Input::Key_Pad_Press(int key, BUTTON button, int index)
{
	if (GetButton_Press(index, button))return true;
	if (GetKeyPress(key))return true;
	return false;
}

bool Input::Key_Pad_Push(int key, BUTTON button, int index)
{
	if (GetButton_Push(index, button))return true;
	if (GetKeyPush(key))return true;
	return false;
}

bool Input::Key_Pad_Release(int key, BUTTON button, int index)
{
	if (GetButton_Release(index, button))return true;
	if (GetKeyRelease(key))return true;
	return false;
}

void Input::SetVibration(int index, float left, float right)
{
	if (left < 0) left = 0;
	if (left > 1) left = 1;
	if (right < 0) right = 0;
	if (right > 1) right = 1;

	XINPUT_VIBRATION vibration;
	ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
	vibration.wLeftMotorSpeed = (WORD)(left * 65535); // use any value between 0-65535 here
	vibration.wRightMotorSpeed = (WORD)(right * 65535); // use any value between 0-65535 here
	XInputSetState(index, &vibration);
}
