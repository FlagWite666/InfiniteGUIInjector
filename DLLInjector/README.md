<img width="285" height="53" alt="image" src="https://github.com/user-attachments/assets/afcd3605-f0e6-465d-bb1e-2fd553063704" /># InfiniteGUI 注入器

一个完全由AI编写的 DLL 注入器，专门为 Minecraft 设计。

![License](https://img.shields.io/badge/License-MIT-blue.svg)
![Platform](https://img.shields.io/badge/platform-Windows-blue.svg)
![.NET](https://img.shields.io/badge/.NET-8.0-blue.svg)

## 功能特性

- ✅ 自动检测并列出所有 Java 进程（java.exe 和 javaw.exe）
- ✅ 内置 InfiniteGUI-DLL，无需手动选择 DLL 文件
- ✅ 内置 glew32.dll，自动检查和安装到 System32
- ✅ 自动提取 DLL 到临时目录并注入
- ✅ 反射式注入技术（使用 LoadLibraryW）
- ✅ 实时刷新进程列表
- ✅ 现代化深色主题界面
- ✅ 自定义深色边框和标题栏
- ✅ 可拖动窗口
- ✅ 状态提示和错误处理
- ✅ 单个 EXE 文件，便于传输
- ✅ 智能安装 glew32.dll（仅在需要时）

## 系统要求

- **.NET**：.NET 8.0 Runtime（已包含）
- **权限**：普通用户权限即可启动（需要管理员权限时会自动提示）

## 使用方法

### 1. 运行注入器

双击 `InfiniteGUIInjector.exe` 或运行 `run.bat`

**注意**：
- 程序启动时会自动检查 `C:\Windows\System32\glew32.dll`
- 如果文件不存在，会尝试自动安装（需要管理员权限）
- 如果安装失败，会提示"请以管理员身份运行"

### 2. 选择进程

程序会自动列出所有 Java 进程（包括 java.exe 和 javaw.exe）：
- 找到 Minecraft 对应的 `javaw.exe` 进程
- 从下拉列表中选择正确的进程

### 3. 注入 DLL

点击"注入"按钮，程序会：
1. 自动提取内置的 DLL 到临时目录
2. 将 DLL 注入到选定的进程
3. 显示注入结果

## 技术细节

### 注入方式

使用 `LoadLibraryW` API 进行 DLL 注入：
- 将 DLL 文件路径写入目标进程内存
- 创建远程线程调用 `LoadLibraryW`
- 系统自动处理 PE 解析、重定位、导入表等

### 架构检查

注入器会自动检查：
- 目标进程架构（32位/64位）
- 注入器架构
- DLL 架构
- 确保三者匹配

### DLL 嵌入

DLL 文件作为嵌入资源包含在 EXE 中：
- **InfiniteGUI-DLL.dll**：主 GUI DLL
- **glew32.dll**：OpenGL 扩展库
- 运行时提取到临时目录
- 注入完成后自动清理
- 便于单个文件分发

### glew32.dll 自动安装

程序启动时会：
1. 检查 `C:\Windows\System32\glew32.dll` 是否存在
2. 如果不存在，尝试从嵌入资源提取并安装
3. 安装需要管理员权限
4. 如果安装失败，提示用户以管理员身份运行

## 编译

### 开发环境

- .NET 8.0 SDK
- Windows 10/11

### 编译命令

```bash
cd DLLInjector
dotnet build -c Release
```

### 发布单个文件

```bash
cd DLLInjector
dotnet publish -c Release -r win-x64 --self-contained false /p:PublishSingleFile=true
```

或者直接运行 `publish.bat` 脚本。

生成的单个 EXE 文件位于：
```
bin\Release\net8.0-windows\win-x64\publish\InfiniteGUIInjector.exe
```

这个文件包含了所有必要的资源，可以直接运行，无需其他文件。

## 文件结构

```
DLLInjector/
├── bin/Release/net8.0-windows/win-x64/publish/
│   ├── InfiniteGUIInjector.exe          # 主程序（单个文件）
│   └── InfiniteGUIInjector.pdb          # 调试符号
├── dll/
│   ├── InfiniteGUI-DLL.dll              # 内置的GUI DLL（嵌入资源）
│   └── glew32.dll                     # OpenGL扩展库（嵌入资源）
├── Properties/
│   └── Resources.Designer.cs           # 资源管理器
├── MainForm.cs                         # 主窗体
├── ProcessHelper.cs                    # 进程检测
├── SafeReflectiveInjector.cs          # 安全注入器
├── DLLInjector.csproj                  # 项目文件
├── LICENSE                             # MIT 许可证
├── README.md                           # 本文件
├── CONTRIBUTING.md                     # 贡献指南
├── build.bat                           # 编译脚本
├── run.bat                            # 运行脚本
└── publish.bat                         # 发布脚本
```

**注意**：`dll/` 文件夹中的 `InfiniteGUI-DLL.dll` 和 `glew32.dll` 是构建时需要的依赖文件，它们会被嵌入到最终生成的EXE文件中。

## 注意事项

1. **管理员权限**：
   - 程序启动时不需要管理员权限
   - 只在需要安装 glew32.dll 时才会请求管理员权限
   - 如果安装失败，请以管理员身份运行

2. **glew32.dll**：
   - 程序会自动检查和安装
   - 如果手动安装失败，请检查 System32 目录权限

## 致谢

感谢所有为 Minecraft 模组开发做出贡献的开发者。

特别感谢：
- QCMaxcer - InfiniteGUI开发者
- TRAEAI - 程序开发
- ImGui - 即时模式 GUI 库
- OpenGL - 图形 API
- LWJGL - Java 轻量级游戏库
- Minecraft 社区

## 许可证

本项目采用 MIT 许可证 - 详见 [LICENSE](LICENSE) 文件

## 免责声明

本项目仅供学习和研究使用。使用本软件造成的任何损失或损害，作者不承担责任。


**Made with ❤️ for the Minecraft Community**
