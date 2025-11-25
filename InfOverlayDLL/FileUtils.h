#pragma once

#include <string>
#include <vector>
#include <shlobj.h>
#include "StringConverter.h"
namespace FileUtils {

    static std::string GetRunPath()
    {
        PWSTR path = nullptr;
        SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &path);

        char buffer[MAX_PATH];
        wcstombs(buffer, path, MAX_PATH);
        CoTaskMemFree(path);

        std::string p = std::string(buffer) + "\\InfOverlay";
        CreateDirectoryA(p.c_str(), NULL);

        return p;
    }

    static std::string GetConfigPath()
    {
        //如果没有Configs文件夹，则创建
        std::string p = GetRunPath() + "\\Configs";
        CreateDirectoryA(p.c_str(), NULL);
        return GetRunPath() + "\\Configs\\config.json";
    }

    static std::string GetSoundPath()
    {
        return GetRunPath() + "\\Sounds";
    }

    static std::string GetSoundPath(std::string soundName)
    {
        return GetRunPath() + "\\Sounds\\" + soundName;
    }

    struct FontInfo {
        std::string name;
        std::wstring path;
    };

    static std::vector<FontInfo> GetFontsFromDirectory(const std::wstring& directory) {
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



};