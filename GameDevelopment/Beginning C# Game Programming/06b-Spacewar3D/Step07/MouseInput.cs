using System;
using System.Windows.Forms;
using Microsoft.DirectX;
using Microsoft.DirectX.DirectInput;



public struct MouseControlValues {
	public int Yaw;
	public int Pitch;
	public bool FireButtonPushed;
	public bool ThrustButtonPushed;
}

	/// <summary>
	/// Summary description for MouseInput.
	/// </summary>
public class MouseInput {
	private Device device = null;
	private MouseControlValues values;
	public MouseControlValues Values {
		get {
			UpdateInput();
			return values;
		}
	}

	public MouseInput(Control parent) {
		// Create our mouse device
		device = new Device(SystemGuid.Mouse);
		device.SetCooperativeLevel(parent, CooperativeLevelFlags.Background | CooperativeLevelFlags.NonExclusive);
		device.Properties.AxisModeAbsolute = false;
		device.Acquire();
	}


	public void UpdateInput() {
		MouseState state = device.CurrentMouseState;
		values.Yaw = state.X;
		values.Pitch = state.Y;

		byte [] buttonStatus = state.GetMouseButtons();
		if (buttonStatus[0]!=0)
			values.FireButtonPushed = true;
		else
			values.FireButtonPushed = false;

		if (buttonStatus[1]!=0)
			values.ThrustButtonPushed = true;
		else
			values.ThrustButtonPushed = false;


	}
}
