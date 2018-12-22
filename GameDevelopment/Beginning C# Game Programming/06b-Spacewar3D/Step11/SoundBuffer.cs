using System;
using Microsoft.DirectX.DirectSound;


	/// <summary>
	/// A single sound buffer for use by the SoundHandler class.
	/// </summary>
public class SoundBuffer
{
	Microsoft.DirectX.DirectSound.Buffer buffer;
	Sounds thisSound;
	bool looping;
	bool lastValue;

	public SoundBuffer(Microsoft.DirectX.DirectSound.Device soundDevice, string filename, Sounds thisSound, bool looping)
	{
		this.thisSound = thisSound;
		this.looping = looping;

		BufferDescription bufferDesc = new BufferDescription();
		bufferDesc.Flags = BufferDescriptionFlags.ControlVolume;

		try
		{
			buffer = new Microsoft.DirectX.DirectSound.Buffer(
				filename, 
				bufferDesc,
				soundDevice);
		}
		catch (Exception e)
		{
			throw new Exception(
				String.Format("Error opening {0}; ",
				filename), e);
		}
	}

	public Sounds Sound
	{
		get
		{
			return thisSound;
		}
	}

	public int Volume
	{ 
		get 
		{ 
			return buffer.Volume;
		} 
		set 
		{ 
			buffer.Volume = value;
		} 
	}

	public void Play(bool on)
	{
		// looping sounds don't get restarted
		if (looping)
		{
			if (on)
			{
				if (!lastValue)
				{
					buffer.SetCurrentPosition(1000);
					buffer.Play(0,BufferPlayFlags.Looping);
				}
			}
			else
			{
				buffer.Stop();
			}
			lastValue = on;
		}
		else
		{
			if (on)
			{
				buffer.SetCurrentPosition(0);
				buffer.Play(0,BufferPlayFlags.Default);
			}
		}
	}
}

