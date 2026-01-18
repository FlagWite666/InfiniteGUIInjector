using System;
using System.Diagnostics;
using System.IO;
using System.Runtime.InteropServices;
using System.Text;

namespace DLLInjector
{
    public class SafeReflectiveInjector
    {
        #region Win32 API

        [DllImport("kernel32.dll", SetLastError = true, CharSet = CharSet.Unicode)]
        private static extern IntPtr VirtualAllocEx(IntPtr hProcess, IntPtr lpAddress, uint dwSize, uint flAllocationType, uint flProtect);

        [DllImport("kernel32.dll", SetLastError = true)]
        private static extern bool WriteProcessMemory(IntPtr hProcess, IntPtr lpBaseAddress, byte[] lpBuffer, uint nSize, out UIntPtr lpNumberOfBytesWritten);

        [DllImport("kernel32.dll", SetLastError = true)]
        private static extern IntPtr CreateRemoteThread(IntPtr hProcess, IntPtr lpThreadAttributes, uint dwStackSize, IntPtr lpStartAddress, IntPtr lpParameter, uint dwCreationFlags, out IntPtr lpThreadId);

        [DllImport("kernel32.dll", SetLastError = true)]
        private static extern IntPtr GetProcAddress(IntPtr hModule, string lpProcName);

        [DllImport("kernel32.dll", SetLastError = true, CharSet = CharSet.Unicode)]
        private static extern IntPtr GetModuleHandle(string lpModuleName);

        [DllImport("kernel32.dll", SetLastError = true)]
        private static extern bool VirtualProtectEx(IntPtr hProcess, IntPtr lpAddress, IntPtr dwSize, uint flNewProtect, out uint lpflOldProtect);

        [DllImport("kernel32.dll", SetLastError = true)]
        private static extern bool CloseHandle(IntPtr hObject);

        [DllImport("kernel32.dll", SetLastError = true)]
        private static extern uint WaitForSingleObject(IntPtr hHandle, uint dwMilliseconds);

        [DllImport("kernel32.dll", SetLastError = true)]
        private static extern bool GetExitCodeThread(IntPtr hThread, out uint lpExitCode);

        [DllImport("kernel32.dll", SetLastError = true)]
        private static extern bool IsWow64Process(IntPtr hProcess, out bool wow64Process);

        [DllImport("kernel32.dll", SetLastError = true, CharSet = CharSet.Unicode)]
        private static extern IntPtr LoadLibraryW(string lpFileName);

        [DllImport("kernel32.dll", SetLastError = true)]
        private static extern bool FreeLibrary(IntPtr hModule);

        #endregion

        #region Constants

        private const uint MEM_COMMIT = 0x1000;
        private const uint MEM_RESERVE = 0x2000;
        private const uint PAGE_EXECUTE_READWRITE = 0x40;
        private const uint PAGE_READWRITE = 0x04;
        private const uint WAIT_OBJECT_0 = 0x00000000;
        private const uint INFINITE = 0xFFFFFFFF;

        #endregion

        public static bool InjectDLL(int processId, string dllPath, out string errorMessage)
        {
            errorMessage = string.Empty;

            try
            {
                Process process = Process.GetProcessById(processId);
                IntPtr hProcess = process.Handle;

                if (!File.Exists(dllPath))
                {
                    errorMessage = "DLL文件不存在";
                    return false;
                }

                bool isTarget64Bit = Is64BitProcess(process);
                bool isInjector64Bit = Environment.Is64BitProcess;

                if (isTarget64Bit != isInjector64Bit)
                {
                    errorMessage = $"架构不匹配: 目标进程是{(isTarget64Bit ? "64位" : "32位")}，但注入器是{(isInjector64Bit ? "64位" : "32位")}。请使用匹配的注入器版本。";
                    return false;
                }

                if (!ValidateDLL(dllPath, out string validationError))
                {
                    errorMessage = $"DLL验证失败: {validationError}";
                    return false;
                }

                string dllFullPath = Path.GetFullPath(dllPath);
                byte[] dllPathBytes = Encoding.Unicode.GetBytes(dllFullPath);

                IntPtr pathBuffer = VirtualAllocEx(hProcess, IntPtr.Zero, (uint)dllPathBytes.Length + 2, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

                if (pathBuffer == IntPtr.Zero)
                {
                    int error = Marshal.GetLastWin32Error();
                    errorMessage = $"内存分配失败 (错误代码: {error})";
                    return false;
                }

                if (!WriteProcessMemory(hProcess, pathBuffer, dllPathBytes, (uint)dllPathBytes.Length, out UIntPtr bytesWritten))
                {
                    int error = Marshal.GetLastWin32Error();
                    errorMessage = $"写入内存失败 (错误代码: {error})";
                    VirtualFreeEx(hProcess, pathBuffer, 0, 0x8000);
                    return false;
                }

                IntPtr loadLibraryW = GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryW");
                if (loadLibraryW == IntPtr.Zero)
                {
                    errorMessage = "获取LoadLibraryW地址失败";
                    VirtualFreeEx(hProcess, pathBuffer, 0, 0x8000);
                    return false;
                }

                IntPtr hThread = CreateRemoteThread(hProcess, IntPtr.Zero, 0, loadLibraryW, pathBuffer, 0, out IntPtr threadId);
                if (hThread == IntPtr.Zero)
                {
                    int error = Marshal.GetLastWin32Error();
                    errorMessage = $"创建远程线程失败 (错误代码: {error})。可能原因：1. 需要管理员权限 2. 目标进程受保护 3. 架构不匹配";
                    VirtualFreeEx(hProcess, pathBuffer, 0, 0x8000);
                    return false;
                }

                WaitForSingleObject(hThread, INFINITE);
                GetExitCodeThread(hThread, out uint exitCode);

                CloseHandle(hThread);
                VirtualFreeEx(hProcess, pathBuffer, 0, 0x8000);

                if (exitCode == 0)
                {
                    errorMessage = $"DLL加载失败 (退出代码: {exitCode})。可能原因：1. DLL依赖项缺失 2. DLL不兼容 3. DLL已损坏 4. DLL入口点异常";
                    return false;
                }

                return true;
            }
            catch (Exception ex)
            {
                errorMessage = $"注入失败: {ex.Message}\n堆栈跟踪: {ex.StackTrace}";
                return false;
            }
        }

        private static bool ValidateDLL(string dllPath, out string errorMessage)
        {
            errorMessage = string.Empty;

            try
            {
                byte[] dllBytes = File.ReadAllBytes(dllPath);

                if (dllBytes.Length < 64)
                {
                    errorMessage = "DLL文件太小，可能已损坏";
                    return false;
                }

                ushort dosSignature = BitConverter.ToUInt16(dllBytes, 0);
                if (dosSignature != 0x5A4D)
                {
                    errorMessage = "无效的DOS签名";
                    return false;
                }

                int peOffset = BitConverter.ToInt32(dllBytes, 60);
                if (peOffset >= dllBytes.Length)
                {
                    errorMessage = "PE偏移超出文件范围";
                    return false;
                }

                uint peSignature = BitConverter.ToUInt32(dllBytes, peOffset);
                if (peSignature != 0x00004550)
                {
                    errorMessage = "无效的PE签名";
                    return false;
                }

                ushort machine = BitConverter.ToUInt16(dllBytes, peOffset + 4);
                bool is64BitDLL = (machine == 0x8664);
                bool is64BitSystem = Environment.Is64BitOperatingSystem;

                if (is64BitDLL != is64BitSystem)
                {
                    errorMessage = $"DLL架构不匹配: DLL是{(is64BitDLL ? "64位" : "32位")}，但系统是{(is64BitSystem ? "64位" : "32位")}";
                    return false;
                }

                return true;
            }
            catch (Exception ex)
            {
                errorMessage = $"DLL验证异常: {ex.Message}";
                return false;
            }
        }

        private static bool Is64BitProcess(Process process)
        {
            if (!Environment.Is64BitOperatingSystem)
                return false;

            bool isWow64;
            bool result = IsWow64Process(process.Handle, out isWow64);
            return !isWow64;
        }

        [DllImport("kernel32.dll", SetLastError = true)]
        private static extern bool VirtualFreeEx(IntPtr hProcess, IntPtr lpAddress, int dwSize, uint dwFreeType);
    }
}