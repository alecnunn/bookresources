using System;
using System.Drawing;
using Microsoft.DirectX;
using Container = System.ComponentModel.Container;
/// <summary>
/// The main windows form for the application.
/// </summary>
public class MainClass{

    private GraphicsClass graphics = null;

    /// <summary>
    // Main entry point of the application.
    /// </summary>
    public static void Main()
    {
        MainClass m = new MainClass();
    }

    public MainClass()
    {
        try
        {
            graphics = new GraphicsClass();
        }
        catch(DirectXException)
        {
            return;
        }
        if( graphics.CreateGraphicsSample() )
            graphics.Run();
    }
}
