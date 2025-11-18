#pragma once

#include <string>
#include "StringConverter.h"
namespace FileManager {

    static std::string GetRunPath()
    {
        char* appdata = nullptr;
        size_t len;
        _dupenv_s(&appdata, &len, "APPDATA");
        std::string p = std::string(appdata ? appdata : "") + "\\InfOverlay";
        CreateDirectoryA(p.c_str(), NULL);
        free(appdata);
        return p;
    }

    static std::string GetConfigPath()
    {
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


};