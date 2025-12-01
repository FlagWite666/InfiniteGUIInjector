#include "opengl_hook.h"
#include "FileUtils.h"
#include "ConfigManager.h"
#include "AudioManager.h"
#include <thread>
#include <atomic>
HMODULE g_hModule = NULL;

static std::atomic_bool g_running = ATOMIC_VAR_INIT(true);
// 线程函数：更新所有 item 状态
void UpdateThread() {
	while (g_running.load()) {
		ItemManager::Instance().UpdateAll();  // 调用UpdateAll()来更新所有item
		std::this_thread::sleep_for(std::chrono::milliseconds(1));  // 休眠100ms，可以根据实际需求调整
	}
}

// 启动更新线程
void StartUpdateThread() {
	std::thread updateThread(UpdateThread);
	updateThread.detach();  // 将线程设为后台线程
}

//void Uninit() {
//	g_running = false;
//	AudioManager::Instance().Shutdown();
//	ConfigManager::Save(FileUtils::configPath);
//}


DWORD WINAPI InitApp(LPVOID)
{
    FileUtils::InitPaths(g_hModule);
    opengl_hook::init();
	while (!opengl_hook::gui.isInit)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	//加载配置文件
	ConfigManager::Load(FileUtils::configPath);
	//初始化音频管理器
	AudioManager::Instance().Init();
	
	StartUpdateThread();

	//while (!opengl_hook::gui.done)
	//{
	//	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	//}
	

    return 0;

}