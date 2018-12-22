using System;
using System.Drawing;
using System.Windows.Forms;
using Microsoft.DirectX;
using Microsoft.DirectX.Direct3D;
using Direct3D=Microsoft.DirectX.Direct3D;

public delegate void MessageDelegate(byte message); // Delegate for messages arriving via DirectPlay.
public delegate void AudioDelegate(); // Delegate to handle audio playback.
public delegate void PeerCloseCallback();           // This delegate will be called when the session terminated event is fired.

public class GraphicsClass : GraphicsSample
{
    private GraphicsFont drawingFont = null;
    private Point destination = new Point(0, 0);
    private InputClass input = null;

    private PlayClass play = null;

    private const byte msgUp = 0;
    private const byte msgDown = 1;
    private const byte msgLeft = 2;
    private const byte msgRight = 3;
    private const byte msgCancelUp = 4;
    private const byte msgCancelDown = 5;
    private const byte msgCancelLeft = 6;
    private const byte msgCancelRight = 7;

    public GraphicsClass()
    {            
        this.Text = "Step1";
         play = new PlayClass(this);
        input = new InputClass(this, play);
        drawingFont = new GraphicsFont( "Arial", System.Drawing.FontStyle.Bold );
    }


    /// <summary>
    /// Called once per frame, the call is the entry point for 3d rendering. This 
    /// function sets up render states, clears the viewport, and renders the scene.
    /// </summary>
    protected override void Render()
    {

        input.GetInputState();

        //Clear the backbuffer to a Blue color 
        device.Clear(ClearFlags.Target | ClearFlags.ZBuffer, Color.Blue, 1.0f, 0);
        //Begin the scene
        device.BeginScene();
        drawingFont.DrawText(5, 5, Color.White, "X: " + destination.X + " Y: " + destination.Y);

        //
        // TODO: Insert application rendering code here.
        //
        device.EndScene();
    }

    /// <summary>
    /// Initialize scene objects.
    /// </summary>
    protected override void InitializeDeviceObjects()
    {
        drawingFont.InitializeDeviceObjects(device);

        //
        // TODO: Insert application device initialization code here.
        //
    }

    /// <summary>
    /// Called when a device needs to be restored.
    /// </summary>
    protected override void RestoreDeviceObjects(System.Object sender, System.EventArgs e)
    {
        //
        // TODO: Insert application device restoration code here.
        //
    }
    protected override void Dispose(bool disposing)
    {
        play.Dispose();
        base.Dispose(disposing);
    }
    public void MessageArrived(byte message)
    {
        switch (message)
        {
            case msgUp:
            {
                destination.X = 1;
                break;
            }
            case msgDown:
            {
                destination.X = -1;
                break;
            }
            case msgLeft:
            {
                destination.Y = 1;
                break;
            }
            case msgRight:
            {
                destination.Y = -1;
                break;
            }
            case msgCancelUp:
            case msgCancelDown:
            {
                destination.X = 0;
                break;
            }
            case msgCancelLeft:
            case msgCancelRight:
            {
                destination.Y = 0;
                break;
            }
        }
    }

    /// <summary>
    // When the peer closes, the code here is executed.
    /// </summary>
    public void PeerClose()
    {
        // The session was terminated, go ahead and shut down
        this.Dispose();
    }
}