Imports Microsoft.DirectX.Direct3D
Imports Microsoft.DirectX

Public Class FullScreenTest
    Inherits System.Windows.Forms.Form
    Public EndTest As Boolean = False
    Dim NUM_VERTS = 4
    Dim objDirect3DDevice As Device = Nothing
    Dim vertBuffer As VertexBuffer = Nothing
    Dim Textures(10) As Texture

    ' Simple textured vertices constant and structure
    Public Const fvf_CUSTOMVERTEX As VertexFormats = VertexFormats.Transformed Or VertexFormats.Texture1

    Private Structure CUSTOMVERTEX
        Public X As Single
        Public Y As Single
        Public Z As Single
        Public rhw As Single
        Public tu As Single
        Public tv As Single
    End Structure


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
    <System.Diagnostics.DebuggerStepThrough()> Private Sub InitializeComponent()
        '
        'FullScreenTest
        '
        Me.AutoScaleBaseSize = New System.Drawing.Size(6, 15)
        Me.ClientSize = New System.Drawing.Size(1344, 856)
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None
        Me.KeyPreview = True
        Me.Name = "FullScreenTest"
        Me.Text = "TestFullScreen"
        Me.TopMost = True
        Me.WindowState = System.Windows.Forms.FormWindowState.Maximized

    End Sub

#End Region

    Private Sub TestWindow_KeyDown(ByVal sender As Object, ByVal e As System.Windows.Forms.KeyEventArgs) Handles MyBase.KeyDown
        If e.KeyCode = Keys.Escape Then
            EndTest = True
        End If
    End Sub
    Private Sub FullScreenTest_MouseDown(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs) Handles MyBase.MouseDown
        EndTest = True
    End Sub


    Public Sub DisposeD3D()
        On Error Resume Next  ' We are leaving, ignore any errors
        Dim i As Integer


        For i = 0 To 9
            If Not (Textures(i) Is Nothing) Then Textures(i).Dispose()
            Textures(i) = Nothing
        Next

        If Not (vertBuffer Is Nothing) Then vertBuffer.Dispose()
        vertBuffer = Nothing

        If Not (objDirect3DDevice Is Nothing) Then objDirect3DDevice.Dispose()
        objDirect3DDevice = Nothing
    End Sub

    Public Function InitD3D(ByVal WinHandle As IntPtr) As Boolean
        Dim DispMode As DisplayMode
        Dim objDirect3Dpp As PresentParameters

        DispMode = Manager.Adapters(Manager.Adapters.Default.Adapter).CurrentDisplayMode
        ' Define the presentation parameters
        objDirect3Dpp = New PresentParameters
        objDirect3Dpp.BackBufferFormat = DispMode.Format
        objDirect3Dpp.BackBufferWidth = DispMode.Width
        objDirect3Dpp.BackBufferHeight = DispMode.Height
        objDirect3Dpp.SwapEffect = SwapEffect.Discard
        ' Try to create the device
        Try
            objDirect3DDevice = New Device(Manager.Adapters.Default.Adapter, _
                           DeviceType.Hardware, WinHandle, CreateFlags.SoftwareVertexProcessing, _
                           objDirect3Dpp)
            objDirect3DDevice.VertexFormat = fvf_CUSTOMVERTEX
            InitD3D = True
        Catch
            InitD3D = False
        End Try
        ' Dispose the used objects 
    End Function

    Public Function CreateTextures() As Boolean
        Dim verts As CustomVertex()
        Dim i As Integer

        Try
            For i = 1 To 10
                Textures(i - 1) = TextureLoader.FromFile(objDirect3DDevice, Application.StartupPath + "\Walk" & CStr(i) & ".bmp")
            Next
            vertBuffer = New VertexBuffer(GetType(CustomVertex), NUM_VERTS, objDirect3DDevice, _
                               Usage.WriteOnly, fvf_CUSTOMVERTEX, Pool.Default)
            verts = vertBuffer.Lock(0, 0)
            SquareVertices(verts)
            vertBuffer.Unlock()
            Return True
        Catch
            Return False
        End Try
    End Function

    Private Sub SquareVertices(ByVal vertices As CUSTOMVERTEX())
        Dim mode As DisplayMode = Manager.Adapters(Manager.Adapters.Default.Adapter).CurrentDisplayMode


        ' Create a square, composed of 2 triangles, taking all the screen
        vertices(0) = CreateFlexVertex(0, 0, 0, 1, 0, 0)
        vertices(1) = CreateFlexVertex(mode.Width, 0, 0, 1, 1, 0)
        vertices(2) = CreateFlexVertex(0, mode.Height, 0, 1, 0, 1)
        vertices(3) = CreateFlexVertex(mode.Width, mode.Height, 0, 1, 1, 1)
    End Sub

    Private Function CreateFlexVertex(ByRef X As Single, ByRef Y As Single, ByRef Z As Single, ByRef rhw As Single, ByRef tu As Single, ByRef tv As Single) As CUSTOMVERTEX
        CreateFlexVertex.X = X
        CreateFlexVertex.Y = Y
        CreateFlexVertex.Z = Z
        CreateFlexVertex.rhw = rhw
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

    Private Sub FullScreenTest_Closing(ByVal sender As Object, ByVal e As System.ComponentModel.CancelEventArgs) Handles MyBase.Closing
        EndTest = True
        DisposeD3D()
    End Sub

End Class


