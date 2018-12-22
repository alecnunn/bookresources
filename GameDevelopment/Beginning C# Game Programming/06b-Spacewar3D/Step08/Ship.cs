using System;
using System.Drawing;
using Microsoft.DirectX;
using Microsoft.DirectX.Direct3D;
using Direct3D=Microsoft.DirectX.Direct3D;


	/// <summary>
	/// This class handles the drawing and collision detection routines for our ship.
	/// </summary>
public class Ship {
	private Shots shots;
	private ShipState state = ShipState.Dead;		// current state
	public ShipState State { get { lock(this){return state;} } set { lock(this){ state = value; }}}
	
	private Device device;
	private PositionedMesh shipMesh;

	private GameClass game;
	
	private float velocity;

	private string hostName;

	private float waitCount = 6.0f;					// wait count before state transition.
	public float WaitCount { get { lock(this){return waitCount; }} set { lock(this){ waitCount = value; }}}
	private int score;								// our score
		
	private int deathCount;							// # of times we've died...
	private float hyperSuccess = Constants.HyperSuccessFactor;		// chance of a hyper being successful...

	private Sounds sounds;
	private Vector3 startingPosition;

	public Shots ShotHandler {
		get {
			lock(this) {
				return shots;
			}
		}
	}
	public int DeathCount {
		get {
			lock(this) {
				return deathCount;
			}
		}
		set {
			lock(this) {
				deathCount = value;
			}
		}
	}

	public int Score {
		get {
			lock(this) {
				return score;
			}
		}
		set {
			lock(this) {
				score = value;
			}
		}
	}

	public string HostName {
		get {
			lock(this) {
				return hostName;
			}
		}
		set {
			lock(this) {
				hostName = value;
			}
		}
	}

	public Vector3 Location {
		get {
			lock (this) {
				return shipMesh.Position.Location;
			}
		}
		set {
			lock (this) {
				shipMesh.Position.Location = value;
			}
		}
	}

	public Vector3 Rotation {
		get {
			lock(this) {
				return shipMesh.Position.Rotation;
			}
		}
		set {
			lock(this) {
				shipMesh.Position.Rotation = value;
			}
		}
	}

	public WorldPosition Position {
		get {
			lock (this) {
				return shipMesh.Position;
			}
		}
		set {
			lock(this) {
				shipMesh.Position=value;
			}
		}
	}

	public float Velocity {
		get {
			lock (this) {
				return velocity;
			}
		}
		set {
			lock(this) {
				velocity = value;
			}
		}
	}



	public Sounds Sounds {
		get {
			lock(this) {
				return sounds;
			}
		}
		set {
			lock(this) {
				sounds = value;
			}
		}
	}

	public Ship(Device device, GameClass gameClass, HullColors hullColor) {

		this.device = device;
		this.game = gameClass;
		shots = new Shots(device);
		if (hullColor == HullColors.White) {
			shipMesh = new PositionedMesh(device, "WhiteShip.x");
			startingPosition = new Vector3(10, 0, 10);
		}
		else {
			shipMesh = new PositionedMesh(device, "RedShip.x");
			startingPosition = new Vector3(-50, 0, -150);
		}

		SetRandomPosition(true);
		
	}


	public void Shoot() {
		if (this.state != ShipState.Normal) {
			return;
		}

		// find out the location of the front of the ship...

		bool shot = shots.Shoot(this.Position.CombinedLocation, this.Position.Direction);
		if (shot) {
			sounds |= Sounds.ShipFire;
		}
	}

	public void SetThrust(bool thrust, float elapsedTime) {

		if (thrust && (state == ShipState.Normal)) {

			if (velocity < Constants.MaxVelocity * 0.1f)
				velocity = Constants.MaxVelocity * 0.1f;
			if (velocity < Constants.MaxVelocity)
				velocity  += velocity * Constants.ThrustPower;
			if (velocity > Constants.MaxVelocity)
				velocity = Constants.MaxVelocity;
			sounds |= Sounds.ShipThrust;
		}

		else {
			velocity  -= velocity * elapsedTime * 2;
			if (velocity < 0.00005f)
				velocity = 0.0f;

			if ((sounds & Sounds.ShipThrust) != 0)
				sounds ^= Sounds.ShipThrust;
		}
	}


	public void EnterHyper() {
		if (state != ShipState.Normal)
			return;

		SetState(ShipState.HyperCharge);
	}

	public void SetRandomPosition(bool setOriginal) {
		if (setOriginal) {
			shipMesh.Position.Move(startingPosition);
		}
		else {
			shipMesh.Position.RotateRel(0,Constants.random.Next(100),0);
			shipMesh.Position.Move(
				Constants.random.Next(1000)-500,
				Constants.random.Next(1000)-500,
				Constants.random.Next(1000)-500);
		}
	}

	public void SetState(ShipState newState) {
		if (newState == state)
			return;

		lock(this) {

			switch (newState) {
				case ShipState.Dying:
					deathCount++;
					waitCount = Constants.DyingCycle;
					sounds |= Sounds.ShipExplode;
					break;

				case ShipState.Dead:
					waitCount = Constants.DeadCycleWait;
					shipMesh.Position.Move(this.startingPosition);

					break;

				case ShipState.Normal:
					if (state == ShipState.Hyper) {
						SetRandomPosition(false);
					}
					else {
						shipMesh.Position.Move(startingPosition);
						velocity = 0.0f;
					}
					hyperSuccess = Constants.HyperSuccessFactor;
					shots.Clear();
					sounds |= Sounds.ShipAppear;
					break;

				case ShipState.HyperCharge:
					sounds |= Sounds.ShipHyper;
					waitCount = Constants.HyperChargeWait;
					break;

				case ShipState.Hyper:
					waitCount = Constants.HyperCycleWait;
					break;
			}
			state = newState;
		}
	}

		
	public void YawPitchRoll(float yawAmount, float pitchAmount, float elapsedTime) {
		float absYaw = Math.Abs(yawAmount);
		if ( (0 <= absYaw) & (absYaw <= 30)) {
			yawAmount = 0;
		}
		else if ((31 <= absYaw) & (absYaw <= 150)) {
			yawAmount *= 1;
		}
		else {
			yawAmount *= 1.2f;
		}
		
		const float sensitivity = 0.002f;

		float yaw = yawAmount * elapsedTime * sensitivity;

		float absPitch = Math.Abs(pitchAmount);

		if ( (0 <= absPitch) & (absPitch <= 30)) {
			pitchAmount = 0;
		}
		else if ((31 <= absPitch) & (absPitch <= 150)) {
			pitchAmount *= 1;
		}
		else {
			pitchAmount *= 1.2f;
		}

		float pitch = pitchAmount * elapsedTime * sensitivity;
		float roll = 0;


		this.Position.YawPitchRoll(yaw, pitch, roll);
	
	}

	public void UpdatePosition(float elapsedTime) {
		shots.UpdatePosition(elapsedTime);

		if (state == ShipState.Dying) {
			return;
		}


		if ((state != ShipState.Normal) &&
			(state != ShipState.HyperCharge))
			return;

		float xAdd = (elapsedTime * velocity * Position.Direction.X);
		float yAdd = (elapsedTime * velocity * Position.Direction.Y);
		float zAdd = (elapsedTime * velocity * Position.Direction.Z);

		this.Position.MoveRel(xAdd, yAdd, zAdd);

	}

	public void UpdateState(float elapsedTime) {
		if (state == ShipState.Dying) {
			this.waitCount -= elapsedTime;
			if (waitCount <= 0) {
				SetState(ShipState.Dead);
			}
		}

		if (state == ShipState.Dead) {
			this.waitCount -= elapsedTime;
			if (waitCount <= 0) {
				SetState(ShipState.Normal);
			}
		}

		if (state == ShipState.HyperCharge) {
			this.waitCount -= elapsedTime;
			shipMesh.Position.RotateRel(0,-elapsedTime * Constants.HyperCycleWait *6.0f, 0);

			if (waitCount <= 0) {
				// if we didn't make it, we're dying...
				double randValue = Constants.random.NextDouble();
				if (hyperSuccess < randValue) {
					SetState(ShipState.Dying);
					game.SendPoint();
				}
				else {
					SetState(ShipState.Hyper);
				}
			}
		}
		if (state == ShipState.Hyper) {
			this.waitCount -= elapsedTime;
			shipMesh.Position.RotateRel(0, -elapsedTime * this.waitCount * 6.0f, 0);
			if (waitCount <= 0) {
				hyperSuccess -= Constants.HyperSuccessDegradation;
				float save = this.hyperSuccess;
				SetState(ShipState.Normal);
				hyperSuccess = save;
			}
		}

	}
	public void TestShip(Ship otherShip) {

		// if we're not alive, don't do any tests...
		if (state != ShipState.Normal)
			return;

		// Test if their shots are close enough to us. 
		if (otherShip.shots.TestShots(this)) {
			SetState(ShipState.Dying);
			game.SendPoint();
		}

		//Test for collision with ship or it's death explosion
		if ((otherShip.state == ShipState.Normal) | (otherShip.state == ShipState.Dying)) {
			Vector3 delta = this.Position.Location - otherShip.Position.Location;
			if (Vector3.Length(delta) < Constants.ShipCollisionLimit) {
				SetState(ShipState.Dying);
				game.SendPoint();
			}
		}
	}

	public void Render() {
		switch (this.state) {
			case ShipState.Normal: {
				device.RenderState.Lighting = true;
				device.RenderState.ZBufferEnable = true;
				device.Transform.World = shipMesh.Position.WorldMatrix;
				shipMesh.Render();
				shots.Render();
				break;
			}
			case ShipState.Dying: {
				break;
			}
			default: {
				return;
			}
		}
	}

}