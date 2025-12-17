#pragma once
#include "imgui/imgui.h"
#include <GL/glew.h>
#include <GL/GL.h>
struct Texture
{
	GLuint id;
	int width;
	int height;
};

class Gui
{
public:
	bool isInit = false;
	bool done = false;
	void init();
	void clean();
	void render() const;
	ImFont* font;
	ImFont* iconFont;
	Texture logoTexture;
};

