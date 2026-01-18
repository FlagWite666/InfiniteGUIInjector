using System;
using System.Diagnostics;
using System.Collections.Generic;
using System.Linq;

namespace DLLInjector
{
    public class ProcessInfo
    {
        public int ProcessId { get; set; }
        public string ProcessName { get; set; }
        public string MainWindowTitle { get; set; }
        public string DisplayName { get; set; }

        public override string ToString()
        {
            return DisplayName;
        }
    }

    public class ProcessHelper
    {
        public static List<ProcessInfo> GetJavaProcesses()
        {
            var javaProcesses = new List<ProcessInfo>();

            try
            {
                var javaExeProcesses = Process.GetProcessesByName("java");
                var javawExeProcesses = Process.GetProcessesByName("javaw");

                var allJavaProcesses = javaExeProcesses.Concat(javawExeProcesses);

                foreach (var process in allJavaProcesses)
                {
                    try
                    {
                        var processInfo = new ProcessInfo
                        {
                            ProcessId = process.Id,
                            ProcessName = process.ProcessName,
                            MainWindowTitle = process.MainWindowTitle
                        };

                        processInfo.DisplayName = string.IsNullOrEmpty(process.MainWindowTitle)
                            ? $"{process.ProcessName}.exe (PID: {process.Id})"
                            : $"{process.MainWindowTitle} (PID: {process.Id})";

                        javaProcesses.Add(processInfo);
                    }
                    catch (Exception)
                    {
                        continue;
                    }
                }

                return javaProcesses.OrderBy(p => p.ProcessId).ToList();
            }
            catch (Exception)
            {
                return new List<ProcessInfo>();
            }
        }

        public static List<ProcessInfo> GetAllProcesses()
        {
            var allProcesses = new List<ProcessInfo>();

            try
            {
                var processes = Process.GetProcesses();

                foreach (var process in processes)
                {
                    try
                    {
                        var processInfo = new ProcessInfo
                        {
                            ProcessId = process.Id,
                            ProcessName = process.ProcessName,
                            MainWindowTitle = process.MainWindowTitle
                        };

                        processInfo.DisplayName = string.IsNullOrEmpty(process.MainWindowTitle)
                            ? $"{process.ProcessName}.exe (PID: {process.Id})"
                            : $"{process.MainWindowTitle} (PID: {process.Id})";

                        allProcesses.Add(processInfo);
                    }
                    catch (Exception)
                    {
                        continue;
                    }
                }

                return allProcesses.OrderBy(p => p.ProcessName).ThenBy(p => p.ProcessId).ToList();
            }
            catch (Exception)
            {
                return new List<ProcessInfo>();
            }
        }

        public static bool IsProcessRunning(int processId)
        {
            try
            {
                Process.GetProcessById(processId);
                return true;
            }
            catch (ArgumentException)
            {
                return false;
            }
        }
    }
}
