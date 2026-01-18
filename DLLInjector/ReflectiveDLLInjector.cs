using System;
using System.Diagnostics;
using System.IO;
using System.Runtime.InteropServices;
using System.Text;

namespace DLLInjector
{
    public class ReflectiveDLLInjector
    {
        #region Win32 API

        [DllImport("kernel32.dll", SetLastError = true)]
        private static extern IntPtr VirtualAllocEx(IntPtr hProcess, IntPtr lpAddress, uint dwSize, uint flAllocationType, uint flProtect);

        [DllImport("kernel32.dll", SetLastError = true)]
        private static extern bool WriteProcessMemory(IntPtr hProcess, IntPtr lpBaseAddress, byte[] lpBuffer, uint nSize, out UIntPtr lpNumberOfBytesWritten);

        [DllImport("kernel32.dll", SetLastError = true)]
        private static extern IntPtr CreateRemoteThread(IntPtr hProcess, IntPtr lpThreadAttributes, uint dwStackSize, IntPtr lpStartAddress, IntPtr lpParameter, uint dwCreationFlags, out IntPtr lpThreadId);

        [DllImport("kernel32.dll", SetLastError = true)]
        private static extern IntPtr GetProcAddress(IntPtr hModule, string lpProcName);

        [DllImport("kernel32.dll", SetLastError = true)]
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

        [DllImport("kernel32.dll", SetLastError = true)]
        private static extern IntPtr VirtualAlloc(IntPtr lpAddress, uint dwSize, uint flAllocationType, uint flProtect);

        [DllImport("kernel32.dll", SetLastError = true)]
        private static extern bool VirtualFree(IntPtr lpAddress, int dwSize, uint dwFreeType);

        [DllImport("kernel32.dll", SetLastError = true)]
        private static extern bool VirtualProtect(IntPtr lpAddress, IntPtr dwSize, uint flNewProtect, out uint lpflOldProtect);

        [DllImport("kernel32.dll", SetLastError = true, CharSet = CharSet.Ansi)]
        private static extern IntPtr LoadLibraryA(string lpFileName);

        [DllImport("kernel32.dll", SetLastError = true)]
        private static extern IntPtr GetModuleHandleA(string lpModuleName);

        #endregion

        #region Constants

        private const uint MEM_COMMIT = 0x1000;
        private const uint MEM_RESERVE = 0x2000;
        private const uint PAGE_EXECUTE_READWRITE = 0x40;
        private const uint PAGE_READWRITE = 0x04;
        private const uint PAGE_READONLY = 0x02;
        private const uint WAIT_OBJECT_0 = 0x00000000;
        private const uint INFINITE = 0xFFFFFFFF;
        private const uint IMAGE_DOS_SIGNATURE = 0x5A4D;
        private const uint IMAGE_NT_SIGNATURE = 0x00004550;
        private const uint IMAGE_DIRECTORY_ENTRY_IMPORT = 1;
        private const uint IMAGE_DIRECTORY_ENTRY_BASERELOC = 5;
        private const uint IMAGE_REL_BASED_DIR64 = 10;
        private const uint IMAGE_REL_BASED_HIGHLOW = 3;
        private const uint IMAGE_SCN_MEM_EXECUTE = 0x20000000;
        private const uint IMAGE_SCN_MEM_READ = 0x40000000;
        private const uint IMAGE_SCN_MEM_WRITE = 0x80000000;

        #endregion

        #region PE Structures

        [StructLayout(LayoutKind.Sequential)]
        private struct IMAGE_DOS_HEADER
        {
            public ushort e_magic;
            public ushort e_cblp;
            public ushort e_cp;
            public ushort e_crlc;
            public ushort e_cparhdr;
            public ushort e_minalloc;
            public ushort e_maxalloc;
            public ushort e_ss;
            public ushort e_sp;
            public ushort e_csum;
            public ushort e_ip;
            public ushort e_cs;
            public ushort e_lfarlc;
            public ushort e_ovno;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 4)]
            public ushort[] e_res;
            public ushort e_oemid;
            public ushort e_oeminfo;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 10)]
            public ushort[] e_res2;
            public int e_lfanew;
        }

        [StructLayout(LayoutKind.Sequential)]
        private struct IMAGE_FILE_HEADER
        {
            public ushort Machine;
            public ushort NumberOfSections;
            public uint TimeDateStamp;
            public uint PointerToSymbolTable;
            public uint NumberOfSymbols;
            public ushort SizeOfOptionalHeader;
            public ushort Characteristics;
        }

        [StructLayout(LayoutKind.Sequential)]
        private struct IMAGE_DATA_DIRECTORY
        {
            public uint VirtualAddress;
            public uint Size;
        }

        [StructLayout(LayoutKind.Sequential)]
        private struct IMAGE_OPTIONAL_HEADER64
        {
            public ushort Magic;
            public byte MajorLinkerVersion;
            public byte MinorLinkerVersion;
            public uint SizeOfCode;
            public uint SizeOfInitializedData;
            public uint SizeOfUninitializedData;
            public uint AddressOfEntryPoint;
            public uint BaseOfCode;
            public ulong ImageBase;
            public uint SectionAlignment;
            public uint FileAlignment;
            public ushort MajorOperatingSystemVersion;
            public ushort MinorOperatingSystemVersion;
            public ushort MajorImageVersion;
            public ushort MinorImageVersion;
            public ushort MajorSubsystemVersion;
            public ushort MinorSubsystemVersion;
            public uint Win32VersionValue;
            public uint SizeOfImage;
            public uint SizeOfHeaders;
            public uint CheckSum;
            public ushort Subsystem;
            public ushort DllCharacteristics;
            public ulong SizeOfStackReserve;
            public ulong SizeOfStackCommit;
            public ulong SizeOfHeapReserve;
            public ulong SizeOfHeapCommit;
            public uint LoaderFlags;
            public uint NumberOfRvaAndSizes;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 16)]
            public IMAGE_DATA_DIRECTORY[] DataDirectory;
        }

        [StructLayout(LayoutKind.Sequential)]
        private struct IMAGE_SECTION_HEADER
        {
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 8)]
            public byte[] Name;
            public uint VirtualSize;
            public uint VirtualAddress;
            public uint SizeOfRawData;
            public uint PointerToRawData;
            public uint PointerToRelocations;
            public uint PointerToLinenumbers;
            public ushort NumberOfRelocations;
            public ushort NumberOfLinenumbers;
            public uint Characteristics;
        }

        [StructLayout(LayoutKind.Sequential)]
        private struct IMAGE_BASE_RELOCATION
        {
            public uint VirtualAddress;
            public uint SizeOfBlock;
        }

        [StructLayout(LayoutKind.Sequential)]
        private struct IMAGE_IMPORT_DESCRIPTOR
        {
            public uint OriginalFirstThunk;
            public uint TimeDateStamp;
            public uint ForwarderChain;
            public uint Name;
            public uint FirstThunk;
        }

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

                byte[] dllBytes = File.ReadAllBytes(dllPath);

                if (!IsValidPE(dllBytes))
                {
                    errorMessage = "无效的PE文件";
                    return false;
                }

                IntPtr dllBuffer = VirtualAllocEx(hProcess, IntPtr.Zero, (uint)dllBytes.Length, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
                if (dllBuffer == IntPtr.Zero)
                {
                    errorMessage = $"内存分配失败 (错误代码: {Marshal.GetLastWin32Error()})";
                    return false;
                }

                if (!WriteProcessMemory(hProcess, dllBuffer, dllBytes, (uint)dllBytes.Length, out UIntPtr bytesWritten))
                {
                    errorMessage = $"写入内存失败 (错误代码: {Marshal.GetLastWin32Error()})";
                    VirtualFreeEx(hProcess, dllBuffer, 0, 0x8000);
                    return false;
                }

                byte[] loaderCode = GenerateReflectiveLoader();
                IntPtr loaderBuffer = VirtualAllocEx(hProcess, IntPtr.Zero, (uint)loaderCode.Length, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
                if (loaderBuffer == IntPtr.Zero)
                {
                    errorMessage = $"加载器内存分配失败 (错误代码: {Marshal.GetLastWin32Error()})";
                    VirtualFreeEx(hProcess, dllBuffer, 0, 0x8000);
                    return false;
                }

                if (!WriteProcessMemory(hProcess, loaderBuffer, loaderCode, (uint)loaderCode.Length, out bytesWritten))
                {
                    errorMessage = $"加载器写入内存失败 (错误代码: {Marshal.GetLastWin32Error()})";
                    VirtualFreeEx(hProcess, dllBuffer, 0, 0x8000);
                    VirtualFreeEx(hProcess, loaderBuffer, 0, 0x8000);
                    return false;
                }

                IntPtr hThread = CreateRemoteThread(hProcess, IntPtr.Zero, 0, loaderBuffer, dllBuffer, 0, out IntPtr threadId);
                if (hThread == IntPtr.Zero)
                {
                    errorMessage = $"创建远程线程失败 (错误代码: {Marshal.GetLastWin32Error()})";
                    VirtualFreeEx(hProcess, dllBuffer, 0, 0x8000);
                    VirtualFreeEx(hProcess, loaderBuffer, 0, 0x8000);
                    return false;
                }

                WaitForSingleObject(hThread, INFINITE);
                GetExitCodeThread(hThread, out uint exitCode);

                CloseHandle(hThread);
                VirtualFreeEx(hProcess, dllBuffer, 0, 0x8000);
                VirtualFreeEx(hProcess, loaderBuffer, 0, 0x8000);

                if (exitCode == 0)
                {
                    errorMessage = $"DLL加载失败 (退出代码: {exitCode})";
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

        private static bool IsValidPE(byte[] peBytes)
        {
            if (peBytes.Length < 64)
                return false;

            IMAGE_DOS_HEADER dosHeader = ByteArrayToStructure<IMAGE_DOS_HEADER>(peBytes, 0);
            if (dosHeader.e_magic != IMAGE_DOS_SIGNATURE)
                return false;

            if (dosHeader.e_lfanew >= peBytes.Length)
                return false;

            uint ntSignature = BitConverter.ToUInt32(peBytes, (int)dosHeader.e_lfanew);
            if (ntSignature != IMAGE_NT_SIGNATURE)
                return false;

            return true;
        }

        private static T ByteArrayToStructure<T>(byte[] bytes, int offset)
        {
            int size = Marshal.SizeOf(typeof(T));
            IntPtr ptr = Marshal.AllocHGlobal(size);
            try
            {
                Marshal.Copy(bytes, offset, ptr, size);
                return (T)Marshal.PtrToStructure(ptr, typeof(T));
            }
            finally
            {
                Marshal.FreeHGlobal(ptr);
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

        private static byte[] GenerateReflectiveLoader()
        {
            return new byte[]
            {
                0x48, 0x89, 0xC8, 0x48, 0x89, 0xD1, 0x48, 0x83, 0xEC, 0x28, 0x48, 0x8B, 0xC9, 0x48, 0x83, 0xC1,
                0x3C, 0x66, 0x81, 0x39, 0x4D, 0x5A, 0x75, 0x07, 0x48, 0x31, 0xC0, 0xEB, 0x50, 0x48, 0x8B,
                0x49, 0x3C, 0x48, 0x83, 0xC1, 0x18, 0x66, 0x81, 0x79, 0x18, 0x50, 0x45, 0x75, 0x07, 0x48,
                0x31, 0xC0, 0xEB, 0x3D, 0x48, 0x31, 0xC0, 0xC3, 0x48, 0x8B, 0x49, 0x3C, 0x48, 0x83, 0xC1, 0x10,
                0x8B, 0x41, 0x50, 0x48, 0x83, 0xC1, 0x28, 0x8B, 0x41, 0x28, 0x48, 0x01, 0xD0, 0x48, 0x89, 0xC2,
                0x48, 0x89, 0xC8, 0x48, 0x31, 0xC0, 0xC3
            };
        }
    }
}