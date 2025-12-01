#pragma once

#include <string>
#include <vector>
#include <shlobj.h>
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")
#include "StringConverter.h"
namespace FileUtils {

    inline std::string appDataPath = "";

    inline std::string configPath = "";

    inline std::string modulePath = "";

    inline std::string soundPath = "";

    inline std::string GetAppDataPath()
    {
        PWSTR path = nullptr;
        SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &path);

        char buffer[MAX_PATH];
        wcstombs(buffer, path, MAX_PATH);
        CoTaskMemFree(path);

        std::string p = std::string(buffer) + "\\InfiniteGUI";
        CreateDirectoryA(p.c_str(), NULL);

        return p;
    }

    inline std::string GetConfigPath()
    {
        //如果没有Configs文件夹，则创建
        std::string p = appDataPath + "\\Configs";
        CreateDirectoryA(p.c_str(), NULL);
        return p + "\\config.json";
    }

    inline std::string GetSoundPath()
    {
        return modulePath + "\\Assets\\Sounds";
    }

    inline std::string GetSoundPath(std::string soundName)
    {
        return soundPath + "\\" + soundName;
    }

    struct FontInfo {
        std::string name;
        std::wstring path;
    };

    inline std::vector<FontInfo> GetFontsFromDirectory(const std::wstring& directory) {
        std::vector<FontInfo> fontInfos;

        WIN32_FIND_DATA findFileData;
        HANDLE hFind = FindFirstFile((directory + L"\\*").c_str(), &findFileData);

        if (hFind == INVALID_HANDLE_VALUE)
            return fontInfos;

        do {
            if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                std::wstring filename = findFileData.cFileName;
                if (filename.find(L".ttf") != std::string::npos || filename.find(L".otf") != std::string::npos) {
                    FontInfo fontInfo;
                    fontInfo.name = StringConverter::WstringToUtf8(filename);
                    fontInfo.path = directory + L"\\" + filename;
                    fontInfos.push_back(fontInfo);
                }
            }
        } while (FindNextFile(hFind, &findFileData) != 0);

        FindClose(hFind);
        return fontInfos;
    }

    inline std::string GetModulePath(HMODULE hMod)
    {
        char path[MAX_PATH];
        GetModuleFileNameA(hMod, path, MAX_PATH);

        // 将 DLL 名字去掉，只保留路径
        std::string fullPath(path);
        size_t pos = fullPath.find_last_of("\\/");
        if (pos != std::string::npos)
            fullPath = fullPath.substr(0, pos);

        return fullPath;
    }

    inline void InitBasePath(std::string basePath)
    {
        PathRemoveFileSpecA(basePath.data());
    }

    inline void InitPaths(HMODULE hMod)
    {
        appDataPath = GetAppDataPath();
        configPath = GetConfigPath();
        modulePath = GetModulePath(hMod);
        soundPath = GetSoundPath();
        InitBasePath(modulePath);
    }

};