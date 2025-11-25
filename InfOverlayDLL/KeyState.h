#pragma once
#include <Windows.h>
#include <WinUser.h>

enum SendInputMode
{
	SC_MOUSE_EVENT = 0,
	SC_SEND_INPUT = 1,
	SC_POST_MESSAGE = 2
};

bool GetKeyDown(int key);
bool GetKeyUp(int key);
bool GetKeyClick(int key);
void SetKeyDown(int key, int mode = SC_POST_MESSAGE);
void SetKeyUp(int key, int mode = SC_POST_MESSAGE);
void SetKeyClick(int key, int sleep = 20, int mode = SC_POST_MESSAGE);

enum Click
{
	Right = VK_RBUTTON,
	Left = VK_LBUTTON
};

