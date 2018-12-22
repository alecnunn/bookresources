using System;
using System.Drawing;
using System.Windows.Forms;
using Microsoft.DirectX;
using Microsoft.DirectX.DirectInput;

/// <summary>
/// This class is where the DirectInput routines
/// for the application resides.
/// </summary>
public class InputClass
{

    private const byte msgUp = 0;
    private const byte msgDown = 1;
    private const byte msgLeft = 2;
    private const byte msgRight = 3;
    private const byte msgCancelUp = 4;
    private const byte msgCancelDown = 5;
    private const byte msgCancelLeft = 6;
    private const byte msgCancelRight = 7;
    private bool pressedUp = false;
    private bool pressedDown = false;
    private bool pressedLeft = false;
    private bool pressedRight = false;

    private Control owner = null;
    private Device localDevice = null;
    private PlayClass play = null;

    public InputClass(Control owner, PlayClass play)
    {
        this.owner = owner;
        this.play = play;

        localDevice = new Device(SystemGuid.Keyboard);
        localDevice.SetDataFormat(DeviceDataFormat.Keyboard);
        localDevice.SetCooperativeLevel(owner, CooperativeLevelFlags.Foreground | CooperativeLevelFlags.NonExclusive);         
    }
    
    public Point GetInputState()
    {
        KeyboardState state = null;
        Point p = new Point(0);

        try
        {
            state = localDevice.GetCurrentKeyboardState();
        }
        catch(InputException)
        {
            do
            {
                Application.DoEvents();
                try{ localDevice.Acquire(); }
                catch (InputLostException)
                { continue; }
                catch(OtherApplicationHasPriorityException)
                { continue; }

                break;

            }while( true );
        }

        if(null == state)
            return p;

        if(state[Key.Down]) 
        {
            pressedDown = true;
            play.WriteMessage(msgDown);
        }
        else if (pressedDown == true)
        {
            pressedDown = false;
            play.WriteMessage(msgCancelDown);
        }
        if(state[Key.Up])
        {
            pressedUp = true;
            play.WriteMessage(msgUp);
        }
        else if (pressedUp == true)
        {
            pressedUp = false;
            play.WriteMessage(msgCancelUp);
        }
        if(state[Key.Left]) 
        {
            pressedLeft = true;
            play.WriteMessage(msgLeft);
        }
        else if (pressedLeft == true)
        {
            pressedLeft = false;
            play.WriteMessage(msgCancelLeft);
        }
        if(state[Key.Right]) 
        {
            pressedRight  = true;
            play.WriteMessage(msgRight);
        }
        else if (pressedRight == true)
        {
            pressedRight = false;
            play.WriteMessage(msgCancelRight);
        }

        return p;
    }
}