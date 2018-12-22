using System;
using System.Drawing;
using System.Threading;
using Microsoft.DirectX;
using Microsoft.DirectX.DirectInput;
using DI = Microsoft.DirectX.DirectInput;
using Microsoft.DirectX.Direct3D;
using D3D = Microsoft.DirectX.Direct3D;
using Microsoft.DirectX.DirectSound;
using DS = Microsoft.DirectX.DirectSound;
using Container = System.ComponentModel.Container;
using System.Windows.Forms;
/// <summary>
/// The main windows form for the application.
/// </summary>
namespace SpaceDonuts {
	public class MainClass : System.Windows.Forms.Form{

		private int screenWidth = 800;
		private int screenHeight = 600;

		private String TileSetFileName = "donuts.bmp";
		private TileSet donutTileSet;
		private TileSet pyramidTileSet;
		private TileSet cubeTileSet;
		private TileSet sphereTileSet;
		private TileSet shipTileSet;
		private TileSet nixiesTileSet;
		private TileSet bulletTileSet;

		private Texture donutTexture;
		private BasicSprite s; //temp holder for sprite creation
		private ShipSprite ship; //reference to our ship

		private D3D.Device device;
		private DI.Device kbd;

		internal HighResolutionTimer hrt = new HighResolutionTimer();
		private SpriteManager sm;
		private System.ComponentModel.Container components = null;
		private float deltaTime;

		//Bullet state data
		private float lastBullet;
		private const float bulletSpacing = 0.01f;
		private const double bulletFireRadius = 22.6f;

		private Random rnd = new Random(); //no fancy seeding

		//Scorekeeping data
		private int totalScore = 0;
		private int livesRemaining = 2;
		private int gameLevel = 0;
		private int totalTargets = 0;
		private const int maxScoreDigits = 8;
		private const int maxLevelDigits = 3;

		private Sounds gameSounds;
		private Sounds shipSounds;
		private SoundHandler soundHandler;

		public MainClass() {
			InitializeComponent();

			this.SetStyle(ControlStyles.AllPaintingInWmPaint | ControlStyles.Opaque, true);
			this.Text = "Space Donuts";
			sm = new SpriteManager();
		}

		protected override void Dispose( bool disposing ) {
			if( disposing ) {
				if (components != null) {
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent() {
			// 
			// MainClass
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(640, 480);
			this.Name = "MainClass";
			this.Text = "Form1";
		}
		#endregion

		[STAThread]
		public static void Main() {
			using (MainClass frm = new MainClass()) {
				frm.Show();
				frm.Initialize();
				Application.Run(frm); //triggers OnPaint event, which is main loop
			}
			Application.Exit();
		}

		private void CollisionHandler(BasicSprite s1, BasicSprite s2) {
			//Check to see if a bullet or ship is hitting a target object
			BasicSprite collidable;
			BasicSprite target;

			if ((s1 is ShipSprite) || (s1 is BulletSprite) ) {
				collidable = s1;
				target = s2;
			}
			else {
				collidable = s2;
				target = s1;
			}

			//remove the bullet/ship from collision checking and take off list
			collidable.Visible = false; //will be ignored for future collisions
			collidable.DurationOver = true; //will be removed at next update
			//if it was a ship, take away a life and restart the ship
			if (collidable.GetType() == typeof(ShipSprite)) {
				shipSounds = Sounds.ShipExplode;
				ship.Visible = false;
				//remove the ship from the sprite manager
				ship.DurationOver = true; 
				//subtract a life
				livesRemaining--;
				//now make a new ship
				NewShip(); 
			}
			//Blow up object
			Destroy(target);
		}

		private void Destroy(BasicSprite sprite) {
			totalTargets--;
			gameSounds = 0; //clear out game sounds
			//sprite type should be: donut, pyramid, cube, or sphere
			if (sprite is DonutSprite) {
				for(int i = 0; i < 3; i++) {
					s = new PyramidSprite(pyramidTileSet);
					s.PositionY = sprite.PositionY;
					s.PositionX = sprite.PositionX;
					s.Angle = (float)rnd.Next(0, 359);
					s.Velocity = (float) rnd.Next(30, 200);
					sm.AddSprite(s);
					totalTargets++;
					gameSounds |= Sounds.DonutExplode;
				}
				totalScore += 10;
			}
			else if (sprite is PyramidSprite) {
				for(int i = 0; i < 2; i++) {
					s = new CubeSprite(cubeTileSet);
					s.PositionY = sprite.PositionY;
					s.PositionX = sprite.PositionX;
					s.Angle = (float)rnd.Next(0, 359);
					s.Velocity = (float) rnd.Next(30, 200);
					s.Visible = true;
					sm.AddSprite(s);
					totalTargets++;
					gameSounds |= Sounds.PyramidExplode;
				}
				totalScore += 20;
			}
			else if (sprite is CubeSprite) {
				for(int i = 0; i < 2; i++) {
					s = new SphereSprite(sphereTileSet);
					s.PositionY = sprite.PositionY;
					s.PositionX = sprite.PositionX;
					s.Angle = (float)rnd.Next(0, 359);
					s.Velocity = (float) rnd.Next(50, 200);
					sm.AddSprite(s);
					totalTargets++;
					gameSounds |= Sounds.CubeExplode;
				}
				totalScore += 10;
			}
			else if (sprite is SphereSprite) {
				totalScore += 10;
				gameSounds |= Sounds.SphereExplode;
			}
			sprite.Visible = false; //will be ignored for future collisions
			sprite.DurationOver = true; //will be removed at next update
		}

		private void Initialize() {
			try {
				//Common DirectX setup calls...
				PresentParameters presentParams = new PresentParameters();
				presentParams.Windowed = true;
				//				presentParams.BackBufferHeight = screenHeight;
				//				presentParams.BackBufferWidth = screenWidth;
				presentParams.SwapEffect = SwapEffect.Discard;
				presentParams.BackBufferFormat = Format.Unknown;
				presentParams.AutoDepthStencilFormat = DepthFormat.D16;
				presentParams.EnableAutoDepthStencil = true;

				// Store the default adapter
				int adapterOrdinal = D3D.Manager.Adapters.Default.Adapter;
				CreateFlags flags = CreateFlags.SoftwareVertexProcessing;

				// Check to see if we can use a pure hardware device
				D3D.Caps caps = D3D.Manager.GetDeviceCaps(adapterOrdinal, D3D.DeviceType.Hardware);

				// Do we support hardware vertex processing?
				if (caps.DeviceCaps.SupportsHardwareTransformAndLight)
					// Replace the software vertex processing
					flags = CreateFlags.HardwareVertexProcessing;
            
				// Do we support a pure device?
				if (caps.DeviceCaps.SupportsPureDevice)
					flags |= CreateFlags.PureDevice;

				device = new D3D.Device(0, D3D.DeviceType.Hardware, this, flags, presentParams);
				device.DeviceReset += new System.EventHandler(this.OnResetDevice);
				OnResetDevice(device, null);

				//Space Donuts setup
				donutTexture = TextureLoader.FromFile(device, MediaUtilities.FindFile(TileSetFileName), 1024, 1024, 
					1, 0,Format.A8R8G8B8, Pool.Managed, Filter.Point, Filter.Point, (unchecked((int)0xff000000)));

				donutTileSet = new TileSet(donutTexture, 0, 0, 6, 5, 32, 32);
				pyramidTileSet = new TileSet(donutTexture, 0, 384, 4, 10, 16, 16);
				sphereTileSet = new TileSet(donutTexture, 0, 512, 2, 20, 8, 8);
				cubeTileSet = new TileSet(donutTexture, 0, 544, 2, 20, 8, 8);
				shipTileSet = new TileSet(donutTexture, 0, 576, 4, 10, 16, 16);
				nixiesTileSet = new TileSet(donutTexture, 0, 832, 1, 14, 8, 8);
				bulletTileSet = new TileSet(donutTexture, 304, 832, 1, 1, 8, 2);

				//set up DirectInput keyboard device...
				kbd = new DI.Device(SystemGuid.Keyboard);
				kbd.SetCooperativeLevel(this, 
					DI.CooperativeLevelFlags.Background | DI.CooperativeLevelFlags.NonExclusive);
				kbd.Acquire();

				soundHandler = new SoundHandler(this);

				sm.OnCollisionEventHandler += new SpriteManager.HandleCollision(this.CollisionHandler);

				hrt.Start();
			}
			catch (DirectXException) {
				// Catch any errors and return a failure
			}
		}

		private void NewShip() {
			ship = new ShipSprite(shipTileSet); 
			ship.CollisionxExtent = 8; //make collision box smaller
			ship.CollisionyExtent = 8; 
			ship.PositionY = (float)this.Height/2;
			ship.PositionX = (float)this.Width/2;
			ship.Velocity = 0f;
			ship.CanCollide = true;
			ship.Angle = 0;
			ship.StartDelay = 2f; //delay start for 2 seconds
			ship.AnimationSpeed = 0f; //ship only moves from user input
			ship.Frame = 10; //aligns ship direction to 0 radians
			sm.AddSprite(ship);
			shipSounds = Sounds.ShipAppear | Sounds.ShipHum;
		}


		private void NewLevel() {
			gameLevel++;
			//Reset game sounds
			shipSounds = Sounds.ShipHum;
			gameSounds = Sounds.LevelStart;
			soundHandler.Play(shipSounds | gameSounds);
			DisplayLevel(gameLevel);
			//Remove all sprites from the sprite manager's list
			sm.Clear();
			//Create new entities
			NewShip();
			for(int i = 0; i < gameLevel; i++) {
				s = new DonutSprite(donutTileSet); 
				s.CollisionxExtent = 24; //make collision box smaller
				s.CollisionyExtent = 24;
				s.PositionY = rnd.Next(donutTileSet.ExtentY*4, this.Height-donutTileSet.ExtentY*4); 
				s.PositionX = rnd.Next(donutTileSet.ExtentX*4, this.Width-donutTileSet.ExtentX*4); 
				s.Angle = (float)rnd.Next(10, 350);
				s.Velocity = (float) rnd.Next(75, 150); 
				s.CanCollide = false;
				totalTargets++;
				sm.AddSprite(s);
			}
			shipSounds = 0;
			gameSounds = 0;
			hrt.Reset();
		}

		public void DisplayLevel(int level) {
			int characterStart = 30;
			int characterSpacing = 15;
			Vector3 displayPosition = 
				new Vector3((float)this.Width/2 - characterStart, (float)this.Height/2, 0f);
			int digit;
			NixieSprite nixie = new NixieSprite(nixiesTileSet);

			//Render the level indicator
			device.Clear(ClearFlags.Target | ClearFlags.ZBuffer, Color.Black, 1.0f, 0);
			device.BeginScene();
			using (D3D.Sprite d3dSprite = new D3D.Sprite(device)) {
				d3dSprite.Begin(D3D.SpriteFlags.AlphaBlend);

				//Show the letter L 
				nixie.Draw(d3dSprite, NixieSprite.NixieCharacters.L, displayPosition);

				//Show the letter E
				displayPosition.X -= characterSpacing;
				nixie.Draw(d3dSprite, NixieSprite.NixieCharacters.E, displayPosition);

				//Show the letter V
				displayPosition.X -= characterSpacing;
				nixie.Draw(d3dSprite, NixieSprite.NixieCharacters.V, displayPosition);

				//Show the letter E
				displayPosition.X -= characterSpacing;
				nixie.Draw(d3dSprite, NixieSprite.NixieCharacters.E, displayPosition);

				//Show the letter L
				displayPosition.X -= characterSpacing;
				nixie.Draw(d3dSprite, NixieSprite.NixieCharacters.L, displayPosition);

				displayPosition.X = (float)this.Width/2 + 40;
				for (int digitCount = 1; digitCount <= maxLevelDigits; digitCount++) {
					digit = level % 10;
					level /= 10;
					nixie.Draw(d3dSprite, (NixieSprite.NixieCharacters)digit, displayPosition);
					displayPosition.X -= characterSpacing;
				} 
				d3dSprite.End();
			}
			device.EndScene();
			device.Present();
			Thread.Sleep(3000); //wait for 3 seconds
		}

		public void OnResetDevice(object sender, EventArgs e) {
			D3D.Device device = (D3D.Device)sender;
		}

		protected override void OnPaint(System.Windows.Forms.PaintEventArgs e) {
			//Update game state
			if (totalTargets == 0) NewLevel();
			if ((gameSounds|shipSounds) != 0) {
				soundHandler.Play(gameSounds | shipSounds);
				shipSounds = Sounds.ShipHum;
				gameSounds = 0;
			}
			deltaTime = hrt.ElapsedTime;
			ProcessInputState(deltaTime); //get keyboard input
			sm.Update(deltaTime);
			sm.CollisionTest();

			//Render the images
			device.Clear(ClearFlags.Target | ClearFlags.ZBuffer, Color.Blue, 1.0f, 0);
			device.BeginScene();
			sm.Draw(device);
			WriteScore(device, totalScore);
			device.EndScene();
			device.Present();
			this.Invalidate();
		}

		protected void ProcessInputState(float delta) {
			foreach (Key k in kbd.GetPressedKeys()) {
				if (k == Key.Space && ship.Visible) {
					//Fire guns
					lastBullet += delta;
					if (lastBullet > bulletSpacing) {
						BulletSprite bullet = new BulletSprite(bulletTileSet); 
						//Calculate bullet start position, outside ship boundaries
						float radAngle = Geometry.DegreeToRadian(ship.Angle);
						int yOffset = (int) ( bulletFireRadius * Math.Sin((double)radAngle));
						int xOffset = (int) ( bulletFireRadius * Math.Cos((double)radAngle));
						bullet.PositionY = (ship.PositionY) + shipTileSet.ExtentY + yOffset;
						//the -4 below is a small nudge to center up the bullets
						bullet.PositionX = (ship.PositionX) + shipTileSet.ExtentX + xOffset - 4; 
						bullet.Angle = ship.Angle;
						bullet.Velocity = 150f;
						bullet.AnimationSpeed = 0f;
						bullet.CanCollide = true;
						bullet.LimitLifespan(2f); //only 2 seconds to live
						sm.AddSprite(bullet);
						lastBullet = 0f;
						if (totalScore > 0) totalScore--; //lose a point for each bullet
						shipSounds |= Sounds.ShipFire;
					}
				}
				if (k == Key.Left)  ship.PreviousFrame();
				if (k == Key.Right) ship.NextFrame();
				if (k == Key.Up) {
					ship.Thrust();
					shipSounds |= Sounds.ShipThrust;
				}
				if (k == Key.Down) {
					//Put on the brakes!
					ship.VelocityX = 0f;
					ship.VelocityY = 0f;
					shipSounds |= Sounds.ShipBrake;
				}
				if (k == Key.Escape) {
					kbd.Unacquire(); //release the keyboard device
					kbd.Dispose();
					Application.Exit();
				}
				if (k == Key.Home) {
					//resets ship to starting position
					ship.PositionY = (float)this.Height/2;
					ship.PositionX = (float)this.Width/2;
					ship.VelocityX = 0f;
					ship.VelocityY = 0f;
					ship.Angle = 0.0f;
					ship.Frame = 10;
				}
				//if (k == Key.D) sm.ShowList();
			}
		}

		public void WriteScore(D3D.Device device, int score) {
			Rectangle nixiePosition = new Rectangle(nixiesTileSet.XOrigin,nixiesTileSet.YOrigin,nixiesTileSet.ExtentX*2,nixiesTileSet.ExtentY*2);
			int digit;
			using (D3D.Sprite d3dSprite = new D3D.Sprite(device)) {
				d3dSprite.Begin(D3D.SpriteFlags.AlphaBlend);
				for (int digitCount = 1; digitCount <= maxScoreDigits; digitCount++) {
					digit = score % 10;
					score /= 10;
					nixiePosition.X = nixiesTileSet.XOrigin + ( digit % nixiesTileSet.NumberFrameColumns ) * nixiesTileSet.ExtentX*2;
					nixiePosition.Y = nixiesTileSet.YOrigin; //we know it's only one row
					Vector3 position = new Vector3((float)this.Width/2 - digitCount*nixiesTileSet.ExtentX*2, 
						(float)this.Height-60f, 0f);
					d3dSprite.Draw(nixiesTileSet.Texture, nixiePosition,  
						new Vector3(), position, Color.FromArgb(255,255,255,255));
				} 
				d3dSprite.End();
			}
		}
	}
}