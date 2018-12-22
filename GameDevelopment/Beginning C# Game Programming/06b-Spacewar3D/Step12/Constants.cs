using System;
using Microsoft.DirectX.Direct3D;
using Microsoft.DirectX;


	/// <summary>
	/// GamePlay constants.  You can tune the game by adjusting these values.
	/// </summary>
sealed public class Constants {
	static public Random random = new Random((int) DateTime.Now.Ticks);

	public const float EngineThrust = 0.3f;
	public const float DyingCycle = 2;
	public const float DeadCycleWait = 2;
	public const float HyperChargeWait = 3;
	public const float HyperCycleWait = 2;
	public const float HyperSuccessFactor = 0.8f;		// initial success factor
	public const float HyperSuccessDegradation = 0.1f;	// degradation per use...
	public const float ShipCollisionLimit = 15f;
	public const float MaxVelocity = 125.0f;
	public const float ThrustPower = 1.001f;

	public const int ShotLifetime = 4;
	public const int NumShots = 12;
	public const float ShotVelocity = 350f;
	public const float ShotDeltaTime = 0.10f;
	public const int ShotInitialMove = 3;		// times to update shot so it shows up outside the ship...
	public const int ShotCollisionLimit = 10;


	public const float StatusMessageDisplayTime = 5.0f;



	public const long RemoteTickTimeout = 5;	// five seconds...

	private Constants() {
	}
}