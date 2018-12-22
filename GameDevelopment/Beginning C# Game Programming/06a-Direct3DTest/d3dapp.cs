//-----------------------------------------------------------------------------
// File: D3DApp.cs
//
// Desc: Application class for the Direct3D samples framework library.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
using System;
using Microsoft.DirectX;
using Microsoft.DirectX.Direct3D;
using Direct3D = Microsoft.DirectX.Direct3D;

/// <summary>
/// The base class for all the graphics (D3D) samples, it derives from windows forms
/// </summary>
public class GraphicsSample : System.Windows.Forms.Form {
	#region Menu Information
	// The menu items that *all* samples will need
	protected System.Windows.Forms.MainMenu mnuMain;
	protected System.Windows.Forms.MenuItem mnuFile;
	private System.Windows.Forms.MenuItem mnuGo;
	private System.Windows.Forms.MenuItem mnuSingle;
	private System.Windows.Forms.MenuItem mnuBreak1;
	private System.Windows.Forms.MenuItem mnuChange;
	private System.Windows.Forms.MenuItem mnuBreak2;
	protected System.Windows.Forms.MenuItem mnuExit;
	#endregion

	// The window we will render too
	private System.Windows.Forms.Control ourRenderTarget;
	// Should we use the default windows
	protected bool isUsingMenus = true;

	// We need to keep track of our enumeration settings
	protected D3DEnumeration enumerationSettings = new D3DEnumeration();
	protected D3DSettings graphicsSettings = new D3DSettings();
	private bool isMaximized = false; // Are we maximized?
	private bool isHandlingSizeChanges = true; // Are we handling size changes?
	private bool isClosing = false; // Are we closing?
	private bool isChangingFormStyle = false; // Are we changing the forms style?
	private bool isWindowActive = true; // Are we waiting for got focus?

	private float lastTime = 0.0f; // The last time
	private int frames = 0; // Number of frames since our last update
	private int appPausedCount = 0; // How many times has the app been paused (and when can it resume)?

	// Internal variables for the state of the app
	protected bool windowed;
	protected bool active;
	protected bool ready;
	protected bool hasFocus;
	protected bool isMultiThreaded = false;

	// Internal variables used for timing
	protected bool frameMoving;
	protected bool singleStep;
	// Main objects used for creating and rendering the 3D scene
	protected PresentParameters presentParams = new PresentParameters(); // Parameters for CreateDevice/Reset
	protected Device device; // The rendering device
	protected RenderStates renderState;
	protected SamplerStates sampleState;
	protected TextureStates textureStates;
	private Caps graphicsCaps;           // Caps for the device
	protected Caps Caps { get { return graphicsCaps; } }
	private CreateFlags behavior;     // Indicate sw or hw vertex processing
	protected BehaviorFlags BehaviorFlags { get { return new BehaviorFlags(behavior); } }
	protected System.Windows.Forms.Control RenderTarget { get { return ourRenderTarget; } set { ourRenderTarget = value; } }

	// Variables for timing
	protected float appTime;             // Current time in seconds
	protected float elapsedTime;      // Time elapsed since last frame
	protected float framePerSecond;              // Instanteous frame rate
	protected string deviceStats;// String to hold D3D device stats
	protected string frameStats; // String to hold frame stats

	private bool deviceLost = false;

	// Overridable variables for the app
	private int minDepthBits;    // Minimum number of bits needed in depth buffer
	protected int MinDepthBits { get { return minDepthBits; } set { minDepthBits = value;  enumerationSettings.AppMinDepthBits = value;} }
	private int minStencilBits;  // Minimum number of bits needed in stencil buffer
	protected int MinStencilBits { get { return minStencilBits; } set { minStencilBits = value;  enumerationSettings.AppMinStencilBits = value;} }
	protected bool showCursorWhenFullscreen; // Whether to show cursor when fullscreen
	protected bool clipCursorWhenFullscreen; // Whether to limit cursor pos when fullscreen
	protected bool startFullscreen; // Whether to start up the app in fullscreen mode

	private System.Drawing.Size storedSize;
	private System.Drawing.Point storedLocation;

	// Overridable functions for the 3D scene created by the app
	protected virtual bool ConfirmDevice(Caps caps, VertexProcessingType vertexProcessingType, 
		Format adapterFormat, Format backBufferFormat) { return true; }
	protected virtual void OneTimeSceneInitialization() { /* Do Nothing */ }
	protected virtual void InitializeDeviceObjects() { /* Do Nothing */ }
	protected virtual void RestoreDeviceObjects(System.Object sender, System.EventArgs e) { /* Do Nothing */ }
	protected virtual void FrameMove() { /* Do Nothing */ }
	protected virtual void Render() { /* Do Nothing */ }
	protected virtual void InvalidateDeviceObjects(System.Object sender, System.EventArgs e) { /* Do Nothing */ }
	protected virtual void DeleteDeviceObjects(System.Object sender, System.EventArgs e) { /* Do Nothing */ }




	/// <summary>
	/// Constructor
	/// </summary>
	public GraphicsSample() {
		device = null;
		active = false;
		ready = false;
		hasFocus = false;
		behavior = 0;

		ourRenderTarget = this;
		frameMoving = true;
		singleStep = false;
		framePerSecond = 0.0f;
		deviceStats = null;
		frameStats  = null;

		this.Text = "D3D9 Sample";
		this.ClientSize = new System.Drawing.Size(400,300);
		this.KeyPreview = true;

		minDepthBits = 16;
		minStencilBits = 0;
		showCursorWhenFullscreen = false;
		startFullscreen = false;

		// When clipCursorWhenFullscreen is TRUE, the cursor is limited to
		// the device window when the app goes fullscreen.  This prevents users
		// from accidentally clicking outside the app window on a multimon system.
		// This flag is turned off by default for debug builds, since it makes 
		// multimon debugging difficult.
#if (DEBUG)
		clipCursorWhenFullscreen = false;
#else
            clipCursorWhenFullscreen = true;
#endif
		InitializeComponent();
	}




	/// <summary>
	/// Picks the best graphics device, and initializes it
	/// </summary>
	/// <returns>true if a good device was found, false otherwise</returns>
	public bool CreateGraphicsSample() {
		enumerationSettings.ConfirmDeviceCallback = new D3DEnumeration.ConfirmDeviceCallbackType(this.ConfirmDevice);
		enumerationSettings.Enumerate();

		if (ourRenderTarget.Cursor == null) {
			// Set up a default cursor
			ourRenderTarget.Cursor = System.Windows.Forms.Cursors.Default;
		}
		// if our render target is the main window and we haven't said 
		// ignore the menus, add our menu
		if ((ourRenderTarget == this) && (isUsingMenus))
			this.Menu = mnuMain;

		try {
			ChooseInitialSettings();

			// Initialize the application timer
			DXUtil.Timer(DirectXTimer.Start);
			// Initialize the app's custom scene stuff
			OneTimeSceneInitialization();
			// Initialize the 3D environment for the app
			InitializeEnvironment();
		}
		catch (SampleException d3de) {
			HandleSampleException(d3de, ApplicationMessage.ApplicationMustExit);
			return false;
		}
		catch {
			HandleSampleException(new SampleException(), ApplicationMessage.ApplicationMustExit);
			return false;
		}

		// The app is ready to go
		ready = true;

		return true;
	}




	/// <summary>
	/// Sets up graphicsSettings with best available windowed mode, subject to 
	/// the doesRequireHardware and doesRequireReference constraints.  
	/// </summary>
	/// <param name="doesRequireHardware">Does the device require hardware support</param>
	/// <param name="doesRequireReference">Does the device require the ref device</param>
	/// <returns>true if a mode is found, false otherwise</returns>
	public bool FindBestWindowedMode(bool doesRequireHardware, bool doesRequireReference) {
		// Get display mode of primary adapter (which is assumed to be where the window 
		// will appear)
		DisplayMode primaryDesktopDisplayMode = Manager.Adapters[0].CurrentDisplayMode;

		GraphicsAdapterInfo bestAdapterInfo = null;
		GraphicsDeviceInfo bestDeviceInfo = null;
		DeviceCombo bestDeviceCombo = null;

		foreach (GraphicsAdapterInfo adapterInfo in enumerationSettings.AdapterInfoList) {
			foreach (GraphicsDeviceInfo deviceInfo in adapterInfo.DeviceInfoList) {
				if (doesRequireHardware && deviceInfo.DevType != DeviceType.Hardware)
					continue;
				if (doesRequireReference && deviceInfo.DevType != DeviceType.Reference)
					continue;

				foreach (DeviceCombo deviceCombo in deviceInfo.DeviceComboList) {
					bool adapterMatchesBackBuffer = (deviceCombo.BackBufferFormat == deviceCombo.AdapterFormat);
					if (!deviceCombo.IsWindowed)
						continue;
					if (deviceCombo.AdapterFormat != primaryDesktopDisplayMode.Format)
						continue;

					// If we haven't found a compatible DeviceCombo yet, or if this set
					// is better (because it's a HAL, and/or because formats match better),
					// save it
					if (bestDeviceCombo == null || 
						bestDeviceCombo.DevType != DeviceType.Hardware && deviceInfo.DevType == DeviceType.Hardware ||
						deviceCombo.DevType == DeviceType.Hardware && adapterMatchesBackBuffer) {
						bestAdapterInfo = adapterInfo;
						bestDeviceInfo = deviceInfo;
						bestDeviceCombo = deviceCombo;
						if (deviceInfo.DevType == DeviceType.Hardware && adapterMatchesBackBuffer) {
							// This windowed device combo looks great -- take it
							goto EndWindowedDeviceComboSearch;
						}
						// Otherwise keep looking for a better windowed device combo
					}
				}
			}
		}

		EndWindowedDeviceComboSearch:
			if (bestDeviceCombo == null)
				return false;

		graphicsSettings.WindowedAdapterInfo = bestAdapterInfo;
		graphicsSettings.WindowedDeviceInfo = bestDeviceInfo;
		graphicsSettings.WindowedDeviceCombo = bestDeviceCombo;
		graphicsSettings.IsWindowed = true;
		graphicsSettings.WindowedDisplayMode = primaryDesktopDisplayMode;
		graphicsSettings.WindowedWidth = ourRenderTarget.ClientRectangle.Right - ourRenderTarget.ClientRectangle.Left;
		graphicsSettings.WindowedHeight = ourRenderTarget.ClientRectangle.Bottom - ourRenderTarget.ClientRectangle.Top;
		if (enumerationSettings.AppUsesDepthBuffer)
			graphicsSettings.WindowedDepthStencilBufferFormat = (DepthFormat)bestDeviceCombo.DepthStencilFormatList[0];
		graphicsSettings.WindowedMultisampleType = (MultiSampleType)bestDeviceCombo.MultiSampleTypeList[0];
		graphicsSettings.WindowedMultisampleQuality = 0;
		graphicsSettings.WindowedVertexProcessingType = (VertexProcessingType)bestDeviceCombo.VertexProcessingTypeList[0];
		graphicsSettings.WindowedPresentInterval = (PresentInterval)bestDeviceCombo.PresentIntervalList[0];

		return true;
	}




	/// <summary>
	/// Sets up graphicsSettings with best available fullscreen mode, subject to 
	/// the doesRequireHardware and doesRequireReference constraints.  
	/// </summary>
	/// <param name="doesRequireHardware">Does the device require hardware support</param>
	/// <param name="doesRequireReference">Does the device require the ref device</param>
	/// <returns>true if a mode is found, false otherwise</returns>
	public bool FindBestFullscreenMode(bool doesRequireHardware, bool doesRequireReference) {
		// For fullscreen, default to first HAL DeviceCombo that supports the current desktop 
		// display mode, or any display mode if HAL is not compatible with the desktop mode, or 
		// non-HAL if no HAL is available
		DisplayMode adapterDesktopDisplayMode = new DisplayMode();
		DisplayMode bestAdapterDesktopDisplayMode = new DisplayMode();
		DisplayMode bestDisplayMode = new DisplayMode();
		bestAdapterDesktopDisplayMode.Width = 0;
		bestAdapterDesktopDisplayMode.Height = 0;
		bestAdapterDesktopDisplayMode.Format = 0;
		bestAdapterDesktopDisplayMode.RefreshRate = 0;

		GraphicsAdapterInfo bestAdapterInfo = null;
		GraphicsDeviceInfo bestDeviceInfo = null;
		DeviceCombo bestDeviceCombo = null;

		foreach (GraphicsAdapterInfo adapterInfo in enumerationSettings.AdapterInfoList) {
			adapterDesktopDisplayMode = Manager.Adapters[adapterInfo.AdapterOrdinal].CurrentDisplayMode;
			foreach (GraphicsDeviceInfo deviceInfo in adapterInfo.DeviceInfoList) {
				if (doesRequireHardware && deviceInfo.DevType != DeviceType.Hardware)
					continue;
				if (doesRequireReference && deviceInfo.DevType != DeviceType.Reference)
					continue;

				foreach (DeviceCombo deviceCombo in deviceInfo.DeviceComboList) {
					bool adapterMatchesBackBuffer = (deviceCombo.BackBufferFormat == deviceCombo.AdapterFormat);
					bool adapterMatchesDesktop = (deviceCombo.AdapterFormat == adapterDesktopDisplayMode.Format);
					if (deviceCombo.IsWindowed)
						continue;

					// If we haven't found a compatible set yet, or if this set
					// is better (because it's a HAL, and/or because formats match better),
					// save it
					if (bestDeviceCombo == null ||
						bestDeviceCombo.DevType != DeviceType.Hardware && deviceInfo.DevType == DeviceType.Hardware ||
						bestDeviceCombo.DevType == DeviceType.Hardware && bestDeviceCombo.AdapterFormat != adapterDesktopDisplayMode.Format && adapterMatchesDesktop ||
						bestDeviceCombo.DevType == DeviceType.Hardware && adapterMatchesDesktop && adapterMatchesBackBuffer) {
						bestAdapterDesktopDisplayMode = adapterDesktopDisplayMode;
						bestAdapterInfo = adapterInfo;
						bestDeviceInfo = deviceInfo;
						bestDeviceCombo = deviceCombo;
						if (deviceInfo.DevType == DeviceType.Hardware && adapterMatchesDesktop && adapterMatchesBackBuffer) {
							// This fullscreen device combo looks great -- take it
							goto EndFullscreenDeviceComboSearch;
						}
						// Otherwise keep looking for a better fullscreen device combo
					}
				}
			}
		}

		EndFullscreenDeviceComboSearch:
			if (bestDeviceCombo == null)
				return false;

		// Need to find a display mode on the best adapter that uses pBestDeviceCombo->AdapterFormat
		// and is as close to bestAdapterDesktopDisplayMode's res as possible
		bestDisplayMode.Width = 0;
		bestDisplayMode.Height = 0;
		bestDisplayMode.Format = 0;
		bestDisplayMode.RefreshRate = 0;
		foreach (DisplayMode displayMode in bestAdapterInfo.DisplayModeList) {
			if (displayMode.Format != bestDeviceCombo.AdapterFormat)
				continue;
			if (displayMode.Width == bestAdapterDesktopDisplayMode.Width &&
				displayMode.Height == bestAdapterDesktopDisplayMode.Height && 
				displayMode.RefreshRate == bestAdapterDesktopDisplayMode.RefreshRate) {
				// found a perfect match, so stop
				bestDisplayMode = displayMode;
				break;
			}
			else if (displayMode.Width == bestAdapterDesktopDisplayMode.Width &&
				displayMode.Height == bestAdapterDesktopDisplayMode.Height && 
				displayMode.RefreshRate > bestDisplayMode.RefreshRate) {
				// refresh rate doesn't match, but width/height match, so keep this
				// and keep looking
				bestDisplayMode = displayMode;
			}
			else if (bestDisplayMode.Width == bestAdapterDesktopDisplayMode.Width) {
				// width matches, so keep this and keep looking
				bestDisplayMode = displayMode;
			}
			else if (bestDisplayMode.Width == 0) {
				// we don't have anything better yet, so keep this and keep looking
				bestDisplayMode = displayMode;
			}
		}

		graphicsSettings.FullscreenAdapterInfo = bestAdapterInfo;
		graphicsSettings.FullscreenDeviceInfo = bestDeviceInfo;
		graphicsSettings.FullscreenDeviceCombo = bestDeviceCombo;
		graphicsSettings.IsWindowed = false;
		graphicsSettings.FullscreenDisplayMode = bestDisplayMode;
		if (enumerationSettings.AppUsesDepthBuffer)
			graphicsSettings.FullscreenDepthStencilBufferFormat = (DepthFormat)bestDeviceCombo.DepthStencilFormatList[0];
		graphicsSettings.FullscreenMultisampleType = (MultiSampleType)bestDeviceCombo.MultiSampleTypeList[0];
		graphicsSettings.FullscreenMultisampleQuality = 0;
		graphicsSettings.FullscreenVertexProcessingType = (VertexProcessingType)bestDeviceCombo.VertexProcessingTypeList[0];
		graphicsSettings.FullscreenPresentInterval = PresentInterval.Default;

		return true;
	}


 

	/// <summary>
	/// Choose the initial settings for the application
	/// </summary>
	/// <returns>true if the settings were initialized</returns>
	public bool ChooseInitialSettings() {
		bool foundFullscreenMode = FindBestFullscreenMode(false, false);
		bool foundWindowedMode = FindBestWindowedMode(false, false);
		if (startFullscreen && foundFullscreenMode)
			graphicsSettings.IsWindowed = false;

		if (!foundFullscreenMode && !foundWindowedMode)
			throw new NoCompatibleDevicesException();

		return (foundFullscreenMode || foundWindowedMode);
	}




	/// <summary>
	/// Build presentation parameters from the current settings
	/// </summary>
	public void BuildPresentParamsFromSettings() {
		presentParams.Windowed = graphicsSettings.IsWindowed;
		presentParams.BackBufferCount = 1;
		presentParams.MultiSample = graphicsSettings.MultisampleType;
		presentParams.MultiSampleQuality = graphicsSettings.MultisampleQuality;
		presentParams.SwapEffect = SwapEffect.Discard;
		presentParams.EnableAutoDepthStencil = enumerationSettings.AppUsesDepthBuffer;
		presentParams.AutoDepthStencilFormat = graphicsSettings.DepthStencilBufferFormat;
		presentParams.PresentFlag = PresentFlag.None;

		if (windowed) {
			presentParams.BackBufferWidth  = ourRenderTarget.ClientRectangle.Right - ourRenderTarget.ClientRectangle.Left;
			presentParams.BackBufferHeight = ourRenderTarget.ClientRectangle.Bottom - ourRenderTarget.ClientRectangle.Top;
			presentParams.BackBufferFormat = graphicsSettings.DeviceCombo.BackBufferFormat;
			presentParams.FullScreenRefreshRateInHz = 0;
			presentParams.PresentationInterval = PresentInterval.Immediate;
			presentParams.DeviceWindow = ourRenderTarget;
		}
		else {
			presentParams.BackBufferWidth  = graphicsSettings.DisplayMode.Width;
			presentParams.BackBufferHeight = graphicsSettings.DisplayMode.Height;
			presentParams.BackBufferFormat = graphicsSettings.DeviceCombo.BackBufferFormat;
			presentParams.FullScreenRefreshRateInHz = graphicsSettings.DisplayMode.RefreshRate;
			presentParams.PresentationInterval = graphicsSettings.PresentInterval;
			presentParams.DeviceWindow = this;
		}
	}




	/// <summary>
	/// Initialize the graphics environment
	/// </summary>
	public void InitializeEnvironment() {
		GraphicsAdapterInfo adapterInfo = graphicsSettings.AdapterInfo;
		GraphicsDeviceInfo deviceInfo = graphicsSettings.DeviceInfo;

		windowed = graphicsSettings.IsWindowed;

		// Set up the presentation parameters
		BuildPresentParamsFromSettings();

		if (deviceInfo.Caps.PrimitiveMiscCaps.IsNullReference) {
			// Warn user about null ref device that can't render anything
			HandleSampleException(new NullReferenceDeviceException(), ApplicationMessage.None);
		}

		CreateFlags createFlags = new CreateFlags();
		if (graphicsSettings.VertexProcessingType == VertexProcessingType.Software)
			createFlags = CreateFlags.SoftwareVertexProcessing;
		else if (graphicsSettings.VertexProcessingType == VertexProcessingType.Mixed)
			createFlags = CreateFlags.MixedVertexProcessing;
		else if (graphicsSettings.VertexProcessingType == VertexProcessingType.Hardware)
			createFlags = CreateFlags.HardwareVertexProcessing;
		else if (graphicsSettings.VertexProcessingType == VertexProcessingType.PureHardware) {
			createFlags = CreateFlags.HardwareVertexProcessing | CreateFlags.PureDevice;
		}
		else
			throw new ApplicationException();
#if (DX9)
#else
		// Make sure to allow multithreaded apps if we need them
		presentParams.ForceNoMultiThreadedFlag = !isMultiThreaded;
#endif
		try {
			// Create the device
			device = new Device(graphicsSettings.AdapterOrdinal, graphicsSettings.DevType, 
				windowed ? ourRenderTarget : this , createFlags, presentParams);

			// Cache our local objects
			renderState = device.RenderState;
			sampleState = device.SamplerState;
			textureStates = device.TextureState;
			// When moving from fullscreen to windowed mode, it is important to
			// adjust the window size after recreating the device rather than
			// beforehand to ensure that you get the window size you want.  For
			// example, when switching from 640x480 fullscreen to windowed with
			// a 1000x600 window on a 1024x768 desktop, it is impossible to set
			// the window size to 1000x600 until after the display mode has
			// changed to 1024x768, because windows cannot be larger than the
			// desktop.
			if (windowed) {
				// Make sure main window isn't topmost, so error message is visible
				System.Drawing.Size currentClientSize = this.ClientSize;
				this.Size = this.ClientSize;
				this.SendToBack();
				this.BringToFront();
				this.ClientSize = currentClientSize;
			}

			// Store device Caps
			graphicsCaps = device.DeviceCaps;
			behavior = createFlags;

			System.Text.StringBuilder sb = new System.Text.StringBuilder();

			// Store device description
			if (deviceInfo.DevType == DeviceType.Reference)
				sb.Append("REF");
			else if (deviceInfo.DevType == DeviceType.Hardware)
				sb.Append("HAL");
			else if (deviceInfo.DevType == DeviceType.Software)
				sb.Append("SW");

			BehaviorFlags behaviorFlags = new BehaviorFlags(createFlags);
			if ((behaviorFlags.HardwareVertexProcessing) && 
				(behaviorFlags.PureDevice)) {
				if (deviceInfo.DevType == DeviceType.Hardware)
					sb.Append(" (pure hw vp)");
				else
					sb.Append(" (simulated pure hw vp)");
			}
			else if (behaviorFlags.HardwareVertexProcessing) {
				if (deviceInfo.DevType == DeviceType.Hardware)
					sb.Append(" (hw vp)");
				else
					sb.Append(" (simulated hw vp)");
			}
			else if (behaviorFlags.MixedVertexProcessing) {
				if (deviceInfo.DevType == DeviceType.Hardware)
					sb.Append(" (mixed vp)");
				else
					sb.Append(" (simulated mixed vp)");
			}
			else if (behaviorFlags.SoftwareVertexProcessing) {
				sb.Append(" (sw vp)");
			}

			if (deviceInfo.DevType == DeviceType.Hardware) {
				sb.Append(": ");
				sb.Append(adapterInfo.AdapterDetails.Description);
			}

			// Set device stats string
			deviceStats = sb.ToString();

			// Set up the fullscreen cursor
			if (showCursorWhenFullscreen && !windowed) {
				System.Windows.Forms.Cursor ourCursor = this.Cursor;
				device.SetCursor(ourCursor, true);
				device.ShowCursor(true);
			}

			// Confine cursor to fullscreen window
			if (clipCursorWhenFullscreen) {
				if (!windowed) {
					System.Drawing.Rectangle rcWindow = this.ClientRectangle;
				}
			}

			// Setup the event handlers for our device
			device.DeviceLost += new System.EventHandler(this.InvalidateDeviceObjects);
			device.DeviceReset += new System.EventHandler(this.RestoreDeviceObjects);
			device.Disposing += new System.EventHandler(this.DeleteDeviceObjects);
			device.DeviceResizing += new System.ComponentModel.CancelEventHandler(this.EnvironmentResized);

			// Initialize the app's device-dependent objects
			try {
				InitializeDeviceObjects();
				RestoreDeviceObjects(null, null);
				active = true;
				return;
			}
			catch {
				// Cleanup before we try again
				InvalidateDeviceObjects(null, null);
				DeleteDeviceObjects(null, null);
				device.Dispose();
				device = null;
				if (this.Disposing)
					return;
			}
		}
		catch {
			// If that failed, fall back to the reference rasterizer
			if (deviceInfo.DevType == DeviceType.Hardware) {
				if (FindBestWindowedMode(false, true)) {
					windowed = true;

					// Make sure main window isn't topmost, so error message is visible
					System.Drawing.Size currentClientSize = this.ClientSize;
					this.Size = this.ClientSize;
					this.SendToBack();
					this.BringToFront();
					this.ClientSize = currentClientSize;

					// Let the user know we are switching from HAL to the reference rasterizer
					HandleSampleException(null, ApplicationMessage.WarnSwitchToRef);

					InitializeEnvironment();
				}
			}
		}
	}




	/// <summary>
	/// Displays sample exceptions to the user
	/// </summary>
	/// <param name="e">The exception that was thrown</param>
	/// <param name="Type">Extra information on how to handle the exception</param>
	public void HandleSampleException(SampleException e, ApplicationMessage Type) {
		// Build a message to display to the user
		string strMsg = string.Empty;
		if (e != null)
			strMsg = e.Message;

		if (ApplicationMessage.ApplicationMustExit == Type) {
			strMsg  += "\n\nThis sample will now exit.";
			System.Windows.Forms.MessageBox.Show(strMsg, this.Text, 
				System.Windows.Forms.MessageBoxButtons.OK, System.Windows.Forms.MessageBoxIcon.Error);

			// Close the window, which shuts down the app
			if (this.IsHandleCreated)
				this.Close();
		}
		else {
			if (ApplicationMessage.WarnSwitchToRef == Type)
				strMsg = "\n\nSwitching to the reference rasterizer,\n";
			strMsg += "a software device that implements the entire\n";
			strMsg += "Direct3D feature set, but runs very slowly.";

			System.Windows.Forms.MessageBox.Show(strMsg, this.Text, 
				System.Windows.Forms.MessageBoxButtons.OK, System.Windows.Forms.MessageBoxIcon.Information);
		}
	}




	/// <summary>
	/// Fired when our environment was resized
	/// </summary>
	/// <param name="sender">the device that's resizing our environment</param>
	/// <param name="e">Set the cancel member to true to turn off automatic device reset</param>
	public void EnvironmentResized(object sender, System.ComponentModel.CancelEventArgs e) {
		// Check to see if we're closing or changing the form style
		if ((isClosing) || (isChangingFormStyle)) {
			// We are, cancel our reset, and exit
			e.Cancel = true;
			return;
		}

		// Check to see if we're minimizing and our rendering object
		// is not the form, if so, cancel the resize
		if ((ourRenderTarget != this) && (this.WindowState == System.Windows.Forms.FormWindowState.Minimized))
			e.Cancel = true;

		if (!isWindowActive) 
			e.Cancel = true;

		// Set up the fullscreen cursor
		if (showCursorWhenFullscreen && !windowed) {
			System.Windows.Forms.Cursor ourCursor = this.Cursor;
			device.SetCursor(ourCursor, true);
			device.ShowCursor(true);
		}

		// If the app is paused, trigger the rendering of the current frame
		if (false == frameMoving) {
			singleStep = true;
			DXUtil.Timer(DirectXTimer.Start);
			DXUtil.Timer(DirectXTimer.Stop);
		}
	}




	/// <summary>
	/// Called when user toggles between fullscreen mode and windowed mode
	/// </summary>
	public void ToggleFullscreen() {
		int AdapterOrdinalOld = graphicsSettings.AdapterOrdinal;
		DeviceType DevTypeOld = graphicsSettings.DevType;

		isHandlingSizeChanges = false;
		isChangingFormStyle = true;
		ready = false;

		// Toggle the windowed state
		windowed = !windowed;

		// Save our maximized settings..
		if (!windowed && isMaximized)
			this.WindowState = System.Windows.Forms.FormWindowState.Normal;

		graphicsSettings.IsWindowed = windowed;

		// If AdapterOrdinal and DevType are the same, we can just do a Reset().
		// If they've changed, we need to do a complete device teardown/rebuild.
		if (graphicsSettings.AdapterOrdinal == AdapterOrdinalOld &&
			graphicsSettings.DevType == DevTypeOld) {
			BuildPresentParamsFromSettings();
			// Resize the 3D device
		RETRY:
			try {
				device.Reset(presentParams);
			}
			catch {
				if (windowed)
					ForceWindowed();
				else {
					// Sit in a loop until the device passes Reset()
					try {
						device.TestCooperativeLevel();
					}
					catch(DeviceNotResetException) {
						// Device still needs to be Reset. Try again.
						// Yield some CPU time to other processes
						System.Threading.Thread.Sleep(100); // 100 milliseconds
						goto RETRY;
					}
				}
			}
			EnvironmentResized(device, new System.ComponentModel.CancelEventArgs());
		}
		else {
			device.Dispose();
			device = null;
			InitializeEnvironment();
		}

		// When moving from fullscreen to windowed mode, it is important to
		// adjust the window size after resetting the device rather than
		// beforehand to ensure that you get the window size you want.  For
		// example, when switching from 640x480 fullscreen to windowed with
		// a 1000x600 window on a 1024x768 desktop, it is impossible to set
		// the window size to 1000x600 until after the display mode has
		// changed to 1024x768, because windows cannot be larger than the
		// desktop.

		if (windowed) {
			// if our render target is the main window and we haven't said 
			// ignore the menus, add our menu
			if ((ourRenderTarget == this) && (isUsingMenus))
				this.Menu = mnuMain;
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.Sizable;
			isChangingFormStyle = false;

			// We were maximized, restore that state
			if (isMaximized) {
				this.WindowState = System.Windows.Forms.FormWindowState.Maximized;
			}
			this.SendToBack();
			this.BringToFront();
			this.ClientSize = storedSize;
			this.Location = storedLocation;
		}
		else {
			if (this.Menu != null)
				this.Menu = null;

			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
			isChangingFormStyle = false;
		}
		isHandlingSizeChanges = true;
		ready = true;
	}




	/// <summary>
	/// Switch to a windowed mode, even if that means picking a new device and/or adapter
	/// </summary>
	public void ForceWindowed() {
		if (windowed)
			return;

		if (!FindBestWindowedMode(false, false)) {
			return;
		}
		windowed = true;

		// Now destroy the current 3D device objects, then reinitialize

		ready = false;

		// Release display objects, so a new device can be created
		device.Dispose();
		device = null;

		// Create the new device
		try {
			InitializeEnvironment();
		}
		catch (SampleException e) {
			HandleSampleException(e,ApplicationMessage.ApplicationMustExit);
		}
		catch {
			HandleSampleException(new SampleException(),ApplicationMessage.ApplicationMustExit);
		}
		ready = true;
	}




	/// <summary>
	/// Called when our sample has nothing else to do, and it's time to render
	/// </summary>
	private void FullRender() {
		// Render a frame during idle time (no messages are waiting)
		if (active && ready) {
			try {
				if ((deviceLost) || (System.Windows.Forms.Form.ActiveForm != this)) {
					// Yield some CPU time to other processes
					System.Threading.Thread.Sleep(100); // 100 milliseconds
				}
				// Render a frame during idle time
				if (active) {
					Render3DEnvironment();
				}
			}
			catch (Exception ee) {
				System.Windows.Forms.MessageBox.Show("An exception has occurred.  This sample must exit.\r\n\r\n" + ee.ToString(), "Exception", 
					System.Windows.Forms.MessageBoxButtons.OK, System.Windows.Forms.MessageBoxIcon.Information);
				this.Close();
			}
		}
	}




	/// <summary>
	/// Run the simulation
	/// </summary>
	public void Run() {
		// Now we're ready to recieve and process Windows messages.
		System.Windows.Forms.Control mainWindow = this;

		// If the render target is a form and *not* this form, use that form instead,
		// otherwise, use the main form.
		if ((ourRenderTarget is System.Windows.Forms.Form) && (ourRenderTarget != this))
			mainWindow = ourRenderTarget;

		mainWindow.Show();
		while (mainWindow.Created) {
			FullRender();
			System.Windows.Forms.Application.DoEvents();
		}
	}




	/// <summary>
	/// Draws the scene 
	/// </summary>
	public void Render3DEnvironment() {
		if (deviceLost) {
			try {
				// Test the cooperative level to see if it's okay to render
				device.TestCooperativeLevel();
			}
			catch (DeviceLostException) {
				// If the device was lost, do not render until we get it back
				isHandlingSizeChanges = false;
				isWindowActive = false;
				return;
			}
			catch (DeviceNotResetException) {
				// Check if the device needs to be resized.

				// If we are windowed, read the desktop mode and use the same format for
				// the back buffer
				if (windowed) {
					GraphicsAdapterInfo adapterInfo = graphicsSettings.AdapterInfo;
					graphicsSettings.WindowedDisplayMode = Manager.Adapters[adapterInfo.AdapterOrdinal].CurrentDisplayMode;
					presentParams.BackBufferFormat = graphicsSettings.WindowedDisplayMode.Format;
				}

				// Reset the device and resize it
				device.Reset(device.PresentationParameters);
				EnvironmentResized(device, new System.ComponentModel.CancelEventArgs());
			}
			deviceLost = false;
		}

		// Get the app's time, in seconds. Skip rendering if no time elapsed
		float fAppTime        = DXUtil.Timer(DirectXTimer.GetApplicationTime);
		float fElapsedAppTime = DXUtil.Timer(DirectXTimer.GetElapsedTime);
		if ((0.0f == fElapsedAppTime) && frameMoving)
			return;

		// FrameMove (animate) the scene
		if (frameMoving || singleStep) {
			// Store the time for the app
			appTime        = fAppTime;
			elapsedTime = fElapsedAppTime;

			// Frame move the scene
			FrameMove();

			singleStep = false;
		}

		// Render the scene as normal
		Render();

		UpdateStats();

		try {
			// Show the frame on the primary surface.
			device.Present();
		}
		catch(DeviceLostException) {
			deviceLost = true;
		}
	}




	/// <summary>
	/// Update the various statistics the simulation keeps track of
	/// </summary>
	public void UpdateStats() {
		// Keep track of the frame count
		float time = DXUtil.Timer(DirectXTimer.GetAbsoluteTime);
		++frames;

		// Update the scene stats once per second
		if (time - lastTime > 1.0f) {
			framePerSecond    = frames / (time - lastTime);
			lastTime = time;
			frames  = 0;

			string strFmt;
			Format fmtAdapter = graphicsSettings.DisplayMode.Format;
			if (fmtAdapter == device.PresentationParameters.BackBufferFormat) {
				strFmt = fmtAdapter.ToString();
			}
			else {
				strFmt = String.Format("backbuf {0}, adapter {1}", 
					device.PresentationParameters.BackBufferFormat.ToString(), fmtAdapter.ToString());
			}

			string strDepthFmt;
			if (enumerationSettings.AppUsesDepthBuffer) {
				strDepthFmt = String.Format(" ({0})", 
					graphicsSettings.DepthStencilBufferFormat.ToString());
			}
			else {
				// No depth buffer
				strDepthFmt = "";
			}

			string strMultiSample;
			switch (graphicsSettings.MultisampleType) {
				case Direct3D.MultiSampleType.NonMaskable: strMultiSample = " (NonMaskable Multisample)"; break;
				case Direct3D.MultiSampleType.TwoSamples: strMultiSample = " (2x Multisample)"; break;
				case Direct3D.MultiSampleType.ThreeSamples: strMultiSample = " (3x Multisample)"; break;
				case Direct3D.MultiSampleType.FourSamples: strMultiSample = " (4x Multisample)"; break;
				case Direct3D.MultiSampleType.FiveSamples: strMultiSample = " (5x Multisample)"; break;
				case Direct3D.MultiSampleType.SixSamples: strMultiSample = " (6x Multisample)"; break;
				case Direct3D.MultiSampleType.SevenSamples: strMultiSample = " (7x Multisample)"; break;
				case Direct3D.MultiSampleType.EightSamples: strMultiSample = " (8x Multisample)"; break;
				case Direct3D.MultiSampleType.NineSamples: strMultiSample = " (9x Multisample)"; break;
				case Direct3D.MultiSampleType.TenSamples: strMultiSample = " (10x Multisample)"; break;
				case Direct3D.MultiSampleType.ElevenSamples: strMultiSample = " (11x Multisample)"; break;
				case Direct3D.MultiSampleType.TwelveSamples: strMultiSample = " (12x Multisample)"; break;
				case Direct3D.MultiSampleType.ThirteenSamples: strMultiSample = " (13x Multisample)"; break;
				case Direct3D.MultiSampleType.FourteenSamples: strMultiSample = " (14x Multisample)"; break;
				case Direct3D.MultiSampleType.FifteenSamples: strMultiSample = " (15x Multisample)"; break;
				case Direct3D.MultiSampleType.SixteenSamples: strMultiSample = " (16x Multisample)"; break;
				default: strMultiSample = string.Empty; break;
			}
			frameStats = String.Format("{0} fps ({1}x{2}), {3}{4}{5}", framePerSecond.ToString("f2"),
				device.PresentationParameters.BackBufferWidth, device.PresentationParameters.BackBufferHeight, 
				strFmt, strDepthFmt, strMultiSample);
		}
	}




	/// <summary>
	/// Called in to toggle the pause state of the app.
	/// </summary>
	/// <param name="pause">true if the simulation should pause</param>
	public void Pause(bool pause) {
		appPausedCount  += (int)(pause ? +1 : -1);
		ready = ((appPausedCount > 0) ? false : true);

		// Handle the first pause request (of many, nestable pause requests)
		if (pause && (1 == appPausedCount)) {
			// Stop the scene from animating
			if (frameMoving)
				DXUtil.Timer(DirectXTimer.Stop);
		}

		if (0 == appPausedCount) {
			// Restart the timers
			if (frameMoving)
				DXUtil.Timer(DirectXTimer.Start);
		}
	}




	/// <summary>
	/// Set our variables to not active and not ready
	/// </summary>
	public void CleanupEnvironment() {
		active = false;
		ready  = false;
		if (device != null)
			device.Dispose();

		device = null;
	}
	#region Menu EventHandlers




	/// <summary>
	/// Prepares the simulation for a new device being selected
	/// </summary>
	public void UserSelectNewDevice(object sender, EventArgs e) {
		// Prompt the user to select a new device or mode
		if (active && ready) {
			Pause(true);
			DoSelectNewDevice();
			Pause(false);
		}
	}




	/// <summary>
	/// Displays a dialog so the user can select a new adapter, device, or
	/// display mode, and then recreates the 3D environment if needed
	/// </summary>
	private void DoSelectNewDevice() {
		isHandlingSizeChanges = false;
		// Can't display dialogs in fullscreen mode
		if (windowed == false) {
			try {
				ToggleFullscreen();
				isHandlingSizeChanges = false;
			}
			catch {
				HandleSampleException(new ResetFailedException(), ApplicationMessage.ApplicationMustExit);
				return;
			}
		}

		// Make sure the main form is in the background
		this.SendToBack();
		D3DSettingsForm settingsForm = new D3DSettingsForm(enumerationSettings, graphicsSettings);
		System.Windows.Forms.DialogResult result = settingsForm.ShowDialog(null);
		if (result != System.Windows.Forms.DialogResult.OK) {
			isHandlingSizeChanges = true;
			return;
		}
		graphicsSettings = settingsForm.settings;

		windowed = graphicsSettings.IsWindowed;

		// Release display objects, so a new device can be created
		device.Dispose();
		device = null;

		// Inform the display class of the change. It will internally
		// re-create valid surfaces, a d3ddevice, etc.
		try {
			InitializeEnvironment();
		}
		catch(SampleException d3de) {
			HandleSampleException(d3de, ApplicationMessage.ApplicationMustExit);
		}
		catch {
			HandleSampleException(new SampleException(), ApplicationMessage.ApplicationMustExit);
		}

		// If the app is paused, trigger the rendering of the current frame
		if (false == frameMoving) {
			singleStep = true;
			DXUtil.Timer(DirectXTimer.Start);
			DXUtil.Timer(DirectXTimer.Stop);
		}
		isHandlingSizeChanges = true;
	}




	/// <summary>
	/// Will start (or stop) simulation
	/// </summary>
	private void ToggleStart(object sender, EventArgs e) {
		//Toggle frame movement
		frameMoving = !frameMoving;
		DXUtil.Timer(frameMoving ? DirectXTimer.Start : DirectXTimer.Stop);
	}




	/// <summary>
	/// Will single step the simulation
	/// </summary>
	private void SingleStep(object sender, EventArgs e) {
		// Single-step frame movement
		if (false == frameMoving)
			DXUtil.Timer(DirectXTimer.Advance);
		else
			DXUtil.Timer(DirectXTimer.Stop);
		frameMoving = false;
		singleStep  = true;
	}




	/// <summary>
	/// Will end the simulation
	/// </summary>
	private void ExitSample(object sender, EventArgs e) {
		this.Close();
	}
	#endregion




	#region WinForms Overrides
	/// <summary>
	/// Clean up any resources
	/// </summary>
	protected override void Dispose(bool disposing) {
		CleanupEnvironment();
		mnuMain.Dispose();
		base.Dispose(disposing);
	}




	/// <summary>
	/// Handle any key presses
	/// </summary>
	protected override void OnKeyPress(System.Windows.Forms.KeyPressEventArgs e) {
		// Check for our shortcut keys (Space)
		if (e.KeyChar == ' ') {
			mnuSingle.PerformClick();
			e.Handled = true;
		}

		// Check for our shortcut keys (Return to start or stop)
		if (e.KeyChar == '\r') {
			mnuGo.PerformClick();
			e.Handled = true;
		}

		// Check for our shortcut keys (Escape to quit)
		if ((byte)e.KeyChar == (byte)(int)System.Windows.Forms.Keys.Escape) {
			mnuExit.PerformClick();
			e.Handled = true;
		}

		// Allow the control to handle the keystroke now
		base.OnKeyPress(e);
	}




	/// <summary>
	/// Handle system keystrokes (ie, alt-enter)
	/// </summary>
	protected override void OnKeyDown(System.Windows.Forms.KeyEventArgs e) {
		if ((e.Alt) && (e.KeyCode == System.Windows.Forms.Keys.Return)) {
			// Toggle the fullscreen/window mode
			if (active && ready) {
				Pause(true);

				try {
					ToggleFullscreen();
					Pause(false);                        
					return;
				}
				catch {
					HandleSampleException(new ResetFailedException(), ApplicationMessage.ApplicationMustExit);
				}
				finally {
					e.Handled = true;
				}
			}
		}
		else if (e.KeyCode == System.Windows.Forms.Keys.F2) {
			DoSelectNewDevice();
		}

		// Allow the control to handle the keystroke now
		base.OnKeyDown(e);
	}




	/// <summary>
	/// Winforms generated code for initializing the form
	/// </summary>
	private void InitializeComponent() {
		// 
		// GraphicsSample
		// 
		this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
		this.MinimumSize = new System.Drawing.Size(100, 100);
		#region MenuCreation/etc
		this.mnuMain = new System.Windows.Forms.MainMenu();
		this.mnuFile = new System.Windows.Forms.MenuItem();
		this.mnuGo = new System.Windows.Forms.MenuItem();
		this.mnuSingle = new System.Windows.Forms.MenuItem();
		this.mnuBreak1 = new System.Windows.Forms.MenuItem();
		this.mnuChange = new System.Windows.Forms.MenuItem();
		this.mnuBreak2 = new System.Windows.Forms.MenuItem();
		this.mnuExit = new System.Windows.Forms.MenuItem();
		// 
		// mainMenu1
		// 
		this.mnuMain.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																				this.mnuFile});
		// 
		// mnuFile
		// 
		this.mnuFile.Index = 0;
		this.mnuFile.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																				this.mnuGo,
																				this.mnuSingle,
																				this.mnuBreak1,
																				this.mnuChange,
																				this.mnuBreak2,
																				this.mnuExit});
		this.mnuFile.Text = "&File";
		// 
		// mnuGo
		// 
		this.mnuGo.Index = 0;
		this.mnuGo.Text = "&Go/stop\tEnter";
		this.mnuGo.Click += new System.EventHandler(this.ToggleStart);
		// 
		// mnuSingle
		// 
		this.mnuSingle.Index = 1;
		this.mnuSingle.Text = "&Single step\tSpace";
		this.mnuSingle.Click += new System.EventHandler(this.SingleStep);
		// 
		// mnuBreak1
		// 
		this.mnuBreak1.Index = 2;
		this.mnuBreak1.Text = "-";
		// 
		// mnuChange
		// 
		this.mnuChange.Index = 3;
		this.mnuChange.Shortcut = System.Windows.Forms.Shortcut.F2;
		this.mnuChange.ShowShortcut = true;
		this.mnuChange.Text = "&Change Device...";
		this.mnuChange.Click += new System.EventHandler(this.UserSelectNewDevice);
		// 
		// mnuBreak2
		// 
		this.mnuBreak2.Index = 4;
		this.mnuBreak2.Text = "-";
		// 
		// mnuExit
		// 
		this.mnuExit.Index = 5;
		this.mnuExit.Text = "E&xit\tESC";
		this.mnuExit.Click += new System.EventHandler(this.ExitSample);
		#endregion
	}




	/// <summary>
	/// When the menu is starting pause our simulation
	/// </summary>
	protected override void OnMenuStart(System.EventArgs e) {
		Pause(true); // Pause the simulation while the menu starts
	}




	/// <summary>
	/// When the menu is complete our simulation can continue
	/// </summary>
	protected override void OnMenuComplete(System.EventArgs e) {
		Pause(false); // Unpause the simulation 
	}




	/// <summary>
	/// Make sure our graphics cursor (if available) moves with the cursor
	/// </summary>
	protected override void OnMouseMove(System.Windows.Forms.MouseEventArgs e) {
		if ((device != null) && (!device.Disposed)) {
			// Move the D3D cursor
			device.SetCursorPosition(e.X, e.Y, false);
		}
		// Let the control handle the mouse now
		base.OnMouseMove(e);
	}




	/// <summary>
	/// Handle size changed events
	/// </summary>
	protected override void OnSizeChanged(System.EventArgs e) {
		this.OnResize(e);
		base.OnSizeChanged(e);
	}




	/// <summary>
	/// Handle resize events
	/// </summary>
	protected override void OnResize(System.EventArgs e) {
		if (isHandlingSizeChanges) {
			// Are we maximized?
			isMaximized = (this.WindowState == System.Windows.Forms.FormWindowState.Maximized);
			if (!isMaximized) {
				storedSize = this.ClientSize;
				storedLocation = this.Location;
			}
		}
		active = !(this.WindowState == System.Windows.Forms.FormWindowState.Minimized || this.Visible == false);
		base.OnResize(e);
	}




	/// <summary>
	/// Once the form has focus again, we can continue to handle our resize
	/// and resets..
	/// </summary>
	protected override void OnGotFocus(System.EventArgs e) {
		isHandlingSizeChanges = true;
		isWindowActive = true;
		base.OnGotFocus (e);
	}




	/// <summary>
	/// Handle move events
	/// </summary>
	protected override void OnMove(System.EventArgs e) {
		if (isHandlingSizeChanges) {
			storedLocation = this.Location;
		}
		base.OnMove(e);
	}




	/// <summary>
	/// Handle closing event
	/// </summary>
	protected override void OnClosing(System.ComponentModel.CancelEventArgs e) {
		isClosing = true;
		base.OnClosing(e);
	}
	#endregion


}

#region Enums for D3D Applications
/// <summary>
/// Messages that can be used when displaying an error
/// </summary>
public enum ApplicationMessage { 
	None, 
	ApplicationMustExit, 
	WarnSwitchToRef
};
#endregion




#region Various SampleExceptions
/// <summary>
/// The default sample exception type
/// </summary>
public class SampleException : System.ApplicationException {
	/// <summary>
	/// Return information about the exception
	/// </summary>
	public override string Message { 
		get { 
			string strMsg = string.Empty;

			strMsg = "Generic application error. Enable\n";
			strMsg += "debug output for detailed information.";

			return strMsg;
		} 
	}
}




/// <summary>
/// Exception informing user no compatible devices were found
/// </summary>
public class NoCompatibleDevicesException : SampleException {
	/// <summary>
	/// Return information about the exception
	/// </summary>
	public override string Message {
		get {
			string strMsg = string.Empty;
			strMsg = "This sample cannot run in a desktop\n";
			strMsg += "window with the current display settings.\n";
			strMsg += "Please change your desktop settings to a\n";
			strMsg += "16- or 32-bit display mode and re-run this\n";
			strMsg += "sample.";

			return strMsg;
		}
	}
}




/// <summary>
/// An exception for when the ReferenceDevice is null
/// </summary>
public class NullReferenceDeviceException : SampleException {
	/// <summary>
	/// Return information about the exception
	/// </summary>
	public override string Message {
		get {
			string strMsg = string.Empty;
			strMsg = "Warning: Nothing will be rendered.\n";
			strMsg += "The reference rendering device was selected, but your\n";
			strMsg += "computer only has a reduced-functionality reference device\n";
			strMsg += "installed.  Install the DirectX SDK to get the full\n";
			strMsg += "reference device.\n";

			return strMsg;
		}
	}
}




/// <summary>
/// An exception for when reset fails
/// </summary>
public class ResetFailedException : SampleException {
	/// <summary>
	/// Return information about the exception
	/// </summary>
	public override string Message {
		get {
			string strMsg = string.Empty;
			strMsg = "Could not reset the Direct3D device.";

			return strMsg;
		}
	}
}




/// <summary>
/// The exception thrown when media couldn't be found
/// </summary>
public class MediaNotFoundException : SampleException {
	private string mediaFile;
	public MediaNotFoundException(string filename) : base() {
		mediaFile = filename;
	}
	public MediaNotFoundException() : base() {
		mediaFile = string.Empty;
	}


    
    
	/// <summary>
	/// Return information about the exception
	/// </summary>
	public override string Message {
		get {
			string strMsg = string.Empty;
			strMsg = "Could not load required media.";
			if (mediaFile.Length > 0)
				strMsg += string.Format("\r\nFile: {0}", mediaFile);

			return strMsg;
		}
	}
}
#endregion




