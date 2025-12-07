#include "imgui\imgui.h"
#include "ImGuiStd.h"
#include "ChangeLog.h"

void ChangeLog::Init()
{
    NewVersion("B0.9.0", "25.11.30");
    AddContent(INFO, u8"发布第一支测试版本");

    NewVersion("B0.9.1", "25.12.05");
    AddContent(OPTIMIZE, u8"优化资源文件夹位置");
    AddContent(ADD, u8"添加窗口样式自定义");
    AddContent(ADD, u8"添加窗口彩虹字");
    AddContent(ADD, u8"添加动态模糊");

    NewVersion("B0.9.2", "25.12.12");
    AddContent(OPTIMIZE, u8"优化ChangeLog界面");
    AddContent(OPTIMIZE, u8"优化/美化动态模糊");
    AddContent(FIXBUG, u8"修复字体彩色绑定问题");
}

ChangeLog::ChangeLog()
{
	Init();
}

void ChangeLog::Draw() const
{
    for (auto& log : logs)
    {
        DrawSingleLog(log);
    }
}

void ChangeLog::AddContent(const ContentType& type, const std::string& content)
{
    Content newContent;
    newContent.type = type;
    newContent.content = content;
    logs.front().contents.push_back(newContent);
}

void ChangeLog::NewVersion(const std::string& version, const std::string& date)
{
	Log newLog;
	newLog.info.version = version;
	newLog.info.date = date;
	logs.push_front(newLog);
}

void ChangeLog::DrawSingleLog(const Log& log)
{
    DrawVersion(log.info);
    for (auto& content : log.contents)
    {
        DrawContent(content);
    }
    ImGui::Separator();
}

void ChangeLog::DrawVersion(const Info& info)
{
    std::string text = "   -" + info.date + "-    |    -" + info.version + "-";
    ImGuiStd::TextShadow(text.c_str());
}

void ChangeLog::DrawContent(const Content& content)
{
    std::string prefix;
    std::string contentStr = content.content;
    std::string finalStr;
    switch (content.type)
    {
    case INFO:
        prefix = u8"[信息]";
        break;
    case OPTIMIZE:
        prefix = u8"[优化]";
        break;
    case ADD:
        prefix = u8"[新增]";
        break;
    case FIXBUG:
        prefix = u8"[修复]";
        break;
    case REMOVE:
        prefix = u8"[移除]";
        break;
    }
    finalStr = prefix + " " + contentStr;
    ImGui::BulletText(finalStr.c_str());
}
