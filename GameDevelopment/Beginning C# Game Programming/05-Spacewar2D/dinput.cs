using System;
using System.Drawing;
using System.Windows.Forms;
using System.Threading;
using Microsoft.DirectX;
using Microsoft.DirectX.DirectInput;


namespace SpaceWar {


	/// <summary>
	/// This is where we initialize DirectInput.  The actual key testing is being done in the GameClass HandleKeys() funtion,
	/// after getting the current keyboard state from this class.
	/// </summary>
	/// 
	public class InputClass {
		private Control owner = null;
		private Device localDevice = null;

		public InputClass(Control owner) {
			this.owner = owner;

			localDevice = new Device(SystemGuid.Keyboard);
			localDevice.SetDataFormat(DeviceDataFormat.Keyboard);
			localDevice.SetCooperativeLevel(owner, CooperativeLevelFlags.Foreground | CooperativeLevelFlags.NonExclusive);         
		}

		public KeyboardState GetKBState() {
			KeyboardState state = null;
			try {
				state = localDevice.GetCurrentKeyboardState();
			}
			catch(InputException) {
				do {
					Application.DoEvents();
					try{ localDevice.Acquire(); }
					catch (InputLostException) { 
						Thread.Sleep(2);
						continue;
					}
					catch(OtherApplicationHasPriorityException) { 
						Thread.Sleep(2);
						continue;
					}

					break;

				}while( true );
			}
			return state;
		}
    
		
	}
}