using System;
using System.Collections;
using System.Drawing;
using System.Windows.Forms;
using System.IO;
using Microsoft.DirectX;
using Microsoft.DirectX.DirectSound;
using Buffer = Microsoft.DirectX.DirectSound.SecondaryBuffer;

namespace SpaceDonuts {
	public class SoundHandler {

		private Device soundDevice = null;
		ArrayList sounds = new ArrayList();
		Sounds lastSound;

		public SoundHandler(Control owner) {
			soundDevice = new Device();
			soundDevice.SetCooperativeLevel(owner, CooperativeLevel.Priority);
			CreateSoundBuffers();
		}
	
		void AddBuffer(string filename, Sounds thisSound, bool looping) {
			sounds.Add(new SoundBuffer(soundDevice,filename,thisSound,looping));
		}

		void CreateSoundBuffers() {
			//Buffers must be created in same order as the enumerated type "Sounds"
			AddBuffer(MediaUtilities.FindFile("shipappear.wav"), Sounds.ShipAppear, false);
			AddBuffer(MediaUtilities.FindFile("shield.wav"), Sounds.ShipShield, false);
			AddBuffer(MediaUtilities.FindFile("gunfire.wav"), Sounds.ShipFire, false);
			AddBuffer(MediaUtilities.FindFile("bangbang.wav"), Sounds.ShipExplode, false);
			AddBuffer(MediaUtilities.FindFile("rev.wav"), Sounds.ShipThrust, false);
			AddBuffer(MediaUtilities.FindFile("skid.wav"), Sounds.ShipBrake, false);
			AddBuffer(MediaUtilities.FindFile("bounce.wav"), Sounds.ShipBounce, false);
			AddBuffer(MediaUtilities.FindFile("hum.wav"), Sounds.ShipHum, true);
			AddBuffer(MediaUtilities.FindFile("level.wav"), Sounds.LevelStart, false);
			AddBuffer(MediaUtilities.FindFile("d_bang.wav"), Sounds.DonutExplode, false);
			AddBuffer(MediaUtilities.FindFile("p_bang.wav"), Sounds.PyramidExplode, false);
			AddBuffer(MediaUtilities.FindFile("d_bang.wav"), Sounds.CubeExplode, false); //should be c_bang
			AddBuffer(MediaUtilities.FindFile("p_bang.wav"), Sounds.SphereExplode, false); //should be s_bang
		}

		public void Play(Sounds soundsToPlay) {
			// check each enum value. If that value is set, play the sound...
			foreach (SoundBuffer buffer in sounds) {
				bool on = ((buffer.Sound & soundsToPlay) != 0);
				buffer.Play(on);
			}
			lastSound = soundsToPlay;
		}
	}
}
