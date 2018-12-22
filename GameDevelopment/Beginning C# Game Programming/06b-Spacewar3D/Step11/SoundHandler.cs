using System;
using System.Windows.Forms;
using Microsoft.DirectX.DirectSound;
using System.Collections;


	/// <summary>
	/// This class handles loading and playing our sounds.
	/// </summary>
public class SoundHandler
{
	Device soundDevice;
	ArrayList sounds = new ArrayList();

	Sounds lastSound;

	const int VolumeEngine = -1000;
	const int VolumeOtherShot = -2000;
	const int VolumeHaHa = -3000;


	public SoundHandler(Control owner)
	{
		soundDevice = new Device();
		soundDevice.SetCooperativeLevel(owner,CooperativeLevel.Normal);
		CreateSoundBuffers();
	}
	
	Microsoft.DirectX.DirectSound.Buffer LoadFile(string filename)
	{

		Microsoft.DirectX.DirectSound.BufferDescription bufferDesc = new BufferDescription();
		bufferDesc.Flags = BufferDescriptionFlags.ControlVolume;

		Microsoft.DirectX.DirectSound.Buffer buffer;
		buffer = new Microsoft.DirectX.DirectSound.Buffer(filename, bufferDesc, soundDevice);
		

		return buffer;
	}

	void AddBuffer(string filename, Sounds thisSound, bool looping, int volume)
	{
		SoundBuffer buffer;

		buffer = new SoundBuffer(
			soundDevice,
			filename,
			thisSound,
			looping);
		sounds.Add(buffer);
		buffer.Volume = volume;
	}

	void AddBuffer(string filename, Sounds thisSound, bool looping)
	{
		AddBuffer(filename, thisSound, looping, 0);
	}

	void CreateSoundBuffers()
	{
		AddBuffer(MediaUtilities.FindFile("hypercharge.wav"), Sounds.ShipHyper, false);
		AddBuffer(MediaUtilities.FindFile("sci fi bass.wav"), Sounds.ShipAppear, false);
		AddBuffer(MediaUtilities.FindFile("laser2.wav"), Sounds.ShipFire, false);
		AddBuffer(MediaUtilities.FindFile("explode.wav"), Sounds.ShipExplode, false);

		AddBuffer(MediaUtilities.FindFile("engine.wav"), Sounds.ShipThrust,	true, VolumeEngine);
		AddBuffer(MediaUtilities.FindFile("laser2_other.wav"), Sounds.OtherShipFire, false, VolumeOtherShot);
		AddBuffer(MediaUtilities.FindFile("explode_other.wav"), Sounds.OtherShipExplode, false);
		AddBuffer(MediaUtilities.FindFile("engine_other.wav"), Sounds.OtherShipThrust, true, VolumeEngine);

		AddBuffer(MediaUtilities.FindFile("sci fi bass.wav"), Sounds.OtherShipAppear, false);
		AddBuffer(MediaUtilities.FindFile("haha.wav"), Sounds.Taunt, false, VolumeHaHa);
			
		AddBuffer(MediaUtilities.FindFile("dude_quest1.wav"), Sounds.Dude1, false);
		AddBuffer(MediaUtilities.FindFile("dude_quest2.wav"), Sounds.Dude2, false);
		AddBuffer(MediaUtilities.FindFile("dude_quest3.wav"), Sounds.Dude3, false);
		AddBuffer(MediaUtilities.FindFile("dude_quest4.wav"), Sounds.Dude4, false);
		AddBuffer(MediaUtilities.FindFile("dude_quest5.wav"), Sounds.Dude5, false);
	}

	public void Play(Sounds soundsToPlay)
	{
		// check each enum value. If that value
		// is set, play the sound...
		foreach (SoundBuffer buffer in sounds)
		{
			bool on = ((buffer.Sound & soundsToPlay) != 0);
			buffer.Play(on);

		}
		lastSound = soundsToPlay;
	}
}