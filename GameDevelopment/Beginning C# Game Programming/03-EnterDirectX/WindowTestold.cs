Imports Microsoft.DirectX.Direct3D
Imports Microsoft.DirectX
Imports System.Drawing

Public Class WindowTest
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
    <System.Diagnostics.DebuggerStepThrough()> Private Sub InitializeComponent()
        '
        'WindowTest
        '
        Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
        Me.ClientSize = New System.Drawing.Size(320, 286)
        Me.KeyPreview = True
        Me.Name = "WindowTest"
        Me.Text = "Test Windowed Mode"

    End Sub

#End Region

    Public EndTest As Boolean = False
    Dim objDirect3DDevice As Device = Nothing


    Dim NUM_VERTS = 4
    Dim vertBuffer As VertexBuffer = Nothing
    Dim Textures(10) As Texture

    ' Simple textured vertices constant and structure
    Public Const FVF_CUSTOMVERTEX As VertexFormats = VertexFormats.Transformed Or VertexFormats.Texture1

    Private Structure CUSTOMVERTEX
        Public X As Single
        Public Y As Single
        Public Z As Single
        Public rhw As Single
        Public tu As Single
        Public tv As Single
    End Structure


    Private Sub TestWindow_KeyDown(ByVal sender As Object, ByVal e As System.Windows.Forms.KeyEventArgs) Handles MyBase.KeyDown
        If e.KeyCode = Keys.Escape Then
            EndTest = True
        End If
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
        objDirect3Dpp.Windowed = True
        objDirect3Dpp.SwapEffect = SwapEffect.Discard
        objDirect3Dpp.BackBufferFormat = DispMode.Format
        ' Try to create the device
        Try
            objDirect3DDevice = New Device(Manager.Adapters.Default.Adapter, _
                           DeviceType.Hardware, WinHandle, CreateFlags.SoftwareVertexProcessing, _
                           objDirect3Dpp)
            objDirect3DDevice.VertexFormat = FVF_CUSTOMVERTEX
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
            ' Load the textures, named from "walk1.bmp" ro "walk10.bmp"
            For i = 1 To 10
                Textures(i - 1) = TextureLoader.FromFile(objDirect3DDevice, Application.StartupPath + "\Walk" & CStr(i) & ".bmp")
            Next

            ' Define the vertex buffer to hold our custom vertices
            vertBuffer = New VertexBuffer(GetType(CustomVertex), NUM_VERTS, objDirect3DDevice, Usage.WriteOnly, FVF_CUSTOMVERTEX, Pool.Default)
            ' Locks the memory, which will return the array to be filled
            verts = vertBuffer.Lock(0, 0)
            ' Defines the vertices
            SquareVertices(verts)
            ' Unlock the buffer, which will save our vertex information to the device
            vertBuffer.Unlock()

            Return True
        Catch
            Return False
        End Try
    End Function

    Private Sub SquareVertices(ByVal vertices As CUSTOMVERTEX())
        ' Create a square, composed of 2 triangles
        vertices(0) = CreateFlexVertex(60, 60, 0, 1, 0, 0)
        vertices(1) = CreateFlexVertex(240, 60, 0, 1, 1, 0)
        vertices(2) = CreateFlexVertex(60, 240, 0, 1, 0, 1)
        vertices(3) = CreateFlexVertex(240, 240, 0, 1, 1, 1)
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

        ' Clears the device with blue color
        objDirect3DDevice.Clear(ClearFlags.Target, Color.FromArgb(0, 0, 255).ToArgb, 1.0F, 0)
        objDirect3DDevice.BeginScene()

        ' Show one texture a time, in order to create the illusion of a walking guy
        objDirect3DDevice.SetTexture(0, Textures(x))
        x += 1
        If x = 10 Then x = 0
        ' Define which vertex buffer should be used
        objDirect3DDevice.SetStreamSource(0, vertBuffer, 0)
        objDirect3DDevice.VertexFormat = FVF_CUSTOMVERTEX
        ' Draw the vertices of the vertex buffer, rendering them as a
        ' triangle strip, using the given texture
        objDirect3DDevice.DrawPrimitives(PrimitiveType.TriangleStrip, 0, NUM_VERTS - 2)
        objDirect3DDevice.EndScene()

        ' Using an extra try-catch will prevent any errors if the device was disposed
        Try
            ' Present the rendered scene
            objDirect3DDevice.Present()
        Catch
        End Try

    End Sub


    Private Sub WindowTest_Closing(ByVal sender As Object, ByVal e As System.ComponentModel.CancelEventArgs) Handles MyBase.Closing
        EndTest = True
        DisposeD3D()
    End Sub
End Class


