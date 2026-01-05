#pragma once

#include <string>
#include <vector>
#include <shlobj.h>
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")
#include "StringConverter.h"
namespace FileUtils {

    inline std::string appDataPath;

    inline std::string configPath;

    inline std::string modulePath;

    inline std::string soundPath;

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
        return appDataPath + "\\Configs";
    }

    inline std::string GetSoundPath()
    {
        return modulePath + "\\Assets\\Sounds";
    }

    inline std::string GetSoundPath(std::string soundName)
    {
        return soundPath + "\\" + soundName;
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
        modulePath = GetModulePath(hMod);
        soundPath = GetSoundPath();
        //MessageBoxA(NULL, "StartGetAppDataPath", "appDataPath", MB_OK);
        appDataPath = GetAppDataPath();
        //MessageBoxA(NULL, appDataPath.c_str(), "appDataPath", MB_OK);
        if(appDataPath.empty()) // 如果获取失败，则使用默认路径（可能是多用户操作系统）
            appDataPath = "C:\\InfiniteGUI";
        //MessageBoxA(NULL, appDataPath.c_str(), "appDataPath", MB_OK);
        configPath = GetConfigPath();
        InitBasePath(modulePath);
    }

};