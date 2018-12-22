//-----------------------------------------------------------------------------
// File: D3DUtil.cs
//
// Desc: Shortcut functions for using DX objects
//
// Copyright (c) Microsoft Corporation. All rights reserved
//-----------------------------------------------------------------------------
using System;
using System.Windows.Forms;
using System.Drawing;
using Microsoft.DirectX;
using Microsoft.DirectX.Direct3D;
using Direct3D = Microsoft.DirectX.Direct3D;




/// <summary>
/// Various helper functions for graphics samples
/// </summary>
public class GraphicsUtility
{
    /// <summary>
    /// Private Constructor 
    /// </summary>
    private GraphicsUtility() 
    { 
    }



    /// <summary>
    /// Initializes a Material structure, setting the diffuse and ambient
    /// colors. It does not set emissive or specular colors.
    /// </summary>
    /// <param name="c">The ambient and diffuse color</param>
    /// <returns>A defined material</returns>
	public static Direct3D.Material InitMaterial(System.Drawing.Color c)
	{
		Material mtrl = new Material();
		mtrl.Ambient = mtrl.Diffuse = c;
		return mtrl;
	}




    /// <summary>
    /// Initializes a light, setting the light position. The
    /// diffuse color is set to white; specular and ambient are left as black.
    /// </summary>
    /// <param name="light">Which light to initialize</param>
    /// <param name="ltType">The type</param>
	public static void InitLight(Light light, LightType ltType, float x, float y, float z)
	{
		light.Type        = ltType;
		light.Diffuse = System.Drawing.Color.White;
		light.Position = new Vector3(x,y,z);
		light.Direction = Vector3.Normalize(light.Position);
		light.Range        = 1000.0f;
	}




    /// <summary>
    /// Helper function to create a texture. It checks the root path first,
    /// then tries the DXSDK media path (as specified in the system registry).
    /// </summary>
	public static Texture CreateTexture(Device device, string textureFilename, Format format)
	{
		// Get the path to the texture
		string path = DXUtil.FindMediaFile(null, textureFilename);

		// Create the texture using D3DX
		return TextureLoader.FromFile(device, path, D3DX.Default, D3DX.Default, D3DX.Default, 0, format, 
			Pool.Managed, Filter.Triangle|Filter.Mirror, 
			Filter.Triangle|Filter.Mirror, 0);
	}




    /// <summary>
    /// Helper function to create a texture. It checks the root path first,
    /// then tries the DXSDK media path (as specified in the system registry).
    /// </summary>
    public static Texture CreateTexture(Device device, string textureFilename)
	{
		return GraphicsUtility.CreateTexture(device, textureFilename, Format.Unknown);
	}





    /// <summary>
    /// Returns a view matrix for rendering to a face of a cubemap.
    /// </summary>
	public static Matrix GetCubeMapViewMatrix(CubeMapFace face)
	{
		Vector3 vEyePt = new Vector3(0.0f, 0.0f, 0.0f);
		Vector3 vLookDir = new Vector3();
		Vector3 vUpDir = new Vector3();

		switch (face)
		{
			case CubeMapFace.PositiveX:
				vLookDir = new Vector3(1.0f, 0.0f, 0.0f);
				vUpDir   = new Vector3(0.0f, 1.0f, 0.0f);
				break;
			case CubeMapFace.NegativeX:
				vLookDir = new Vector3(-1.0f, 0.0f, 0.0f);
				vUpDir   = new Vector3(0.0f, 1.0f, 0.0f);
				break;
			case CubeMapFace.PositiveY:
				vLookDir = new Vector3(0.0f, 1.0f, 0.0f);
				vUpDir   = new Vector3(0.0f, 0.0f,-1.0f);
				break;
			case CubeMapFace.NegativeY:
				vLookDir = new Vector3(0.0f,-1.0f, 0.0f);
				vUpDir   = new Vector3(0.0f, 0.0f, 1.0f);
				break;
			case CubeMapFace.PositiveZ:
				vLookDir = new Vector3(0.0f, 0.0f, 1.0f);
				vUpDir   = new Vector3(0.0f, 1.0f, 0.0f);
				break;
			case CubeMapFace.NegativeZ:
				vLookDir = new Vector3(0.0f, 0.0f,-1.0f);
				vUpDir   = new Vector3(0.0f, 1.0f, 0.0f);
				break;
		}

		// Set the view transform for this cubemap surface
		Matrix matView = Matrix.LookAtLH(vEyePt, vLookDir, vUpDir);
		return matView;
	}




    /// <summary>
    /// Returns a quaternion for the rotation implied by the window's cursor position
    /// </summary>
	public static Quaternion GetRotationFromCursor(System.Windows.Forms.Form control, float fTrackBallRadius)
	{
		System.Drawing.Point pt = System.Windows.Forms.Cursor.Position;
		System.Drawing.Rectangle rc = control.ClientRectangle;
		pt = control.PointToClient(pt);
		float xpos = (((2.0f * pt.X) / (rc.Right-rc.Left)) - 1);
		float ypos = (((2.0f * pt.Y) / (rc.Bottom-rc.Top)) - 1);
		float sz;

		if (xpos == 0.0f && ypos == 0.0f)
			return new Quaternion(0.0f, 0.0f, 0.0f, 1.0f);

		float d2 = (float)Math.Sqrt(xpos*xpos + ypos*ypos);

		if (d2 < fTrackBallRadius * 0.70710678118654752440) // Inside sphere
			sz = (float)Math.Sqrt(fTrackBallRadius*fTrackBallRadius - d2*d2);
		else                                                 // On hyperbola
			sz = (fTrackBallRadius*fTrackBallRadius) / (2.0f*d2);

		// Get two points on trackball's sphere
		Vector3 p1 = new Vector3(xpos, ypos, sz);
		Vector3 p2 = new Vector3(0.0f, 0.0f, fTrackBallRadius);

		// Get axis of rotation, which is cross product of p1 and p2
		Vector3 axis = Vector3.Cross(p1,p2);

		// Calculate angle for the rotation about that axis
		float t = Vector3.Length(Vector3.Subtract(p2,p1)) / (2.0f*fTrackBallRadius);
		if (t > +1.0f) t = +1.0f;
		if (t < -1.0f) t = -1.0f;
		float fAngle = (float)(2.0f * Math.Asin(t));

		// Convert axis to quaternion
		return Quaternion.RotationAxis(axis, fAngle);
	}




    /// <summary>
    /// Returns a quaternion for the rotation implied by the window's cursor position
    /// </summary>
	public static Quaternion GetRotationFromCursor(System.Windows.Forms.Form control)
	{
		return GetRotationFromCursor(control, 1.0f);
	}




    /// <summary>
    /// Axis to axis quaternion double angle (no normalization)
    /// Takes two points on unit sphere an angle THETA apart, returns
    /// quaternion that represents a rotation around cross product by 2*THETA.
    /// </summary>
	public static Quaternion D3DXQuaternionUnitAxisToUnitAxis2(Vector3 fromVector, Vector3 toVector)
	{
		Vector3 axis = Vector3.Cross(fromVector, toVector);    // proportional to sin(theta)
		return new Quaternion(axis.X, axis.Y, axis.Z, Vector3.Dot(fromVector, toVector));
	}




    /// <summary>
    /// Axis to axis quaternion 
    /// Takes two points on unit sphere an angle THETA apart, returns
    /// quaternion that represents a rotation around cross product by theta.
    /// </summary>
	public static Quaternion D3DXQuaternionAxisToAxis(Vector3 fromVector, Vector3 toVector)
	{
		Vector3 vA = Vector3.Normalize(fromVector), vB = Vector3.Normalize(toVector);
		Vector3 vHalf = Vector3.Add(vA,vB);
		vHalf = Vector3.Normalize(vHalf);
		return GraphicsUtility.D3DXQuaternionUnitAxisToUnitAxis2(vA, vHalf);
	}



    
    /// <summary>
    /// Gets the number of ColorChanelBits from a format
    /// </summary>
	static public int GetColorChannelBits(Format format)
	{
		switch (format)
		{
			case Format.R8G8B8:
				return 8;
			case Format.A8R8G8B8:
				return 8;
			case Format.X8R8G8B8:
				return 8;
			case Format.R5G6B5:
				return 5;
			case Format.X1R5G5B5:
				return 5;
			case Format.A1R5G5B5:
				return 5;
			case Format.A4R4G4B4:
				return 4;
			case Format.R3G3B2:
				return 2;
			case Format.A8R3G3B2:
				return 2;
			case Format.X4R4G4B4:
				return 4;
			case Format.A2B10G10R10:
				return 10;
			case Format.A2R10G10B10:
				return 10;
			default:
				return 0;
		}
	}




    /// <summary>
    /// Gets the number of alpha channel bits 
    /// </summary>
	static public int GetAlphaChannelBits(Format format)
	{
		switch (format)
		{
			case Format.R8G8B8:
				return 0;
			case Format.A8R8G8B8:
				return 8;
			case Format.X8R8G8B8:
				return 0;
			case Format.R5G6B5:
				return 0;
			case Format.X1R5G5B5:
				return 0;
			case Format.A1R5G5B5:
				return 1;
			case Format.A4R4G4B4:
				return 4;
			case Format.R3G3B2:
				return 0;
			case Format.A8R3G3B2:
				return 8;
			case Format.X4R4G4B4:
				return 0;
			case Format.A2B10G10R10:
				return 2;
			case Format.A2R10G10B10:
				return 2;
			default:
				return 0;
		}
	}



    
    /// <summary>
    /// Gets the number of depth bits
    /// </summary>
	static public int GetDepthBits(DepthFormat format)
	{
		switch (format)
		{
			case DepthFormat.D16:
				return 16;
			case DepthFormat.D15S1:
				return 15;
			case DepthFormat.D24X8:
				return 24;
			case DepthFormat.D24S8:
				return 24;
			case DepthFormat.D24X4S4:
				return 24;
			case DepthFormat.D32:
				return 32;
			default:
				return 0;
		}
	}




    /// <summary>
    /// Gets the number of stencil bits
    /// </summary>
	static public int GetStencilBits(DepthFormat format)
	{
		switch (format)
		{
			case DepthFormat.D16:
				return 0;
			case DepthFormat.D15S1:
				return 1;
			case DepthFormat.D24X8:
				return 0;
			case DepthFormat.D24S8:
				return 8;
			case DepthFormat.D24X4S4:
				return 4;
			case DepthFormat.D32:
				return 0;
			default:
				return 0;
		}
	}




    /// <summary>
    /// Assembles and creates a file-based vertex shader
    /// </summary>
	public static VertexShader CreateVertexShader(Device device, string filename)
	{
		GraphicsStream code = null;
		string path = null;
	
		// Get the path to the vertex shader file
		path = DXUtil.FindMediaFile(null, filename);

		// Assemble the vertex shader file
		code = ShaderLoader.FromFile(path, null, 0);

		// Create the vertex shader
		return new VertexShader(device, code);
	}

}




/// <summary>
/// An arc ball class
/// </summary>
public class GraphicsArcBall
{
	private int internalWidth;   // ArcBall's window width
	private int internalHeight;  // ArcBall's window height
	private float internalradius;  // ArcBall's radius in screen coords
	private float internalradiusTranslation; // ArcBall's radius for translating the target

	private Quaternion internaldownQuat;               // Quaternion before button down
	private Quaternion internalnowQuat;                // Composite quaternion for current drag
	private Matrix internalrotationMatrix;         // Matrix for arcball's orientation
	private Matrix internalrotationDelta;    // Matrix for arcball's orientation
	private Matrix internaltranslationMatrix;      // Matrix for arcball's position
	private Matrix internaltranslationDelta; // Matrix for arcball's position
	private bool internaldragging;               // Whether user is dragging arcball
	private bool internaluseRightHanded;        // Whether to use RH coordinate system
	private int saveMouseX = 0;      // Saved mouse position
	private int saveMouseY = 0;
	private Vector3 internalvectorDown;         // Button down vector
	System.Windows.Forms.Control parent; // parent




    /// <summary>
    /// Constructor
    /// </summary>
	public GraphicsArcBall(System.Windows.Forms.Control p)
	{
		internaldownQuat = Quaternion.Identity;
		internalnowQuat = Quaternion.Identity;
		internalrotationMatrix = Matrix.Identity;
		internalrotationDelta = Matrix.Identity;
		internaltranslationMatrix = Matrix.Identity;
		internaltranslationDelta  = Matrix.Identity;
		internaldragging = false;
		internalradiusTranslation = 1.0f;
		internaluseRightHanded = false;

		parent = p;
		// Hook the events 
		p.MouseDown += new MouseEventHandler(this.OnContainerMouseDown);
		p.MouseUp += new MouseEventHandler(this.OnContainerMouseUp);
		p.MouseMove += new MouseEventHandler(this.OnContainerMouseMove);
	}




    /// <summary>
    /// Set the window dimensions
    /// </summary>
	public void SetWindow(int width, int height, float radius)
	{
		// Set ArcBall info
		internalWidth  = width;
		internalHeight = height;
		internalradius = radius;
	}




    /// <summary>
    /// Screen coords to a vector
    /// </summary>
	private Vector3 ScreenToVector(int xpos, int ypos)
	{
		// Scale to screen
		float x   = -(xpos - internalWidth/2)  / (internalradius*internalWidth/2);
		float y   =  (ypos - internalHeight/2) / (internalradius*internalHeight/2);

		if (internaluseRightHanded)
		{
			x = -x;
			y = -y;
		}

		float z   = 0.0f;
		float mag = x*x + y*y;

		if (mag > 1.0f)
		{
			float scale = 1.0f/(float)Math.Sqrt(mag);
			x *= scale;
			y *= scale;
		}
		else
			z = (float)Math.Sqrt(1.0f - mag);

		// Return vector
		return new Vector3(x, y, z);
	}




    /// <summary>
    /// Fired when the containers mouse button is down
    /// </summary>
	private void OnContainerMouseDown(object sender, System.Windows.Forms.MouseEventArgs e)
	{
		// Store off the position of the cursor when the button is pressed
		saveMouseX = e.X;
		saveMouseY = e.Y;

		if (e.Button == System.Windows.Forms.MouseButtons.Left)
		{
			// Start drag mode
			internaldragging = true;
			internalvectorDown = ScreenToVector(e.X, e.Y);
			internaldownQuat = internalnowQuat;
		}
	}




    /// <summary>
    /// Fired when the containers mouse button has been released
    /// </summary>
	private void OnContainerMouseUp(object sender, System.Windows.Forms.MouseEventArgs e)
	{
		if (e.Button == System.Windows.Forms.MouseButtons.Left)
		{
			// End drag mode
			internaldragging = false;
		}
	}




    /// <summary>
    /// Fired when the containers mouse is moving
    /// </summary>
	private void OnContainerMouseMove(object sender, System.Windows.Forms.MouseEventArgs e)
	{
		if (e.Button == System.Windows.Forms.MouseButtons.Left)
		{
			if (internaldragging)
			{
				// recompute nowQuat
				Vector3 vCur = ScreenToVector(e.X, e.Y);
				Quaternion qAxisToAxis = GraphicsUtility.D3DXQuaternionAxisToAxis(internalvectorDown, vCur);
				internalnowQuat = internaldownQuat;
				internalnowQuat = Quaternion.Multiply(internalnowQuat,qAxisToAxis);
				internalrotationDelta = Matrix.RotationQuaternion(qAxisToAxis);
			}
			else
				internalrotationDelta = Matrix.Identity;

			internalrotationMatrix = Matrix.RotationQuaternion(internalnowQuat);
			internaldragging = true;
		}

		if ((e.Button == System.Windows.Forms.MouseButtons.Right) || (e.Button == System.Windows.Forms.MouseButtons.Middle))
		{
			// Normalize based on size of window and bounding sphere radius
			float fDeltaX = (saveMouseX-e.X) * internalradiusTranslation / internalWidth;
			float fDeltaY = (saveMouseY-e.Y) * internalradiusTranslation / internalHeight;

			if (e.Button == System.Windows.Forms.MouseButtons.Right)
			{
				internaltranslationDelta = Matrix.Translation(-2 * fDeltaX, 2 * fDeltaY, 0.0f);
				internaltranslationMatrix = Matrix.Multiply(internaltranslationMatrix, internaltranslationDelta);
			}
			if (e.Button == System.Windows.Forms.MouseButtons.Middle)
			{
				internaltranslationDelta = Matrix.Translation(0.0f, 0.0f, 5 * fDeltaY);
				internaltranslationMatrix = Matrix.Multiply(internaltranslationMatrix, internaltranslationDelta);
			}

			// Store mouse coordinate
			saveMouseX = e.X;
			saveMouseY = e.Y;
		}
	}

	#region Various properties of the class
	public float Radius
	{
		set
		{ internalradiusTranslation = value; }
	}
	public bool RightHanded
	{
		get { return internaluseRightHanded; }
		set { internaluseRightHanded = value; }
	}
	public Matrix RotationMatrix
	{
		get { return internalrotationMatrix; }
	}
	public Matrix RotationDeltaMatrix
	{
		get { return internalrotationDelta; }
	}
	public Matrix TranslationMatrix
	{
		get { return internaltranslationMatrix; }
	}
	public Matrix TranslationDeltaMatrix
	{
		get { return internaltranslationDelta; }
	}
	public bool IsBeingDragged
	{
		get { return internaldragging; }
	}
	#endregion
}




/// <summary>
/// Handles our meshes
/// </summary>
public class GraphicsMesh : IDisposable
{
	private string fileName = null;
	private Mesh systemMemoryMesh = null; // SysMem mesh, lives through resize
	private Mesh localMemoryMesh = null; // Local mesh, rebuilt on resize
	private Direct3D.Material[] materials = null;
	private Texture[] textures = null;
	private bool isUsingMeshMaterials = true;
	private VertexBuffer systemMemoryVertexBuffer = null;
	private VertexBuffer localMemoryVertexBuffer = null;
	private IndexBuffer systemMemoryIndexBuffer = null;
	private IndexBuffer localMemoryIndexBuffer = null;




    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="filename">The initial filename</param>
	public GraphicsMesh(string filename)
	{
		fileName = filename;
	}
    public GraphicsMesh() : this ("CD3DFile_Mesh") {}




    /// <summary>
    /// The system memory mesh
    /// </summary>
	public Mesh SystemMesh
	{
		get { return systemMemoryMesh; }
	}




    /// <summary>
    /// The local memory mesh
    /// </summary>
	public Mesh LocalMesh
	{
		get { return localMemoryMesh; }
	}




    /// <summary>
    /// Should we use the mesh materials
    /// </summary>
	public bool IsUsingMaterials
	{
		set { isUsingMeshMaterials = value; }
	}




    /// <summary>
    /// Creates a new mesh
    /// </summary>
    /// <param name="device">The device used to create the mesh</param>
    /// <param name="filename">the file to load</param>
	public void Create(Device device, string filename)
	{
		string strPath = null;
		GraphicsStream adjacencyBuffer;
		ExtendedMaterial[] Mat;

		if (device != null)
		{
			device.DeviceLost += new System.EventHandler(this.InvalidateDeviceObjects);
			device.Disposing += new System.EventHandler(this.InvalidateDeviceObjects);
			device.DeviceReset += new System.EventHandler(this.RestoreDeviceObjects);
		}

		// Find the path for the file, and convert it to ANSI (for the D3DX API)
		strPath = DXUtil.FindMediaFile(null, filename);

		// Load the mesh
		systemMemoryMesh =  Mesh.FromFile(strPath, MeshFlags.SystemMemory, device, out adjacencyBuffer, out Mat);

		// Optimize the mesh for performance
		systemMemoryMesh.OptimizeInPlace(MeshFlags.OptimizeCompact | MeshFlags.OptimizeAttrSort | MeshFlags.OptimizeVertexCache, adjacencyBuffer);

		textures = new Texture[Mat.Length];
		materials = new Direct3D.Material[Mat.Length];

		for (int i=0; i<Mat.Length; i++)
		{
			materials[i] = Mat[i].Material3D;
			// Set the ambient color for the material (D3DX does not do this)
			materials[i].Ambient = materials[i].Diffuse;
     
			if (Mat[i].TextureFilename != null)
			{
				// Create the texture
				string texturefilename = DXUtil.FindMediaFile(null, Mat[i].TextureFilename);
				textures[i] = TextureLoader.FromFile(device, texturefilename);
			}
		}

		adjacencyBuffer.Close();
		adjacencyBuffer = null;
	}




    /// <summary>
    /// Set the flexible vertex format
    /// </summary>
	public void SetVertexFormat(Device device, VertexFormats format)
	{
		Mesh pTempSysMemMesh = null;
		Mesh pTempLocalMesh  = null;

		if (systemMemoryMesh != null)
		{
			pTempSysMemMesh = systemMemoryMesh.Clone(MeshFlags.SystemMemory, format, device);
		}
		if (localMemoryMesh != null)
		{
			try
			{
				pTempLocalMesh = localMemoryMesh.Clone(0, format, device);
			}
			catch (Exception e)
			{
				pTempSysMemMesh.Dispose();
				pTempSysMemMesh = null;
				throw e;
			}
		}

		if (systemMemoryMesh != null)
			systemMemoryMesh.Dispose();
		systemMemoryMesh = null;

		if (localMemoryMesh != null)
			localMemoryMesh.Dispose();
		localMemoryMesh = null;

		// Clean up any vertex/index buffers
		DisposeLocalBuffers(true, true);

		if (pTempSysMemMesh != null) systemMemoryMesh = pTempSysMemMesh;
		if (pTempLocalMesh != null) localMemoryMesh  = pTempLocalMesh;

		// Compute normals in case the meshes have them
		if (systemMemoryMesh != null)
			systemMemoryMesh.ComputeNormals();
		if (localMemoryMesh != null)
			localMemoryMesh.ComputeNormals();
	}




    /// <summary>
    /// Restore the device objects after the device was reset
    /// </summary>
	public void RestoreDeviceObjects(object sender, EventArgs e)
	{
		if (null == systemMemoryMesh)
			throw new ArgumentException();

		Device device = (Device)sender;
		// Make a local memory version of the mesh. Note: because we are passing in
		// no flags, the default behavior is to clone into local memory.
		localMemoryMesh = systemMemoryMesh.Clone(0, systemMemoryMesh.VertexFormat, device);
		// Clean up any vertex/index buffers
		DisposeLocalBuffers(false, true);

	}




    /// <summary>
    /// Invalidate our local mesh
    /// </summary>
	public void InvalidateDeviceObjects(object sender, EventArgs e)
	{
		if (localMemoryMesh != null)
			localMemoryMesh.Dispose();
		localMemoryMesh = null;
		// Clean up any vertex/index buffers
		DisposeLocalBuffers(false, true);
	}




    /// <summary>
    /// Get the vertex buffer assigned to the system mesh
    /// </summary>
	public VertexBuffer SystemVertexBuffer
	{
		get
		{
			if (systemMemoryVertexBuffer != null)
				return systemMemoryVertexBuffer;

			if (systemMemoryMesh == null)
				return null;
			
			systemMemoryVertexBuffer = systemMemoryMesh.VertexBuffer;
			return systemMemoryVertexBuffer;
		}
	}




    /// <summary>
    /// Get the vertex buffer assigned to the Local mesh
    /// </summary>
	public VertexBuffer LocalVertexBuffer
	{
		get
		{
			if (localMemoryVertexBuffer != null)
				return localMemoryVertexBuffer;

			if (localMemoryMesh == null)
				return null;
			
			localMemoryVertexBuffer = localMemoryMesh.VertexBuffer;
			return localMemoryVertexBuffer;
		}
	}




    /// <summary>
    /// Get the Index buffer assigned to the system mesh
    /// </summary>
	public IndexBuffer SystemIndexBuffer
	{
		get
		{
			if (systemMemoryIndexBuffer != null)
				return systemMemoryIndexBuffer;

			if (systemMemoryMesh == null)
				return null;

			systemMemoryIndexBuffer = systemMemoryMesh.IndexBuffer;
			return systemMemoryIndexBuffer;
		}
	}




    /// <summary>
    /// Get the Index buffer assigned to the Local mesh
    /// </summary>
	public IndexBuffer LocalIndexBuffer
	{
		get
		{
			if (localMemoryIndexBuffer != null)
				return localMemoryIndexBuffer;

			if (localMemoryMesh == null)
				return null;

			localMemoryIndexBuffer = localMemoryMesh.IndexBuffer;
			return localMemoryIndexBuffer;
		}
	}




    /// <summary>
    /// Clean up any resources
    /// </summary>
	public void Dispose()
	{
		if (textures != null)
		{
			for (int i = 0; i<textures.Length; i++)
			{
				if (textures[i] != null)
					textures[i].Dispose();
				textures[i] = null;
			}
			textures = null;
		}

		// Clean up any vertex/index buffers
		DisposeLocalBuffers(true, true);

		// Clean up any memory
		if (systemMemoryMesh != null)
			systemMemoryMesh.Dispose();
		systemMemoryMesh = null;

		// In case the finalizer hasn't been called yet.
		GC.SuppressFinalize(this);
	}




    /// <summary>
    /// Actually draw the mesh
    /// </summary>
    /// <param name="device">The device used to draw</param>
    /// <param name="canDrawOpaque">Can draw the opaque parts of the mesh</param>
    /// <param name="canDrawAlpha">Can draw the alpha parts of the mesh</param>
	public void Render(Device device, bool canDrawOpaque, bool canDrawAlpha)
	{
		if (null == localMemoryMesh)
			throw new ArgumentException();

		RenderStates rs = device.RenderState;
		// Frist, draw the subsets without alpha
		if (canDrawOpaque)
		{
			for (int i=0; i<materials.Length; i++)
			{
				if (isUsingMeshMaterials)
				{
					if (canDrawAlpha)
					{
						if (materials[i].Diffuse.A < 0xff)
							continue;
					}
					device.Material = materials[i];
					device.SetTexture(0, textures[i]);
				}
				localMemoryMesh.DrawSubset(i);
			}
		}

		// Then, draw the subsets with alpha
		if (canDrawAlpha && isUsingMeshMaterials)
		{
			// Enable alpha blending
			rs.AlphaBlendEnable = true;
			rs.SourceBlend = Blend.SourceAlpha;
			rs.DestinationBlend = Blend.InvSourceAlpha;
			for (int i=0; i<materials.Length; i++)
			{
				if (materials[i].Diffuse.A == 0xff)
					continue;

				// Set the material and texture
				device.Material = materials[i];
				device.SetTexture(0, textures[i]);
				localMemoryMesh.DrawSubset(i);
			}
			// Restore state
			rs.AlphaBlendEnable = false;
		}
	}




    /// <summary>
    /// Draw the mesh with opaque and alpha 
    /// </summary>
    public void Render(Device device)
	{
		Render(device, true, true);
	}




    /// <summary>
    /// Cleans up the local vertex buffers/index buffers
    /// </summary>
    /// <param name="systemBuffers"></param>
    /// <param name="localBuffers"></param>
	private void DisposeLocalBuffers(bool systemBuffers, bool localBuffers)
	{
		if (systemBuffers)
		{
			if (systemMemoryIndexBuffer != null)
				systemMemoryIndexBuffer.Dispose();
			systemMemoryIndexBuffer = null;

			if (systemMemoryVertexBuffer != null)
				systemMemoryVertexBuffer.Dispose();
			systemMemoryVertexBuffer = null;
		}
		if (localBuffers)
		{
			if (localMemoryIndexBuffer != null)
				localMemoryIndexBuffer.Dispose();
			localMemoryIndexBuffer = null;

			if (localMemoryVertexBuffer != null)
				localMemoryVertexBuffer.Dispose();
			localMemoryVertexBuffer = null;
		}
	}
}

