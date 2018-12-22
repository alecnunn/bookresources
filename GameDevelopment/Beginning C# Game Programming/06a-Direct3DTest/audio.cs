using System;
using System.Windows.Forms;
using Microsoft.DirectX.AudioVideoPlayback;
using Microsoft.DirectX.DirectSound;
using Buffer = Microsoft.DirectX.DirectSound.Buffer;

/// <summary>
/// Summary description for audio.
/// </summary>
public class AudioClass
{
    Device localDevice = null;
    Buffer localBuffer  = null;
    Audio audioPlayer = null;

    public AudioClass(Control owner)
    {
        BufferDescription desc = new BufferDescription();

        localDevice = new Device();
        localDevice.SetCooperativeLevel(owner, CooperativeLevel.Normal);
        
        localBuffer = new Buffer( DXUtil.FindMediaFile(null, "drumpad-bass_drum.wav") , desc, localDevice);         
        audioPlayer = new Audio(DXUtil.FindMediaFile(null, "DirectX Theme.wma"));
    }

    public void PlaySound()
    {
        localBuffer.Play(0, BufferPlayFlags.Default);
    }

    public void PlayAudio()
    {
        audioPlayer.Stop();
        audioPlayer.Play();
    }
}
