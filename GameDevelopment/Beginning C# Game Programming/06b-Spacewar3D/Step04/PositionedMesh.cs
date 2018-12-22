using System;
using System.Drawing;
using System.Collections;
using Microsoft.DirectX;
using Microsoft.DirectX.Direct3D;
using Direct3D=Microsoft.DirectX.Direct3D;


public class PositionedMesh : IDisposable {
	private Device device = null;
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
	private Sphere boundingSphere;

	private  WorldPosition worldPosition;
	public WorldPosition Position { 
		get { return worldPosition; } 
		set { worldPosition = value; } 
	}


	
	private float velocity;
	public float Velocity { 
		get { return velocity; } 
		set { velocity = value; } 
	}


	/// <summary>
	/// Constructor
	/// </summary>
	/// <param name="filename">The initial filename</param>
	public PositionedMesh(string filename) {
		fileName = filename;
	}
	public PositionedMesh() : this ("PositionedMesh") {}

	public PositionedMesh(Device device,  string filename) {
		Create(device, filename);
	}

	/// <summary>
	/// The system memory mesh
	/// </summary>
	public Mesh SystemMesh {
		get { return systemMemoryMesh; }
	}

	/// <summary>
	/// The local memory mesh
	/// </summary>
	public Mesh LocalMesh {
		get { return localMemoryMesh; }
	}

	/// <summary>
	/// Should we use the mesh materials
	/// </summary>
	public bool IsUsingMaterials {
		set { isUsingMeshMaterials = value; }
	}

	///<summary>
	///The bounding sphere
	///</summary>
	public Sphere BoundingSphere {
		get { return boundingSphere; }
	}

	/// <summary>
	/// Creates a new mesh
	/// </summary>
	/// <param name="device">The device used to create the mesh</param>
	/// <param name="filename">the file to load</param>
	public void Create(Device device, string filename) {
		worldPosition = new WorldPosition();

		GraphicsStream adjacencyBuffer;
		ExtendedMaterial[] Mat;

		this.device = device;
		if (device != null) {
			device.DeviceLost += new System.EventHandler(this.InvalidateDeviceObjects);
			device.Disposing += new System.EventHandler(this.InvalidateDeviceObjects);
			device.DeviceReset += new System.EventHandler(this.RestoreDeviceObjects);
		}
		filename = MediaUtilities.FindFile(filename);
		// Load the mesh
		systemMemoryMesh =  Mesh.FromFile(filename, MeshFlags.SystemMemory, device, out adjacencyBuffer, out Mat);

		Mesh tempMesh = null;
		string errorString;	
		tempMesh = Mesh.Clean(systemMemoryMesh, adjacencyBuffer, adjacencyBuffer, out errorString);
		systemMemoryMesh.Dispose();
		systemMemoryMesh = tempMesh;

		// Optimize the mesh for performance
		MeshFlags flags = MeshFlags.OptimizeCompact | MeshFlags.OptimizeAttrSort | MeshFlags.OptimizeVertexCache;
		systemMemoryMesh.OptimizeInPlace(flags, adjacencyBuffer);
		adjacencyBuffer.Close();
		adjacencyBuffer = null;
		// Setup bounding volumes
		VertexBuffer vb = systemMemoryMesh.VertexBuffer;
		GraphicsStream vertexData = vb.Lock(0, 0, LockFlags.ReadOnly);
		boundingSphere.Radius = Geometry.ComputeBoundingSphere(vertexData,systemMemoryMesh.NumberVertices,systemMemoryMesh.VertexFormat, out boundingSphere.CenterPoint);
		vb.Unlock();
		vb.Dispose();

		textures = new Texture[Mat.Length];
		materials = new Direct3D.Material[Mat.Length];

		for (int i=0; i<Mat.Length; i++) {
			materials[i] = Mat[i].Material3D;
			// Set the ambient color for the material (D3DX does not do this)
			materials[i].Ambient = materials[i].Diffuse;
     
			if (Mat[i].TextureFilename != null) {
				// Create the texture
				textures[i] = TextureLoader.FromFile(device, MediaUtilities.FindFile(Mat[i].TextureFilename));
			}
		}
		RestoreDeviceObjects(device, null);
	}

	/// <summary>
	/// Set the flexible vertex format
	/// </summary>
	public void SetVertexFormat(Device device, VertexFormats format) {
			
		Mesh pTempSysMemMesh = null;
		Mesh pTempLocalMesh  = null;

		if (systemMemoryMesh != null) {
			pTempSysMemMesh = systemMemoryMesh.Clone(MeshFlags.SystemMemory, format, device);
		}
		if (localMemoryMesh != null) {
			try {
				pTempLocalMesh = localMemoryMesh.Clone(0, format, device);
			}
			catch (Exception e) {
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
	public void RestoreDeviceObjects(object sender, EventArgs e) {
		if (null == systemMemoryMesh)
			throw new ArgumentException();

		Device device = (Device)sender;
		// Make a local memory version of the mesh.
		localMemoryMesh = systemMemoryMesh.Clone(MeshFlags.VbWriteOnly | MeshFlags.IbWriteOnly,
			systemMemoryMesh.VertexFormat, device);
		// Clean up any vertex/index buffers
		DisposeLocalBuffers(false, true);

	}

	/// <summary>
	/// Invalidate our local mesh
	/// </summary>
	public void InvalidateDeviceObjects(object sender, EventArgs e) {
		if (localMemoryMesh != null)
			localMemoryMesh.Dispose();
		localMemoryMesh = null;
		// Clean up any vertex/index buffers
		DisposeLocalBuffers(false, true);
	}

	/// <summary>
	/// Get the vertex buffer assigned to the system mesh
	/// </summary>
	public VertexBuffer SystemVertexBuffer {
		get {
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
	public VertexBuffer LocalVertexBuffer {
		get {
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
	public IndexBuffer SystemIndexBuffer {
		get {
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
	public IndexBuffer LocalIndexBuffer {
		get {
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
	public void Dispose() {
		if (textures != null) {
			for (int i = 0; i<textures.Length; i++) {
				if (textures[i] != null)
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
	public void Render(bool canDrawOpaque, bool canDrawAlpha) {
		if (null == localMemoryMesh)
			throw new ArgumentException();
		// Set the world transform
		device.Transform.World = worldPosition.WorldMatrix;
		RenderStates rs = device.RenderState;
		// Frist, draw the subsets without alpha
		if (canDrawOpaque) {
			for (int i=0; i<materials.Length; i++) {
				if (isUsingMeshMaterials) {
					if (canDrawAlpha) {
						if (materials[i].Diffuse.A < 0xff)
							continue;
					}
					device.Material = materials[i];
					if (textures[i] != null) {
						device.SetTexture(0, textures[i]);
					}
					else {
						device.SetTexture(0,null);
					}
				}
				localMemoryMesh.DrawSubset(i);
			}
		}

		// Then, draw the subsets with alpha
		if (canDrawAlpha && isUsingMeshMaterials) {
			// Enable alpha blending
			rs.AlphaBlendEnable = true;
			rs.SourceBlend = Blend.SourceAlpha;
			rs.DestinationBlend = Blend.InvSourceAlpha;
			for (int i=0; i<materials.Length; i++) {
				if (materials[i].Diffuse.A == 0xff)
					continue;

				// Set the material and texture
				device.Material = materials[i];
					
						
				if (textures[i] != null) {
					device.SetTexture(0, textures[i]);
				}
				else {
					device.SetTexture(0,null);
				}

				localMemoryMesh.DrawSubset(i);
			}
			// Restore state
			rs.AlphaBlendEnable = false;
		}
	}

	/// <summary>
	/// Draw the mesh with opaque and alpha 
	/// </summary>
	public void Render() {
		Render(true, true);
	}

	/// <summary>
	/// Cleans up the local vertex buffers/index buffers
	/// </summary>
	/// <param name="systemBuffers"></param>
	/// <param name="localBuffers"></param>
	private void DisposeLocalBuffers(bool systemBuffers, bool localBuffers) {
		if (systemBuffers) {
			if (systemMemoryIndexBuffer != null)
				systemMemoryIndexBuffer.Dispose();
			systemMemoryIndexBuffer = null;

			if (systemMemoryVertexBuffer != null)
				systemMemoryVertexBuffer.Dispose();
			systemMemoryVertexBuffer = null;
		}
		if (localBuffers) {
			if (localMemoryIndexBuffer != null)
				localMemoryIndexBuffer.Dispose();
			localMemoryIndexBuffer = null;

			if (localMemoryVertexBuffer != null)
				localMemoryVertexBuffer.Dispose();
			localMemoryVertexBuffer = null;
		}
	}
}
