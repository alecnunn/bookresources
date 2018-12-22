using System;
using System.Drawing;
using System.Windows.Forms;
using Microsoft.DirectX;
using Microsoft.DirectX.DirectInput;

/// <summary>
/// This class is where the DirectInput routines
/// for the application resides.
/// </summary>
public class InputClass {

	private Control owner = null;
	private Device localDevice = null;

	public InputClass(Control owner) {
		this.owner = owner;

		localDevice = new Device(SystemGuid.Keyboard);
		localDevice.SetDataFormat(DeviceDataFormat.Keyboard);
		localDevice.SetCooperativeLevel(owner, CooperativeLevelFlags.Foreground | CooperativeLevelFlags.NonExclusive);         
	}
    
	public KeyboardState GetInputState() {
		KeyboardState kbState = null;

		try {
			kbState = localDevice.GetCurrentKeyboardState();
		}
		catch(InputException) {
			do {
				Application.DoEvents();
				try{ localDevice.Acquire(); }
				catch (InputLostException) {
				  continue; }
				catch(OtherApplicationHasPriorityException) {
				  continue; }

				break;

			}while( true );
		}
		return kbState;
	}
}