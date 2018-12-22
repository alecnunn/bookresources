using System;
using System.Drawing;
using Microsoft.DirectX;
using Container = System.ComponentModel.Container;


	/// <summary>
	/// The main windows form for the application.
	/// </summary>
public class MainClass {

	private GameClass game = null;
	private SplashScreen splash = null;
	private bool enableNetwork = false;
	public bool EnableNetwork { get { return enableNetwork; } set { enableNetwork = value; } }

	private Size gameFormSize;
	public Size GameFormSize { get { return gameFormSize; } set { gameFormSize = value; } }

	private bool fullScreen = false;
	public bool FullScreen { get { return fullScreen; } set { fullScreen = value; } }

	/// <summary>
	// Main entry point of the application.
	/// </summary>
	public static void Main() {
		MainClass m = new MainClass();
	}

	public MainClass() {

		//display the splash screen and determine network status
		splash = new SplashScreen(this);
		splash.ShowDialog();

		try {
			game = new GameClass(fullScreen, gameFormSize, enableNetwork);
		}
		catch(DirectXException) {
			return;
		}
		if( game.CreateGraphicsSample() )
			game.Run();
	}
}
