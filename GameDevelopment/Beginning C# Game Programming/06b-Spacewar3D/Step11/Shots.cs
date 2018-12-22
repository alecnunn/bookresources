using System;
using System.Drawing;
using Microsoft.DirectX;
using Microsoft.DirectX.Direct3D;
using Direct3D = Microsoft.DirectX.Direct3D;


	/// <summary>
	///  Handles the collision detection and returns the array of shots for sending to other players 
	/// </summary>
[Serializable]
public class Shots
{

	Photon[] shots;
	[NonSerialized]	
	float timeSinceShot = 0;
	[NonSerialized]	
	Rectangle screenBounds;	
		

	public Shots(Device device)
	{
		shots = new Photon[Constants.NumShots];
		for (int count = 0; count < Constants.NumShots; count++)
		{
			shots[count] = new Photon(device);
		}
	}

	public Rectangle ScreenBounds
	{
		get
		{
			return screenBounds;
		}
		set
		{
			screenBounds = value;
		}
	}
	public Photon[] GetShotArray()
	{
		lock(this)
		{
			return shots;
		}
	}

	public void SetShotArray(Photon[] shotArray)
	{
		lock(this)
		{
			this.shots = shotArray;
		}
	}

	public bool Shoot(Vector3 position, Vector3 launchVector)
	{
		if (timeSinceShot < Constants.ShotDeltaTime)
			return false;

		timeSinceShot = 0;


		foreach (Photon shot in shots)
		{
			if (!shot.Alive)
			{
				shot.SetShot(position, launchVector);
				return true;
			}
		}
		return false;
	}

	public void UpdatePosition(float elapsedTime)
	{
		timeSinceShot += elapsedTime;

		foreach (Photon shot in shots)
		{
			shot.UpdatePosition(elapsedTime);
		}
	}

	public void Clear()
	{
		foreach (Photon shot in shots)
		{
			shot.Alive = false;
		}
	}
	public bool TestShots(Ship ship)
	{
		foreach (Photon shot in shots)
		{
			if (shot.Alive)
			{
				float distance = Vector3.Length(shot.Location - ship.Position.Location);
				if (distance < Constants.ShotCollisionLimit)
				{
					shot.Alive = false;
					return true;
				}
			}
		}
		return false;
	}

	public void Render()
	{
		foreach (Photon shot in shots)
		{
			shot.Render();
		}
	}

	public void RestoreDeviceObjects()
	{
		foreach (Photon shot in shots)
		{
			shot.RestoreDeviceObjects();
		}
	}
}