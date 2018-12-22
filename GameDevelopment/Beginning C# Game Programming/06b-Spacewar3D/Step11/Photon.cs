using System;
using System.Drawing;
using System.Windows.Forms;
using Microsoft.DirectX;
using Microsoft.DirectX.Direct3D;
using Direct3D=Microsoft.DirectX.Direct3D;


	/// <summary>
	/// Summary description for Photon.
	/// </summary>
public class Photon : IDisposable
{

	private const float maxLife = 5;
	public Vector3 Location 
	{
		get
		{
			
			return photonMesh.Position.Location;
		} set 
		  {
			  photonMesh.Position.Location = value;
		  }
	}

	private Vector3 launchVector;
	private PositionedMesh photonMesh;
	private Device device;
	private float age;
	public float Age { get { return age; } set { age = value; } }
	private bool alive = false;
	public bool Alive { get { return alive; } set { alive = value; }  }

	private bool disposing = false;

	public Photon(Device device)
	{
		this.device = device;
		
		photonMesh = new PositionedMesh(device, "photon.x");

		alive = false;
		age = 0;
	}

	public void SetShot(Vector3 position, Vector3 launchVector)
	{
		age = 0;
		alive = true;
		photonMesh.Position.Location = position;
		this.launchVector = launchVector;
	}

	public void UpdatePosition(float elapsedtime)
	{
		if (disposing)
			return;
		age += elapsedtime;
		if (age >= Constants.ShotLifetime )
		{
			this.alive = false;
		}
		else
		{
			photonMesh.Position.RotateRel(0, elapsedtime*10,0);
			Vector3 moveVector = elapsedtime * Constants.ShotVelocity * launchVector;
			photonMesh.Position.MoveRel(moveVector.X, moveVector.Y, moveVector.Z);
		}

	}

	public void Render()
	{
		if (disposing | !alive)
			return;

		device.RenderState.Lighting = false;

		device.RenderState.AlphaBlendEnable = true;
		device.RenderState.AlphaBlendOperation = BlendOperation.Add;
		device.RenderState.AlphaSourceBlend = Blend.One;
		device.RenderState.DestinationBlend = Blend.One;
		device.Transform.World = photonMesh.Position.WorldMatrix;
		photonMesh.Render();
		device.RenderState.AlphaBlendEnable = false;
		device.RenderState.Lighting = true;
	}

	public void RestoreDeviceObjects()
	{

	}
	#region IDisposable Members

	public void Dispose()
	{
		disposing = true;

		if (photonMesh != null)
			photonMesh.Dispose();

	}

	#endregion
}