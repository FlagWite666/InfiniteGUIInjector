#include "GlobalWindowStyle.h"

void GlobalWindowStyle::Toggle()
{
}

void GlobalWindowStyle::Load(const nlohmann::json& j)
{
	LoadItem(j);
	LoadStyle(j);
}

void GlobalWindowStyle::Save(nlohmann::json& j) const
{
	SaveItem(j);
	SaveStyle(j);
}

void GlobalWindowStyle::DrawSettings()
{
	DrawStyleSettings();
}

ItemStyle& GlobalWindowStyle::GetGlobeStyle()
{
	return itemStyle;
}