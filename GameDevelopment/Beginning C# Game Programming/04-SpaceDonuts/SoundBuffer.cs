using System;
using System.Configuration;
using Microsoft.DirectX;
using Microsoft.DirectX.DirectSound;
using Buffer = Microsoft.DirectX.DirectSound.SecondaryBuffer;
using Device = Microsoft.DirectX.DirectSound.Device;

namespace SpaceDonuts {
	/// <summary>
	/// A single sound buffer for use by the SoundHandler class.
	/// </summary>
	public class SoundBuffer {
		Buffer buffer;
		Sounds thisSound;
		bool looping;
		bool lastValue;

		public SoundBuffer(Device soundDevice, string filename, Sounds thisSound, bool looping) {
			this.thisSound = thisSound;
			this.looping = looping;

			try {
				buffer = new SecondaryBuffer(filename, soundDevice);
			}
			catch (Exception e) {
				throw new Exception(String.Format("Error opening {0}", filename), e);
			}
		}

		public Sounds Sound {
			get {
				return thisSound;
			}
		}

		public int Volume { 
			get { 
				return buffer.Volume;
			} 
			set { 
				buffer.Volume = value;
			} 
		}

		public void Play(bool onFlag) {
			// looping sounds don't get restarted
			if (looping) {
				if (onFlag) {
					if (!lastValue) {
						buffer.SetCurrentPosition(1000);
						buffer.Play(0,BufferPlayFlags.Looping);
					}
				}
				else {
					buffer.Stop();
				}
				lastValue = onFlag;
			}
			else {
				if (onFlag) {
					buffer.SetCurrentPosition(0);
					buffer.Play(0,BufferPlayFlags.Default);
				}
			}
		}

		public void Stop() {
			if(buffer != null)
				buffer.Stop();
		}
	}
}
