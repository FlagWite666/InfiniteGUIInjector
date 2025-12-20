#include "FpsItem.h"

#include "Anim.h"
#include "ImGuiStd.h"


void FpsItem::Toggle()
{
}

void FpsItem::Update()
{
	FPS = float(frameCount) / (float(updateIntervalMs) / 1000.0f);
	frameCount = 0;
	if (showGuiFPS)
	{
		guiFPS = float(guiFrameCount) / (float(updateIntervalMs) / 1000.0f);
		guiFrameCount = 0;
	}
	dirtyState.contentDirty = true;
}
void FpsItem::DrawContent()
{
	guiFrameCount++; //gui帧率
	int FPS = int(this->FPS);
	std::string fpsText;
	if (showGuiFPS)
	{
		int guiFps = int(this->guiFPS);
		fpsText = prefix + std::to_string(FPS) + " | " + std::to_string(guiFps) + suffix;
	}
	else
		fpsText = prefix + std::to_string(FPS) + suffix;

	ImVec4 targetTextColor = ImGui::GetStyleColorVec4(ImGuiCol_Text);
	//获取io
	ImGuiIO& io = ImGui::GetIO();
	//计算速度
	float speed = 3.0f * std::clamp(io.DeltaTime, 0.0f, 0.05f);
	color.color = ImLerp(color.color, targetTextColor, speed);
	// 判断动画是否结束
	if (Anim::AlmostEqual(color.color, targetTextColor))
	{
		color.color = targetTextColor;
		dirtyState.animating = false;
	}
	ImGuiStd::TextColoredShadow(color.color, fpsText.c_str());
}

void FpsItem::RenderBeforeGui()
{
	frameCount++; //游戏帧率
	auto now = Clock::now();
	lastFrameTimeMs = std::chrono::duration_cast<std::chrono::duration<float>>(now - lastFrameTime).count();
	lastFrameTime = now;
}

void FpsItem::DrawSettings(const float& bigPadding, const float& centerX, const float& itemWidth)
{
	//DrawItemSettings();
	ImGui::SetCursorPosX(bigPadding);
	ImGui::SetNextItemWidth(itemWidth);
	ImGui::Checkbox(u8"显示无限Gui的FPS", &showGuiFPS);
	ImGui::SameLine(); ImGuiStd::HelpMarker(
		u8"无限GUI会主动降低自身的渲染计算频率以优化性能。\nGUI是否刷新取决于窗口内容是否发生变化以及是否处于动画状态。\n    在界面静止时，无限GUI 会复用已有渲染结果；\n    在动画过程中，刷新频率最高限制为显示器的刷新率，以保证视觉流畅并避免无意义的高频计算。\n这种按需刷新机制在性能与体验之间取得了最佳平衡。");
	DrawAffixSettings(bigPadding, centerX, itemWidth);
	DrawWindowSettings(bigPadding, centerX, itemWidth);
}

void FpsItem::Load(const nlohmann::json& j)
{
	LoadItem(j);
	LoadAffix(j);
	LoadWindow(j);
	if (j.contains("showGuiFPS")) showGuiFPS = j.at("showGuiFPS").get<bool>();

}
void FpsItem::Save(nlohmann::json& j) const
{
	SaveItem(j);
	SaveAffix(j);
	SaveWindow(j);
	j["showGuiFPS"] = showGuiFPS;
}