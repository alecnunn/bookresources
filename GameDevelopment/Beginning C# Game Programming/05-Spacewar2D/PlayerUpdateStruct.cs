using System;
using Microsoft.DirectX;


public struct PlayerUpdate {
	public Vector2 ShipPosition;
	public int Outline;
	public Vector2 ShipVelocity;
	public int State;
	public int WaitCount;
	public int DeathCount;
	public int FlameIndex;
	public int Sounds;
	public int Score;
}