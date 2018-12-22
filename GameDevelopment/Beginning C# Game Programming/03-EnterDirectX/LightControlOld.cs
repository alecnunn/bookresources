Imports Microsoft.DirectX
Imports Microsoft.DirectX.Direct3D

Public Class LightControl
    Inherits System.Windows.Forms.Form
#Region " Windows Form Designer generated code "

    Public Sub New()
        MyBase.New()

        'This call is required by the Windows Form Designer.
        InitializeComponent()

        'Add any initialization after the InitializeComponent() call

    End Sub

    'Form overrides dispose to clean up the component list.
    Protected Overloads Overrides Sub Dispose(ByVal disposing As Boolean)
        If disposing Then
            If Not (components Is Nothing) Then
                components.Dispose()
            End If
        End If
        MyBase.Dispose(disposing)
    End Sub

    'Required by the Windows Form Designer
    Private components As System.ComponentModel.IContainer

    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.  
    'Do not modify it using the code editor.
    Friend WithEvents TabControl1 As System.Windows.Forms.TabControl
    Friend WithEvents Vertex1 As System.Windows.Forms.TabPage
    Friend WithEvents Vertex2 As System.Windows.Forms.TabPage
    Friend WithEvents Vertex3 As System.Windows.Forms.TabPage
    Friend WithEvents Vertex4 As System.Windows.Forms.TabPage
    Friend WithEvents GroupBox1 As System.Windows.Forms.GroupBox
    Friend WithEvents lblBlue1 As System.Windows.Forms.Label
    Friend WithEvents lblGreen1 As System.Windows.Forms.Label
    Friend WithEvents lblRed1 As System.Windows.Forms.Label
    Friend WithEvents GroupBox2 As System.Windows.Forms.GroupBox
    Friend WithEvents GroupBox3 As System.Windows.Forms.GroupBox
    Friend WithEvents GroupBox4 As System.Windows.Forms.GroupBox
    Friend WithEvents Red1 As System.Windows.Forms.NumericUpDown
    Friend WithEvents Green2 As System.Windows.Forms.NumericUpDown
    Friend WithEvents Red2 As System.Windows.Forms.NumericUpDown
    Friend WithEvents Blue1 As System.Windows.Forms.NumericUpDown
    Friend WithEvents Green1 As System.Windows.Forms.NumericUpDown
    Friend WithEvents lblBlue2 As System.Windows.Forms.Label
    Friend WithEvents BLue2 As System.Windows.Forms.NumericUpDown
    Friend WithEvents lblGreen2 As System.Windows.Forms.Label
    Friend WithEvents LblRed2 As System.Windows.Forms.Label
    Friend WithEvents lblBlue3 As System.Windows.Forms.Label
    Friend WithEvents Blue3 As System.Windows.Forms.NumericUpDown
    Friend WithEvents lblGreen3 As System.Windows.Forms.Label
    Friend WithEvents Green3 As System.Windows.Forms.NumericUpDown
    Friend WithEvents lblRed3 As System.Windows.Forms.Label
    Friend WithEvents Red3 As System.Windows.Forms.NumericUpDown
    Friend WithEvents lblBlue4 As System.Windows.Forms.Label
    Friend WithEvents Blue4 As System.Windows.Forms.NumericUpDown
    Friend WithEvents lblGreen4 As System.Windows.Forms.Label
    Friend WithEvents lblRed4 As System.Windows.Forms.Label
    Friend WithEvents Green4 As System.Windows.Forms.NumericUpDown
    Friend WithEvents Red4 As System.Windows.Forms.NumericUpDown
    <System.Diagnostics.DebuggerStepThrough()> Private Sub InitializeComponent()
        Me.TabControl1 = New System.Windows.Forms.TabControl
        Me.Vertex1 = New System.Windows.Forms.TabPage
        Me.GroupBox1 = New System.Windows.Forms.GroupBox
        Me.lblBlue1 = New System.Windows.Forms.Label
        Me.Blue1 = New System.Windows.Forms.NumericUpDown
        Me.lblGreen1 = New System.Windows.Forms.Label
        Me.Green1 = New System.Windows.Forms.NumericUpDown
        Me.lblRed1 = New System.Windows.Forms.Label
        Me.Red1 = New System.Windows.Forms.NumericUpDown
        Me.Vertex2 = New System.Windows.Forms.TabPage
        Me.GroupBox2 = New System.Windows.Forms.GroupBox
        Me.lblBlue2 = New System.Windows.Forms.Label
        Me.BLue2 = New System.Windows.Forms.NumericUpDown
        Me.lblGreen2 = New System.Windows.Forms.Label
        Me.Green2 = New System.Windows.Forms.NumericUpDown
        Me.LblRed2 = New System.Windows.Forms.Label
        Me.Red2 = New System.Windows.Forms.NumericUpDown
        Me.Vertex3 = New System.Windows.Forms.TabPage
        Me.GroupBox3 = New System.Windows.Forms.GroupBox
        Me.lblBlue3 = New System.Windows.Forms.Label
        Me.Blue3 = New System.Windows.Forms.NumericUpDown
        Me.lblGreen3 = New System.Windows.Forms.Label
        Me.Green3 = New System.Windows.Forms.NumericUpDown
        Me.lblRed3 = New System.Windows.Forms.Label
        Me.Red3 = New System.Windows.Forms.NumericUpDown
        Me.Vertex4 = New System.Windows.Forms.TabPage
        Me.GroupBox4 = New System.Windows.Forms.GroupBox
        Me.lblBlue4 = New System.Windows.Forms.Label
        Me.Blue4 = New System.Windows.Forms.NumericUpDown
        Me.lblGreen4 = New System.Windows.Forms.Label
        Me.Green4 = New System.Windows.Forms.NumericUpDown
        Me.lblRed4 = New System.Windows.Forms.Label
        Me.Red4 = New System.Windows.Forms.NumericUpDown
        Me.TabControl1.SuspendLayout()
        Me.Vertex1.SuspendLayout()
        Me.GroupBox1.SuspendLayout()
        CType(Me.Blue1, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.Green1, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.Red1, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.Vertex2.SuspendLayout()
        Me.GroupBox2.SuspendLayout()
        CType(Me.BLue2, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.Green2, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.Red2, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.Vertex3.SuspendLayout()
        Me.GroupBox3.SuspendLayout()
        CType(Me.Blue3, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.Green3, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.Red3, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.Vertex4.SuspendLayout()
        Me.GroupBox4.SuspendLayout()
        CType(Me.Blue4, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.Green4, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.Red4, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'TabControl1
        '
        Me.TabControl1.Controls.AddRange(New System.Windows.Forms.Control() {Me.Vertex1, Me.Vertex2, Me.Vertex3, Me.Vertex4})
        Me.TabControl1.Font = New System.Drawing.Font("Microsoft Sans Serif", 9.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.TabControl1.Location = New System.Drawing.Point(8, 8)
        Me.TabControl1.Name = "TabControl1"
        Me.TabControl1.SelectedIndex = 0
        Me.TabControl1.Size = New System.Drawing.Size(264, 192)
        Me.TabControl1.TabIndex = 8
        '
        'Vertex1
        '
        Me.Vertex1.Controls.AddRange(New System.Windows.Forms.Control() {Me.GroupBox1})
        Me.Vertex1.Location = New System.Drawing.Point(4, 24)
        Me.Vertex1.Name = "Vertex1"
        Me.Vertex1.Size = New System.Drawing.Size(256, 164)
        Me.Vertex1.TabIndex = 0
        Me.Vertex1.Text = "Vertex 1"
        '
        'GroupBox1
        '
        Me.GroupBox1.Controls.AddRange(New System.Windows.Forms.Control() {Me.lblBlue1, Me.Blue1, Me.lblGreen1, Me.Green1, Me.lblRed1, Me.Red1})
        Me.GroupBox1.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.GroupBox1.Location = New System.Drawing.Point(32, 8)
        Me.GroupBox1.Name = "GroupBox1"
        Me.GroupBox1.Size = New System.Drawing.Size(184, 144)
        Me.GroupBox1.TabIndex = 2
        Me.GroupBox1.TabStop = False
        Me.GroupBox1.Text = "Color"
        '
        'lblBlue1
        '
        Me.lblBlue1.Font = New System.Drawing.Font("Microsoft Sans Serif", 14.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.lblBlue1.Location = New System.Drawing.Point(8, 106)
        Me.lblBlue1.Name = "lblBlue1"
        Me.lblBlue1.Size = New System.Drawing.Size(80, 24)
        Me.lblBlue1.TabIndex = 5
        Me.lblBlue1.Text = "Blue"
        '
        'Blue1
        '
        Me.Blue1.Font = New System.Drawing.Font("Microsoft Sans Serif", 14.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Blue1.Increment = New Decimal(New Integer() {5, 0, 0, 0})
        Me.Blue1.Location = New System.Drawing.Point(96, 104)
        Me.Blue1.Maximum = New Decimal(New Integer() {255, 0, 0, 0})
        Me.Blue1.Name = "Blue1"
        Me.Blue1.Size = New System.Drawing.Size(56, 29)
        Me.Blue1.TabIndex = 4
        '
        'lblGreen1
        '
        Me.lblGreen1.Font = New System.Drawing.Font("Microsoft Sans Serif", 14.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.lblGreen1.Location = New System.Drawing.Point(8, 66)
        Me.lblGreen1.Name = "lblGreen1"
        Me.lblGreen1.Size = New System.Drawing.Size(80, 24)
        Me.lblGreen1.TabIndex = 3
        Me.lblGreen1.Text = "Green"
        '
        'Green1
        '
        Me.Green1.Font = New System.Drawing.Font("Microsoft Sans Serif", 14.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Green1.Increment = New Decimal(New Integer() {5, 0, 0, 0})
        Me.Green1.Location = New System.Drawing.Point(96, 64)
        Me.Green1.Maximum = New Decimal(New Integer() {255, 0, 0, 0})
        Me.Green1.Name = "Green1"
        Me.Green1.Size = New System.Drawing.Size(56, 29)
        Me.Green1.TabIndex = 2
        '
        'lblRed1
        '
        Me.lblRed1.Font = New System.Drawing.Font("Microsoft Sans Serif", 14.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.lblRed1.Location = New System.Drawing.Point(8, 26)
        Me.lblRed1.Name = "lblRed1"
        Me.lblRed1.Size = New System.Drawing.Size(80, 24)
        Me.lblRed1.TabIndex = 1
        Me.lblRed1.Text = "Red"
        '
        'Red1
        '
        Me.Red1.Font = New System.Drawing.Font("Microsoft Sans Serif", 14.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Red1.Increment = New Decimal(New Integer() {5, 0, 0, 0})
        Me.Red1.Location = New System.Drawing.Point(96, 24)
        Me.Red1.Maximum = New Decimal(New Integer() {255, 0, 0, 0})
        Me.Red1.Name = "Red1"
        Me.Red1.Size = New System.Drawing.Size(56, 29)
        Me.Red1.TabIndex = 0
        '
        'Vertex2
        '
        Me.Vertex2.Controls.AddRange(New System.Windows.Forms.Control() {Me.GroupBox2})
        Me.Vertex2.Location = New System.Drawing.Point(4, 24)
        Me.Vertex2.Name = "Vertex2"
        Me.Vertex2.Size = New System.Drawing.Size(256, 164)
        Me.Vertex2.TabIndex = 1
        Me.Vertex2.Text = "Vertex2"
        '
        'GroupBox2
        '
        Me.GroupBox2.Controls.AddRange(New System.Windows.Forms.Control() {Me.lblBlue2, Me.BLue2, Me.lblGreen2, Me.Green2, Me.LblRed2, Me.Red2})
        Me.GroupBox2.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.GroupBox2.Location = New System.Drawing.Point(32, 8)
        Me.GroupBox2.Name = "GroupBox2"
        Me.GroupBox2.Size = New System.Drawing.Size(184, 144)
        Me.GroupBox2.TabIndex = 3
        Me.GroupBox2.TabStop = False
        Me.GroupBox2.Text = "Color"
        '
        'lblBlue2
        '
        Me.lblBlue2.Font = New System.Drawing.Font("Microsoft Sans Serif", 14.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.lblBlue2.Location = New System.Drawing.Point(8, 106)
        Me.lblBlue2.Name = "lblBlue2"
        Me.lblBlue2.Size = New System.Drawing.Size(80, 24)
        Me.lblBlue2.TabIndex = 5
        Me.lblBlue2.Text = "Blue"
        '
        'BLue2
        '
        Me.BLue2.Font = New System.Drawing.Font("Microsoft Sans Serif", 14.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.BLue2.Increment = New Decimal(New Integer() {5, 0, 0, 0})
        Me.BLue2.Location = New System.Drawing.Point(96, 104)
        Me.BLue2.Maximum = New Decimal(New Integer() {255, 0, 0, 0})
        Me.BLue2.Name = "BLue2"
        Me.BLue2.Size = New System.Drawing.Size(56, 29)
        Me.BLue2.TabIndex = 4
        '
        'lblGreen2
        '
        Me.lblGreen2.Font = New System.Drawing.Font("Microsoft Sans Serif", 14.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.lblGreen2.Location = New System.Drawing.Point(8, 66)
        Me.lblGreen2.Name = "lblGreen2"
        Me.lblGreen2.Size = New System.Drawing.Size(80, 24)
        Me.lblGreen2.TabIndex = 3
        Me.lblGreen2.Text = "Green"
        '
        'Green2
        '
        Me.Green2.Font = New System.Drawing.Font("Microsoft Sans Serif", 14.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Green2.Increment = New Decimal(New Integer() {5, 0, 0, 0})
        Me.Green2.Location = New System.Drawing.Point(96, 64)
        Me.Green2.Maximum = New Decimal(New Integer() {255, 0, 0, 0})
        Me.Green2.Name = "Green2"
        Me.Green2.Size = New System.Drawing.Size(56, 29)
        Me.Green2.TabIndex = 2
        '
        'LblRed2
        '
        Me.LblRed2.Font = New System.Drawing.Font("Microsoft Sans Serif", 14.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.LblRed2.Location = New System.Drawing.Point(8, 26)
        Me.LblRed2.Name = "LblRed2"
        Me.LblRed2.Size = New System.Drawing.Size(80, 24)
        Me.LblRed2.TabIndex = 1
        Me.LblRed2.Text = "Red"
        '
        'Red2
        '
        Me.Red2.Font = New System.Drawing.Font("Microsoft Sans Serif", 14.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Red2.Increment = New Decimal(New Integer() {5, 0, 0, 0})
        Me.Red2.Location = New System.Drawing.Point(96, 24)
        Me.Red2.Maximum = New Decimal(New Integer() {255, 0, 0, 0})
        Me.Red2.Name = "Red2"
        Me.Red2.Size = New System.Drawing.Size(56, 29)
        Me.Red2.TabIndex = 0
        '
        'Vertex3
        '
        Me.Vertex3.Controls.AddRange(New System.Windows.Forms.Control() {Me.GroupBox3})
        Me.Vertex3.Location = New System.Drawing.Point(4, 24)
        Me.Vertex3.Name = "Vertex3"
        Me.Vertex3.Size = New System.Drawing.Size(256, 164)
        Me.Vertex3.TabIndex = 2
        Me.Vertex3.Text = "Vertex 3"
        '
        'GroupBox3
        '
        Me.GroupBox3.Controls.AddRange(New System.Windows.Forms.Control() {Me.lblBlue3, Me.Blue3, Me.lblGreen3, Me.Green3, Me.lblRed3, Me.Red3})
        Me.GroupBox3.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.GroupBox3.Location = New System.Drawing.Point(32, 8)
        Me.GroupBox3.Name = "GroupBox3"
        Me.GroupBox3.Size = New System.Drawing.Size(184, 144)
        Me.GroupBox3.TabIndex = 3
        Me.GroupBox3.TabStop = False
        Me.GroupBox3.Text = "Color"
        '
        'lblBlue3
        '
        Me.lblBlue3.Font = New System.Drawing.Font("Microsoft Sans Serif", 14.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.lblBlue3.Location = New System.Drawing.Point(8, 106)
        Me.lblBlue3.Name = "lblBlue3"
        Me.lblBlue3.Size = New System.Drawing.Size(80, 24)
        Me.lblBlue3.TabIndex = 5
        Me.lblBlue3.Text = "Blue"
        '
        'Blue3
        '
        Me.Blue3.Font = New System.Drawing.Font("Microsoft Sans Serif", 14.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Blue3.Increment = New Decimal(New Integer() {5, 0, 0, 0})
        Me.Blue3.Location = New System.Drawing.Point(96, 104)
        Me.Blue3.Maximum = New Decimal(New Integer() {255, 0, 0, 0})
        Me.Blue3.Name = "Blue3"
        Me.Blue3.Size = New System.Drawing.Size(56, 29)
        Me.Blue3.TabIndex = 4
        '
        'lblGreen3
        '
        Me.lblGreen3.Font = New System.Drawing.Font("Microsoft Sans Serif", 14.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.lblGreen3.Location = New System.Drawing.Point(8, 66)
        Me.lblGreen3.Name = "lblGreen3"
        Me.lblGreen3.Size = New System.Drawing.Size(80, 24)
        Me.lblGreen3.TabIndex = 3
        Me.lblGreen3.Text = "Green"
        '
        'Green3
        '
        Me.Green3.Font = New System.Drawing.Font("Microsoft Sans Serif", 14.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Green3.Increment = New Decimal(New Integer() {5, 0, 0, 0})
        Me.Green3.Location = New System.Drawing.Point(96, 64)
        Me.Green3.Maximum = New Decimal(New Integer() {255, 0, 0, 0})
        Me.Green3.Name = "Green3"
        Me.Green3.Size = New System.Drawing.Size(56, 29)
        Me.Green3.TabIndex = 2
        '
        'lblRed3
        '
        Me.lblRed3.Font = New System.Drawing.Font("Microsoft Sans Serif", 14.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.lblRed3.Location = New System.Drawing.Point(8, 26)
        Me.lblRed3.Name = "lblRed3"
        Me.lblRed3.Size = New System.Drawing.Size(80, 24)
        Me.lblRed3.TabIndex = 1
        Me.lblRed3.Text = "Red"
        '
        'Red3
        '
        Me.Red3.Font = New System.Drawing.Font("Microsoft Sans Serif", 14.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Red3.Increment = New Decimal(New Integer() {5, 0, 0, 0})
        Me.Red3.Location = New System.Drawing.Point(96, 24)
        Me.Red3.Maximum = New Decimal(New Integer() {255, 0, 0, 0})
        Me.Red3.Name = "Red3"
        Me.Red3.Size = New System.Drawing.Size(56, 29)
        Me.Red3.TabIndex = 0
        '
        'Vertex4
        '
        Me.Vertex4.Controls.AddRange(New System.Windows.Forms.Control() {Me.GroupBox4})
        Me.Vertex4.Location = New System.Drawing.Point(4, 24)
        Me.Vertex4.Name = "Vertex4"
        Me.Vertex4.Size = New System.Drawing.Size(256, 164)
        Me.Vertex4.TabIndex = 3
        Me.Vertex4.Text = "Vertex 4"
        '
        'GroupBox4
        '
        Me.GroupBox4.Controls.AddRange(New System.Windows.Forms.Control() {Me.lblBlue4, Me.Blue4, Me.lblGreen4, Me.Green4, Me.lblRed4, Me.Red4})
        Me.GroupBox4.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.GroupBox4.Location = New System.Drawing.Point(32, 8)
        Me.GroupBox4.Name = "GroupBox4"
        Me.GroupBox4.Size = New System.Drawing.Size(184, 144)
        Me.GroupBox4.TabIndex = 3
        Me.GroupBox4.TabStop = False
        Me.GroupBox4.Text = "Color"
        '
        'lblBlue4
        '
        Me.lblBlue4.Font = New System.Drawing.Font("Microsoft Sans Serif", 14.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.lblBlue4.Location = New System.Drawing.Point(8, 106)
        Me.lblBlue4.Name = "lblBlue4"
        Me.lblBlue4.Size = New System.Drawing.Size(80, 24)
        Me.lblBlue4.TabIndex = 5
        Me.lblBlue4.Text = "Blue"
        '
        'Blue4
        '
        Me.Blue4.Font = New System.Drawing.Font("Microsoft Sans Serif", 14.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Blue4.Increment = New Decimal(New Integer() {5, 0, 0, 0})
        Me.Blue4.Location = New System.Drawing.Point(96, 104)
        Me.Blue4.Maximum = New Decimal(New Integer() {255, 0, 0, 0})
        Me.Blue4.Name = "Blue4"
        Me.Blue4.Size = New System.Drawing.Size(56, 29)
        Me.Blue4.TabIndex = 4
        '
        'lblGreen4
        '
        Me.lblGreen4.Font = New System.Drawing.Font("Microsoft Sans Serif", 14.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.lblGreen4.Location = New System.Drawing.Point(8, 66)
        Me.lblGreen4.Name = "lblGreen4"
        Me.lblGreen4.Size = New System.Drawing.Size(80, 24)
        Me.lblGreen4.TabIndex = 3
        Me.lblGreen4.Text = "Green"
        '
        'Green4
        '
        Me.Green4.Font = New System.Drawing.Font("Microsoft Sans Serif", 14.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Green4.Increment = New Decimal(New Integer() {5, 0, 0, 0})
        Me.Green4.Location = New System.Drawing.Point(96, 64)
        Me.Green4.Maximum = New Decimal(New Integer() {255, 0, 0, 0})
        Me.Green4.Name = "Green4"
        Me.Green4.Size = New System.Drawing.Size(56, 29)
        Me.Green4.TabIndex = 2
        '
        'lblRed4
        '
        Me.lblRed4.Font = New System.Drawing.Font("Microsoft Sans Serif", 14.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.lblRed4.Location = New System.Drawing.Point(8, 26)
        Me.lblRed4.Name = "lblRed4"
        Me.lblRed4.Size = New System.Drawing.Size(80, 24)
        Me.lblRed4.TabIndex = 1
        Me.lblRed4.Text = "Red"
        '
        'Red4
        '
        Me.Red4.Font = New System.Drawing.Font("Microsoft Sans Serif", 14.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Red4.Increment = New Decimal(New Integer() {5, 0, 0, 0})
        Me.Red4.Location = New System.Drawing.Point(96, 24)
        Me.Red4.Maximum = New Decimal(New Integer() {255, 0, 0, 0})
        Me.Red4.Name = "Red4"
        Me.Red4.Size = New System.Drawing.Size(56, 29)
        Me.Red4.TabIndex = 0
        '
        'LightControl
        '
        Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
        Me.ClientSize = New System.Drawing.Size(282, 206)
        Me.Controls.AddRange(New System.Windows.Forms.Control() {Me.TabControl1})
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle
        Me.KeyPreview = True
        Me.MaximizeBox = False
        Me.MinimizeBox = False
        Me.Name = "LightControl"
        Me.Text = "Light  Control"
        Me.TabControl1.ResumeLayout(False)
        Me.Vertex1.ResumeLayout(False)
        Me.GroupBox1.ResumeLayout(False)
        CType(Me.Blue1, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.Green1, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.Red1, System.ComponentModel.ISupportInitialize).EndInit()
        Me.Vertex2.ResumeLayout(False)
        Me.GroupBox2.ResumeLayout(False)
        CType(Me.BLue2, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.Green2, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.Red2, System.ComponentModel.ISupportInitialize).EndInit()
        Me.Vertex3.ResumeLayout(False)
        Me.GroupBox3.ResumeLayout(False)
        CType(Me.Blue3, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.Green3, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.Red3, System.ComponentModel.ISupportInitialize).EndInit()
        Me.Vertex4.ResumeLayout(False)
        Me.GroupBox4.ResumeLayout(False)
        CType(Me.Blue4, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.Green4, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.Red4, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)

    End Sub

#End Region
    Public EndTest As Boolean = False
    Dim objDirect3DDevice As Device = Nothing
    Dim vertBuffer As VertexBuffer = Nothing
    Dim Textures(10) As Texture

    ' Flexible Vertex format for light test
    Public Const fvf_CUSTOMVERTEX As VertexFormats = VertexFormats.Transformed Or VertexFormats.Diffuse Or VertexFormats.Texture1

    Dim NUM_VERTS = 4

    Private Structure CUSTOMVERTEX
        Public X As Single
        Public Y As Single
        Public Z As Single
        Public rhw As Single
        Public color As Integer
        Public tu As Single
        Public tv As Single
    End Structure

    'Dispose of any used resources
    Public Sub DisposeD3D()
        On Error Resume Next  ' We are leaving, ignore any errors
        Dim i As Integer
        disposeVertices()
        For i = 0 To 9
            If Not (Textures(i) Is Nothing) Then Textures(i).Dispose()
            Textures(i) = Nothing
        Next

        If Not (objDirect3DDevice Is Nothing) Then objDirect3DDevice.Dispose()
        objDirect3DDevice = Nothing
    End Sub

    Public Sub disposeVertices()
        If Not (vertBuffer Is Nothing) Then vertBuffer.Dispose()
        vertBuffer = Nothing
    End Sub


    Public Function InitD3D(ByVal WinHandle As IntPtr) As Boolean
        Dim DispMode As DisplayMode
        Dim objDirect3Dpp As PresentParameters

        DispMode = Manager.Adapters(Manager.Adapters.Default.Adapter).CurrentDisplayMode
        ' Define the presentation parameters
        objDirect3Dpp = New PresentParameters
        objDirect3Dpp.Windowed = True
        objDirect3Dpp.SwapEffect = SwapEffect.Discard
        objDirect3Dpp.BackBufferFormat = DispMode.Format
        ' Try to create the device
        Try
            objDirect3DDevice = New Device(Manager.Adapters.Default.Adapter, _
               DeviceType.Hardware, WinHandle, CreateFlags.SoftwareVertexProcessing, _
               objDirect3Dpp)
            ' Define which type of vertex will be used
            objDirect3DDevice.VertexFormat = fvf_CUSTOMVERTEX
            InitD3D = True
        Catch
            InitD3D = False
        End Try
        ' Dispose the used objects 
        DispMode = Nothing
        objDirect3Dpp = Nothing
    End Function

    Public Function CreateTextures() As Boolean
        Dim verts As CustomVertex()
        Dim i As Integer

        Try
            For i = 1 To 10
                Textures(i - 1) = TextureLoader.FromFile(objDirect3DDevice, Application.StartupPath + "\Walk" & CStr(i) & ".bmp")
            Next
            If Not CreateVertices() Then
                Return False
            Else
                Return True
            End If
        Catch
            Return False
        End Try

    End Function

    Public Function CreateVertices() As Boolean
        Dim verts As CustomVertex()
        Try
            ' If the vertex buffer was previously created, dispose them
            disposeVertices()

            vertBuffer = New VertexBuffer(GetType(CustomVertex), NUM_VERTS, objDirect3DDevice, Usage.WriteOnly, fvf_CUSTOMVERTEX, Pool.Default)
            verts = vertBuffer.Lock(0, 0)
            SquareVertices(verts)
            vertBuffer.Unlock()
            Return True
        Catch
            Return False
        End Try
    End Function

    Private Sub SquareVertices(ByVal vertices As CUSTOMVERTEX())
        ' Create a square, composed of 2 triangles
        vertices(0) = CreateFlexVertex(60, 60, 0, 1, Color.FromArgb(Red1.Value, Green1.Value, Blue1.Value), 0, 0)
        vertices(1) = CreateFlexVertex(240, 60, 0, 1, Color.FromArgb(Red2.Value, Green2.Value, BLue2.Value), 1, 0)
        vertices(2) = CreateFlexVertex(60, 240, 0, 1, Color.FromArgb(Red3.Value, Green3.Value, Blue3.Value), 0, 1)
        vertices(3) = CreateFlexVertex(240, 240, 0, 1, Color.FromArgb(Red4.Value, Green4.Value, Blue4.Value), 1, 1)
    End Sub


    Private Function CreateFlexVertex(ByRef X As Single, ByRef Y As Single, ByRef Z As Single, ByRef rhw As Single, ByRef Color As Color, ByRef tu As Single, ByRef tv As Single) As CUSTOMVERTEX
        CreateFlexVertex.X = X
        CreateFlexVertex.Y = Y
        CreateFlexVertex.Z = Z
        CreateFlexVertex.rhw = rhw
        CreateFlexVertex.color = Color.ToArgb
        CreateFlexVertex.tu = tu
        CreateFlexVertex.tv = tv
    End Function


    Public Sub Render()
        Static x As Integer = 0

        If (objDirect3DDevice Is Nothing) Then Return

        objDirect3DDevice.Clear(ClearFlags.Target, Color.FromArgb(0, 0, 255).ToArgb, 1.0F, 0)
        objDirect3DDevice.BeginScene()
        ' Show one texture a time, in order to create the illusion of a walking guy
        objDirect3DDevice.SetTexture(0, Textures(x))
        x += 1
        If x = 10 Then x = 0

        objDirect3DDevice.SetStreamSource(0, vertBuffer, 0)
        objDirect3DDevice.DrawPrimitives(PrimitiveType.TriangleStrip, 0, NUM_VERTS - 2)
        objDirect3DDevice.EndScene()

        Try
            objDirect3DDevice.Present()
        Catch
        End Try
    End Sub


    Private Sub MatrixControl_KeyDown(ByVal sender As Object, ByVal e As System.Windows.Forms.KeyEventArgs) Handles MyBase.KeyDown
        If e.KeyCode = Keys.Escape Then
            EndTest = True
        End If
    End Sub


    Private Sub LightControl_Closing(ByVal sender As Object, ByVal e As System.ComponentModel.CancelEventArgs) Handles MyBase.Closing
        DisposeD3D()
        EndTest = True
    End Sub

    Private Sub Color_TextChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles Red1.TextChanged, Red2.TextChanged, Red3.TextChanged, Red4.TextChanged, Blue1.TextChanged, BLue2.TextChanged, Blue3.TextChanged, Blue4.TextChanged, Green1.TextChanged, Green2.TextChanged, Green3.TextChanged, Green4.TextChanged
        CreateVertices()
    End Sub
End Class



