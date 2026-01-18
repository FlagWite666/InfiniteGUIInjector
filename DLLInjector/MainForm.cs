using System;
using System.Drawing;
using System.Windows.Forms;
using System.IO;
using System.Reflection;

namespace DLLInjector
{
    public partial class MainForm : Form
    {
        private Button btnRefresh;
        private Button btnInject;
        private Button btnClose;
        private ComboBox cmbProcessList;
        private Label lblProcessList;
        private Label lblStatus;
        private System.Windows.Forms.Timer refreshTimer;

        private const int BorderThickness = 1;
        private const int TitleBarHeight = 32;
        private bool isDragging = false;
        private Point dragCursorPoint;
        private Point dragFormPoint;
        private string embeddedDLLPath;

        public MainForm()
        {
            InitializeComponent();
            CheckAndInstallGLEW32();
            ExtractEmbeddedDLL();
            LoadProcesses();
        }

        private void InitializeComponent()
        {
            this.Text = "InfiniteGUI注入器";
            this.Size = new Size(500, 280);
            this.StartPosition = FormStartPosition.CenterScreen;
            this.FormBorderStyle = FormBorderStyle.None;
            this.MaximizeBox = false;
            this.BackColor = Color.FromArgb(45, 45, 48);
            this.ForeColor = Color.White;
            this.SetStyle(ControlStyles.AllPaintingInWmPaint | ControlStyles.UserPaint | ControlStyles.DoubleBuffer, true);

            btnClose = new Button
            {
                Text = "✕",
                Location = new Point(460, 8),
                Size = new Size(30, 20),
                BackColor = Color.Transparent,
                ForeColor = Color.White,
                FlatStyle = FlatStyle.Flat,
                Cursor = Cursors.Hand,
                Font = new Font("Segoe UI", 9F)
            };
            btnClose.FlatAppearance.BorderSize = 0;
            btnClose.MouseEnter += (s, e) => btnClose.BackColor = Color.FromArgb(232, 17, 35);
            btnClose.MouseLeave += (s, e) => btnClose.BackColor = Color.Transparent;
            btnClose.Click += (s, e) => this.Close();

            lblProcessList = new Label
            {
                Text = "选择 Java 进程:",
                Location = new Point(20, 50),
                Size = new Size(200, 23),
                ForeColor = Color.White,
                BackColor = Color.Transparent
            };

            cmbProcessList = new ComboBox
            {
                Location = new Point(20, 80),
                Size = new Size(440, 25),
                DropDownStyle = ComboBoxStyle.DropDownList,
                BackColor = Color.FromArgb(60, 60, 65),
                ForeColor = Color.White,
                FlatStyle = FlatStyle.Flat
            };

            btnRefresh = new Button
            {
                Text = "刷新",
                Location = new Point(20, 115),
                Size = new Size(100, 30),
                BackColor = Color.FromArgb(0, 120, 215),
                ForeColor = Color.White,
                FlatStyle = FlatStyle.Flat,
                Cursor = Cursors.Hand
            };
            btnRefresh.FlatAppearance.BorderSize = 0;
            btnRefresh.Click += BtnRefresh_Click;

            btnInject = new Button
            {
                Text = "注入",
                Location = new Point(20, 160),
                Size = new Size(440, 40),
                BackColor = Color.FromArgb(16, 185, 129),
                ForeColor = Color.White,
                FlatStyle = FlatStyle.Flat,
                Font = new Font("Microsoft YaHei UI", 10F, FontStyle.Bold),
                Cursor = Cursors.Hand
            };
            btnInject.FlatAppearance.BorderSize = 0;
            btnInject.Click += BtnInject_Click;

            lblStatus = new Label
            {
                Text = "准备就绪",
                Location = new Point(20, 220),
                Size = new Size(440, 23),
                ForeColor = Color.FromArgb(156, 220, 254),
                BackColor = Color.Transparent
            };

            this.Controls.Add(btnClose);
            this.Controls.Add(lblProcessList);
            this.Controls.Add(cmbProcessList);
            this.Controls.Add(btnRefresh);
            this.Controls.Add(btnInject);
            this.Controls.Add(lblStatus);

            this.MouseDown += MainForm_MouseDown;
            this.MouseMove += MainForm_MouseMove;
            this.MouseUp += MainForm_MouseUp;
            this.Paint += MainForm_Paint;

            refreshTimer = new System.Windows.Forms.Timer
            {
                Interval = 3000
            };
            refreshTimer.Tick += RefreshTimer_Tick;
            refreshTimer.Start();
        }

        private void CheckAndInstallGLEW32()
        {
            try
            {
                string system32Path = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.System), "glew32.dll");
                bool needsInstallation = !File.Exists(system32Path);

                if (needsInstallation)
                {
                    lblStatus.Text = "正在安装 glew32.dll...";
                    Application.DoEvents();

                    bool success = TryInstallGLEW32();
                    
                    if (success)
                    {
                        lblStatus.Text = "glew32.dll 安装成功";
                    }
                    else
                    {
                        lblStatus.Text = "glew32.dll 安装失败，请以管理员身份运行";
                    }
                }
                else
                {
                    lblStatus.Text = "glew32.dll 已安装";
                }
            }
            catch (Exception ex)
            {
                lblStatus.Text = $"检查 glew32.dll 失败: {ex.Message}";
            }
        }

        private bool TryInstallGLEW32()
        {
            try
            {
                var assembly = System.Reflection.Assembly.GetExecutingAssembly();
                
                using (var stream = assembly.GetManifestResourceStream("DLLInjector.glew32.dll"))
                {
                    if (stream == null)
                    {
                        return false;
                    }

                    byte[] dllBytes = new byte[stream.Length];
                    stream.Read(dllBytes, 0, dllBytes.Length);

                    string system32Path = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.System), "glew32.dll");
                    
                    File.WriteAllBytes(system32Path, dllBytes);
                    return true;
                }
            }
            catch
            {
                return false;
            }
        }

        private void ExtractEmbeddedDLL()
        {
            try
            {
                string tempPath = Path.GetTempPath();
                embeddedDLLPath = Path.Combine(tempPath, "InfiniteGUI-DLL.dll");
 
                if (File.Exists(embeddedDLLPath))
                {
                    File.Delete(embeddedDLLPath);
                }
 
                var assembly = System.Reflection.Assembly.GetExecutingAssembly();
                
                // 列出所有嵌入的资源
                var resourceNames = assembly.GetManifestResourceNames();
                lblStatus.Text = $"正在查找资源... 找到 {resourceNames.Length} 个资源";
                Application.DoEvents();
 
                // 尝试不同的资源名称
                string[] possibleNames = new[]
                {
                    "DLLInjector.InfiniteGUI_DLL.dll",
                    "DLLInjector.x64.Release.InfiniteGUI-DLL.dll",
                    "InfiniteGUI_DLL.dll",
                    "InfiniteGUI-DLL.dll"
                };
 
                byte[] dllBytes = null;
                string foundResourceName = null;
 
                foreach (string resourceName in possibleNames)
                {
                    using (var stream = assembly.GetManifestResourceStream(resourceName))
                    {
                        if (stream != null)
                        {
                            dllBytes = new byte[stream.Length];
                            stream.Read(dllBytes, 0, dllBytes.Length);
                            foundResourceName = resourceName;
                            break;
                        }
                    }
                }
 
                if (dllBytes == null)
                {
                    lblStatus.Text = $"未找到嵌入的DLL资源。可用资源: {string.Join(", ", resourceNames)}";
                    return;
                }
 
                File.WriteAllBytes(embeddedDLLPath, dllBytes);
                lblStatus.Text = $"DLL 已提取 (资源: {foundResourceName})";
            }
            catch (Exception ex)
            {
                lblStatus.Text = $"DLL 提取失败: {ex.Message}";
            }
        }

        private void LoadProcesses()
        {
            var processes = ProcessHelper.GetJavaProcesses();
            cmbProcessList.Items.Clear();

            foreach (var process in processes)
            {
                cmbProcessList.Items.Add(process);
            }

            if (cmbProcessList.Items.Count > 0)
            {
                cmbProcessList.SelectedIndex = 0;
                lblStatus.Text = $"找到 {cmbProcessList.Items.Count} 个 Java 进程";
            }
            else
            {
                lblStatus.Text = "未找到 Java 进程，请先启动 Minecraft";
            }
        }

        private void BtnRefresh_Click(object sender, EventArgs e)
        {
            LoadProcesses();
        }

        private void RefreshTimer_Tick(object sender, EventArgs e)
        {
            var currentProcesses = ProcessHelper.GetJavaProcesses();
            bool needsRefresh = false;

            if (cmbProcessList.Items.Count != currentProcesses.Count)
            {
                needsRefresh = true;
            }
            else
            {
                for (int i = 0; i < currentProcesses.Count; i++)
                {
                    var current = currentProcesses[i];
                    if (i < cmbProcessList.Items.Count)
                    {
                        var existing = (ProcessInfo)cmbProcessList.Items[i];
                        if (existing.ProcessId != current.ProcessId)
                        {
                            needsRefresh = true;
                            break;
                        }
                    }
                }
            }

            if (needsRefresh)
            {
                LoadProcesses();
            }
        }

        private void BtnInject_Click(object sender, EventArgs e)
        {
            if (cmbProcessList.SelectedItem == null)
            {
                MessageBox.Show("请先选择一个进程", "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            if (string.IsNullOrEmpty(embeddedDLLPath) || !File.Exists(embeddedDLLPath))
            {
                MessageBox.Show("DLL 文件未找到，请重新启动程序", "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            var selectedProcess = (ProcessInfo)cmbProcessList.SelectedItem;

            btnInject.Enabled = false;
            btnInject.Text = "注入中...";
            lblStatus.Text = "正在注入 DLL...";
            Application.DoEvents();

            bool success = SafeReflectiveInjector.InjectDLL(selectedProcess.ProcessId, embeddedDLLPath, out string errorMessage);

            btnInject.Enabled = true;
            btnInject.Text = "注入";

            if (success)
            {
                lblStatus.Text = $"成功注入到进程 {selectedProcess.ProcessName} (PID: {selectedProcess.ProcessId})";
                MessageBox.Show($"DLL 注入成功！\n\n进程: {selectedProcess.ProcessName}\nPID: {selectedProcess.ProcessId}", "成功", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
            else
            {
                lblStatus.Text = $"注入失败: {errorMessage}";
                MessageBox.Show($"DLL 注入失败！\n\n错误信息: {errorMessage}", "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void MainForm_MouseDown(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left && e.Y < TitleBarHeight)
            {
                isDragging = true;
                dragCursorPoint = Cursor.Position;
                dragFormPoint = this.Location;
            }
        }

        private void MainForm_MouseMove(object sender, MouseEventArgs e)
        {
            if (isDragging)
            {
                Point dif = Point.Subtract(Cursor.Position, new Size(dragCursorPoint));
                this.Location = Point.Add(dragFormPoint, new Size(dif));
            }
        }

        private void MainForm_MouseUp(object sender, MouseEventArgs e)
        {
            isDragging = false;
        }

        private void MainForm_Paint(object sender, PaintEventArgs e)
        {
            Graphics g = e.Graphics;
            Rectangle rect = this.ClientRectangle;

            using (Pen borderPen = new Pen(Color.FromArgb(80, 80, 80), BorderThickness))
            {
                g.DrawRectangle(borderPen, rect.X, rect.Y, rect.Width - 1, rect.Height - 1);
            }

            using (Brush titleBrush = new SolidBrush(Color.FromArgb(35, 35, 38)))
            {
                g.FillRectangle(titleBrush, new Rectangle(0, 0, rect.Width, TitleBarHeight));
            }

            using (Brush titleLineBrush = new SolidBrush(Color.FromArgb(80, 80, 80)))
            {
                g.FillRectangle(titleLineBrush, new Rectangle(0, TitleBarHeight - 1, rect.Width, 1));
            }

            using (Font titleFont = new Font("Microsoft YaHei UI", 9F, FontStyle.Regular))
            using (Brush titleTextBrush = new SolidBrush(Color.White))
            {
                StringFormat sf = new StringFormat
                {
                    Alignment = StringAlignment.Near,
                    LineAlignment = StringAlignment.Center
                };
                g.DrawString("InfiniteGUI注入器", titleFont, titleTextBrush, new RectangleF(10, 0, rect.Width - 50, TitleBarHeight), sf);
            }
        }

        protected override void OnFormClosing(FormClosingEventArgs e)
        {
            refreshTimer?.Stop();
            refreshTimer?.Dispose();
            
            if (!string.IsNullOrEmpty(embeddedDLLPath) && File.Exists(embeddedDLLPath))
            {
                try
                {
                    File.Delete(embeddedDLLPath);
                }
                catch { }
            }
            
            base.OnFormClosing(e);
        }
    }
}