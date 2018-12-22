using System;
using Microsoft.DirectX;

[Serializable]
public struct PlayerUpdate {
	public Vector3 Location;
	public Matrix WorldMatrix;
	public int State;
	public float WaitCount;
	public int DeathCount;
	public int Sounds;
	public int Score;
}