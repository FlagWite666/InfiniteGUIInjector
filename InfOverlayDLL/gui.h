#pragma once
class Gui
{
public:
	bool isInit = false;
	bool done = false;
	void init();
	void clean();
	void render();
};

