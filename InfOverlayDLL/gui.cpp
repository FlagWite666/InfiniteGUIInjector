#include "gui.h"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_win32.h"
#include "opengl_hook.h"

//#include <gl/glew.h>
//#include <gl/GL.h>

#include "fonts.h"
#include "ImGuiSty.h"
#include "ItemManager.h"
#include "App.h"

#include "Menu.h"
static ImGuiContext* imGuiContext = nullptr;

void Gui::init()
{

	/*GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glViewport(0, 0, viewport[2], viewport[3]);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, viewport[2], viewport[3], 0, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDisable(GL_DEPTH_TEST);*/




	Fonts::init();
	imGuiContext = ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // 可选
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // 可选

	// 默认关闭 ImGui 捕获鼠标（只有激活 UI 时允许）
	io.MouseDrawCursor = false;
	io.ConfigFlags |= ImGuiConfigFlags_NoMouse; // 禁止 ImGui 捕获鼠标输入（我们在切换时会调整）
	io.IniFilename = nullptr; // 禁止生成 imgui.ini
	//ImGui::StyleColorsDark();
	SetStyleGray(GlobalConfig::Instance().roundCornerRadius);
	//SetStylePurple();
	//SetStyleViolet();
	//SetStyleVioletBlue();

	ImGui_ImplWin32_Init(opengl_hook::handle_window);
	ImGui_ImplOpenGL3_Init();
	//ImFontConfig config{};
	//config.FontDataOwnedByAtlas = false;
	//font = io.Fonts->AddFontFromMemoryTTF(Fonts::harmony.data, Fonts::harmony.size, 17.f, &config, io.Fonts->GetGlyphRangesChineseFull());

	ImFontConfig font_cfg;
	font_cfg.FontDataOwnedByAtlas = false;
	font_cfg.OversampleH = 1;
	font_cfg.OversampleV = 1;
	font_cfg.PixelSnapH = true;

	ImFont* font;
	if (GlobalConfig::Instance().fontPath == "default")
		font = io.Fonts->AddFontFromMemoryCompressedTTF(Fonts::ur.data, Fonts::ur.size, 20.0f, &font_cfg, io.Fonts->GetGlyphRangesChineseFull());
	else
		font = io.Fonts->AddFontFromFileTTF(GlobalConfig::Instance().fontPath.c_str(), 20.0f, &font_cfg, io.Fonts->GetGlyphRangesChineseFull());
	if (font == nullptr) {
		font = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\msyh.ttc", 20.0f, &font_cfg, io.Fonts->GetGlyphRangesChineseFull());
	}
	App::Instance().iconFont = io.Fonts->AddFontFromMemoryTTF(Fonts::icons.data, Fonts::icons.size, 20.0f, &font_cfg);


	io.FontDefault = font;

	if (const GLenum err = glewInit(); GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}

	isInit = true;

}
void Gui::clean()
{
	if (imGuiContext)ImGui::GetIO().Fonts->Clear();
	if ((ImGui::GetCurrentContext() ? (void*)ImGui::GetIO().BackendRendererUserData : nullptr))ImGui_ImplOpenGL3_Shutdown();
	if ((ImGui::GetCurrentContext() ? (void*)ImGui::GetIO().BackendPlatformUserData : nullptr))ImGui_ImplWin32_Shutdown();
	if (imGuiContext)ImGui::DestroyContext(imGuiContext);
}
//#include <base/voyage.h>
//#include <base/features/events/events.h>
static std::atomic_flag clipCursor = ATOMIC_FLAG_INIT;
static RECT originalClip;
void Gui::render()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	//static std::once_flag flag{};
	//std::call_once(flag, [&]
	//	{
	//		MyGuiBloom = std::make_shared<BloomImGui::FxBloomSystem>(opengl_hook::screen_size.x, opengl_hook::screen_size.y);
	//		MyGuiBloom->GetImGuiNewFrameFUNC(ImGui_ImplWin32_NewFrame);
	//		MyGuiBloom->SettingBloomRadius(30.f);
	//	});

	//MyGuiBloom->RenderContextCaptureBegin();

	//ImGui::SetNextWindowPos(ImVec2(0, 0));
	//ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
	ImGuiContext* context = ImGui::GetCurrentContext();
	if (context->WithinFrameScope)
	{
		//ImGui::Begin("Overlay", nullptr,
		//	ImGuiWindowFlags_NoTitleBar |
		//	ImGuiWindowFlags_NoResize |
		//	ImGuiWindowFlags_NoMove |
		//	ImGuiWindowFlags_NoScrollbar |
		//	ImGuiWindowFlags_NoSavedSettings |
		//	ImGuiWindowFlags_NoInputs |
		//	ImGuiWindowFlags_NoFocusOnAppearing |
		//	ImGuiWindowFlags_NoBringToFrontOnFocus |
		//	ImGuiWindowFlags_NoBackground);
		////voyage::get().event_bus.fire_event(render_2d_event{});
		//ImGui::End();
		ItemManager::Instance().RenderAll();
		Menu::Instance().Render(&done);
	}




	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
