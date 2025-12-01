#pragma once
#include <GL/glew.h>
#include <Windows.h>
#include <string>
#include "imgui\imgui.h"

struct version
{
	int major;
	int minor;
	int build;
};

struct texture
{
	GLuint id;
	int width;
	int height;
};

class App
{
public:
	std::string appName = u8"无限GUI";
	version appVersion = { 0, 9, 0 };
	std::string appAuthor = "QC_Max";
	std::string appDescription = "Infinite GUI is a library that allows you to create overlays for in-game information.";
	std::string announcement = u8"这里没有公告捏...";
	version cloudVersion = { 0, 0, 0 };
	std::wstring versionUrl = L"https://gitee.com/qc_max/InfiniteGUI/raw/master/version.json";
	std::wstring announcementUrl = L"https://gitee.com/qc_max/InfiniteGUI/raw/master/announcement.txt";
	HWND clientHwnd = nullptr;
	ImFont* iconFont;
	texture logoTexture;
	static App& Instance()
	{
		static App instance;
		return instance;
	}

	void GetAnnouncement();

	bool CheckUpdate();
};