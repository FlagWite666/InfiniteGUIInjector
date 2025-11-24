#include "GameStateDetector.h"
#include "App.h"
#include <windows.h>
void GameStateDetector::Update()
{
	isInGame = !IsMouseCursorVisible();
}

void GameStateDetector::Load(const nlohmann::json& j)
{
	LoadItem(j);
	if (j.contains("hideItemInGui")) hideItemInGui = j["hideItemInGui"].get<bool>();
}

void GameStateDetector::Save(nlohmann::json& j) const
{
	SaveItem(j);
	j["hideItemInGui"] = hideItemInGui;
}

void GameStateDetector::DrawSettings()
{
}

//bool GameStateDetector::IsPaused() const
//{
//	return false;
//}
//
//bool GameStateDetector::IsInventoryOpen() const
//{
//	return false;
//}
//
//bool GameStateDetector::IsChatTyping() const
//{
//	return false;
//}
//
//bool GameStateDetector::IsInGUI() const
//{
//	return false;
//}

bool GameStateDetector::IsNeedHide() const 
{
	return !isInGame && hideItemInGui;
}

bool GameStateDetector::IsInGame() const
{
	return isInGame;
}

//bool GameStateDetector::DetWindowCenter() const
//{
//	RECT rc = { 0, 0 };
//	GetWindowRect(App::Instance().clientHwnd, &rc);
//	int x = (int)floor((rc.right - rc.left) / 2) + rc.left;
//	int y = (int)floor((rc.bottom - rc.top) / 2) + rc.top;
//	static int smx = GetSystemMetrics(SM_CXSCREEN);
//	static int smy = GetSystemMetrics(SM_CYSCREEN);
//	if (rc.right != smx && rc.bottom != smy)
//		y += 12;
//
//	POINT curPos = { x, y };
//
//	RECT rc2 = { 0, 0 };
//
//	rc2.left = x - centerLevel + 1;
//	rc2.top = y - centerLevel;
//	rc2.right = x + centerLevel;
//	rc2.bottom = y +centerLevel + 1;
//	GetCursorPos(&curPos);
//	return PtInRect(&rc2, curPos);
//
//}


bool GameStateDetector::IsMouseCursorVisible() const
{
	CURSORINFO ci;
	ci.cbSize = sizeof(ci);

	if (GetCursorInfo(&ci))
	{
		return (ci.flags & CURSOR_SHOWING) != 0;
	}
	return false;
}