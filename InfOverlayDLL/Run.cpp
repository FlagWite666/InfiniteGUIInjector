#include "Run.h"
#include "ItemManager.h"
#include "ConfigManager.h"
#include "Menu.h"
#include "GlobalConfig.h"
#include "StringConverter.h"
#include "FileUtils.h"
#include "AudioManager.h"
#include "App.h"
#include "fonts\Uranus_Pixel_11Px.h"
#include "pch.h"
#include "ImguiSty.h"
#include <thread>
typedef BOOL(WINAPI* OldSwapBuffers)(HDC);
OldSwapBuffers fpSwapBuffers = NULL;
// 仅当 uiActive=true 时，让 ImGui 处理 Win32 消息
LRESULT CALLBACK HookedWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

    bool isRepeat = (lParam & (1 << 30)) != 0;
    bool state;
    if (message == WM_KEYDOWN || message == WM_LBUTTONDOWN || message == WM_RBUTTONDOWN ||
        message == WM_MBUTTONDOWN || message == WM_XBUTTONDOWN)
        state = true;
    else if (message == WM_KEYUP || message == WM_LBUTTONUP || message == WM_RBUTTONUP ||
        message == WM_MBUTTONUP || message == WM_XBUTTONUP)
        state = false;
    else
        state = true;

    ItemManager::Instance().ProcessKeyEvents(state, isRepeat, wParam);
    // 1. 按键检测
    if (state)
    {
        if (wParam == GlobalConfig::Instance().menuKey &&!isRepeat)
            Menu::Instance().Toggle();

        if (wParam == VK_ESCAPE && Menu::Instance().open)
            Menu::Instance().Toggle();

    }

    if (Menu::Instance().open)
    {
        // 只有 UI 激活时才把消息交给 ImGui 处理
        ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam);
        switch (message)
        {
        case WM_IME_STARTCOMPOSITION:
        case WM_IME_ENDCOMPOSITION:
        case WM_IME_COMPOSITION:

        case WM_IME_SETCONTEXT:
        case WM_IME_NOTIFY:
        case WM_IME_CONTROL:
        case WM_IME_COMPOSITIONFULL:
        case WM_IME_SELECT:
        case WM_IME_CHAR:
        case WM_IME_REQUEST:
        case WM_IME_KEYDOWN:
        case WM_IME_KEYUP:

        case WM_INPUTLANGCHANGEREQUEST:
        case WM_INPUTLANGCHANGE:
            goto end;
        default:
            return TRUE;
        }

    }
end:
    // 始终把消息交回原来的窗口过程（不管 UI 是否激活）
    return CallWindowProc(g_oldWndProc, hWnd, message, wParam, lParam);
}

// 线程函数：更新所有 item 状态
void UpdateThread() {
    App::Instance().GetAnnouncement();
    while (true) {
        ItemManager::Instance().UpdateAll();  // 调用UpdateAll()来更新所有item
        std::this_thread::sleep_for(std::chrono::milliseconds(1));  // 休眠100ms，可以根据实际需求调整
    }
}

// 启动更新线程
void StartUpdateThread() {
    std::thread updateThread(UpdateThread);
    updateThread.detach();  // 将线程设为后台线程
}
// 在找到有效 HDC/窗口后初始化 ImGui（只初始化一次）
void InitImGuiForContext()
{
    if (!wglGetCurrentContext())
    {
        return;
    }
    if (g_isInit) return;
    if (!App::Instance().clientHwnd) return;


    // ...

    // 构造参数第一个为 缓冲帧大小(一般与图像窗口大小相同)
    // 第二个为可选参数: std::function<void(const std::string&)> (如果需要自定义日志输出)
    // 替换窗口过程以便我们可以在 UI 激活时处理消息
    g_oldWndProc = (WNDPROC)SetWindowLongPtr(App::Instance().clientHwnd, GWLP_WNDPROC, (LONG_PTR)HookedWndProc);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // 可选
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // 可选

    // 默认关闭 ImGui 捕获鼠标（只有激活 UI 时允许）
    io.MouseDrawCursor = false;
    io.ConfigFlags |= ImGuiConfigFlags_NoMouse; // 禁止 ImGui 捕获鼠标输入（我们在切换时会调整）
    io.IniFilename = nullptr; // 禁止生成 imgui.ini
    ImGui::StyleColorsDark();
    setStyle(GlobalConfig::Instance().roundCornerRadius);

    // 初始化平台/渲染绑定。GLSL 版本根据 MC 的 GL context 版本可调整
    ImGui_ImplWin32_Init(App::Instance().clientHwnd);

    //ImGui_ImplOpenGL3_Init("#version 130"); // 如果你的 MC 是较新 GL，可改为 "#version 330 core"
     // 尝试获取当前 OpenGL 上下文信息
    const GLubyte* glVersionStr = glGetString(GL_VERSION);
    const GLubyte* glRendererStr = glGetString(GL_RENDERER);

    if (!glVersionStr)
    {
        MessageBoxA(App::Instance().clientHwnd, "OpenGL context not ready! Please call after SwapBuffers hook is active.", "Error", MB_ICONERROR);
        return;
    }

    std::string glVersion(reinterpret_cast<const char*>(glVersionStr));
    std::string glRenderer(reinterpret_cast<const char*>(glRendererStr ? glRendererStr : (const GLubyte*)"Unknown"));

    int major = 0, minor = 0;
    sscanf(glVersion.c_str(), "%d.%d", &major, &minor);

    const char* glsl_version = "#version 130"; // fallback default

    if (major >= 4) glsl_version = "#version 410 core";
    else if (major == 3 && minor >= 3) glsl_version = "#version 330 core";
    else if (major == 3 && minor >= 2) glsl_version = "#version 150 core";

    // 输出检测结果到控制台（或日志）
    std::string msg = "Detected OpenGL version: " + glVersion +
        "\nRenderer: " + glRenderer +
        "\nUsing GLSL: " + glsl_version + "\n";
    OutputDebugStringA(msg.c_str());
    ImGui_ImplOpenGL3_Init(glsl_version);

    ImFont* font;
    if (GlobalConfig::Instance().fontPath == "default")
        font = io.Fonts->AddFontFromMemoryCompressedTTF(Ur_data, Ur_size, 20.0f, nullptr, io.Fonts->GetGlyphRangesChineseFull());
    else
        font = io.Fonts->AddFontFromFileTTF(GlobalConfig::Instance().fontPath.c_str(), 20.0f, nullptr, io.Fonts->GetGlyphRangesChineseFull());
    if (font == nullptr) {
        font = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\msyh.ttc", 20.0f, nullptr, io.Fonts->GetGlyphRangesChineseFull());
    }
    io.FontDefault = font;
    g_isInit = true;

    //加载配置文件
    ConfigManager::Load(FileUtils::GetConfigPath());
    //初始化音频管理器
    AudioManager::Instance().Init();
    StartUpdateThread();  // 启动更新线程
}
// Hooked SwapBuffers - 每次换帧都会被调用
BOOL WINAPI MySwapBuffers(HDC hdc)
{
    // 获取与当前 HDC 关联的顶层窗口（只在首次时用于初始化）
    if (!App::Instance().clientHwnd)
    {
        HWND possible = WindowFromDC(hdc);
        if (possible) App::Instance().clientHwnd = possible;
    }

    // 在首次有有效窗口时初始化 ImGui（安全）
    if (!g_isInit && App::Instance().clientHwnd)
    {
        InitImGuiForContext();
        //MyGuiBloom = new BloomImGui::FxBloomSystem(1000, 618);//这个为窗口大小
    }

    // 如果 ImGui 未初始化，直接调用原 SwapBuffers
    if (!g_isInit)
        return fpSwapBuffers(hdc);

    // 切换 ImGui 鼠标捕获设置
    ImGuiIO& io = ImGui::GetIO();
    if (Menu::Instance().open)
    {
        io.ConfigFlags &= ~ImGuiConfigFlags_NoMouse; // 允许 ImGui 处理鼠标
    }
    else
    {
        io.ConfigFlags |= ImGuiConfigFlags_NoMouse;  // 禁止 ImGui 捕获鼠标
    }
    // --- 新帧 ---
    ImGui_ImplOpenGL3_NewFrame(); // 如果不可用，可省略（一些版本没有此函数，但通常有）
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    {
        ItemManager::Instance().RenderAll(App::Instance().clientHwnd);
        Menu::Instance().Render();
    }

    // 渲染 ImGui
    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // 如果多视口 (通常注入到 MC 时不使用多视口) -- 跳过

    // 调用原始 SwapBuffers
    return fpSwapBuffers(hdc);
}

// 为安全：避免重复初始化/卸载
static bool g_hookInitialized = false;
static bool g_mhInitialized = false;
// 初始化 Hook（在单独线程里执行）
DWORD WINAPI InitThread(LPVOID)
{

    if (g_mhInitialized) return 0;

    if (MH_Initialize() != MH_OK)
    {
        return 0;
    }
    g_mhInitialized = true;

    // 创建 Hook（对 wglSwapBuffers）
    // 注意：直接使用 &SwapBuffers 也可（GDI SwapBuffers），这里 hook opengl 的 SwapBuffers（用户提供的代码用 SwapBuffers）
    if (MH_CreateHookApi(L"opengl32", "wglSwapBuffers", &MySwapBuffers, reinterpret_cast<void**>(&fpSwapBuffers)) != MH_OK)
    {
        // fallback: try hooking SwapBuffers from GDI32 (less likely)
        if (MH_CreateHook(&::SwapBuffers, &MySwapBuffers, reinterpret_cast<void**>(&fpSwapBuffers)) != MH_OK)
        {
            // failed to create hook
            MH_Uninitialize();
            g_mhInitialized = false;
            return 0;
        }
    }

    // 启用所有 hook
    if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK)
    {
        MH_DisableHook(MH_ALL_HOOKS);
        MH_Uninitialize();
        g_mhInitialized = false;
        return 0;
    }

    g_hookInitialized = true;
    return 0;
}

void Uninit()
{
    // 卸载 ImGui
    if (g_isInit)
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
        g_isInit = false;
    }

    // 恢复窗口过程
    if (App::Instance().clientHwnd && g_oldWndProc)
    {
        SetWindowLongPtr(App::Instance().clientHwnd, GWLP_WNDPROC, (LONG_PTR)g_oldWndProc);
        g_oldWndProc = NULL;
    }

    AudioManager::Instance().Shutdown();// 卸载音频管理器

    // 卸载 MinHook
    if (g_mhInitialized)
    {
        MH_DisableHook(MH_ALL_HOOKS);
        MH_RemoveHook(MH_ALL_HOOKS);
        MH_Uninitialize();
        g_mhInitialized = false;
        g_hookInitialized = false;
    }
}
