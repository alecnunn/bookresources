/* Copyright (C) Ian Lewis, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Ian Lewis, 2001"
 */
// Mixer.h: interface for the CMixer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MIXER_H__D23055A6_576C_4945_BB65_C35F2744B2EC__INCLUDED_)
#define AFX_MIXER_H__D23055A6_576C_4945_BB65_C35F2744B2EC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>

class CWavePtr;
struct CWaveDesc;

/*
CMixer
virtual base for mixer classes. The responsibility of the mixer is
to 1) allocate channels, and 2) keep the channels playing by
updating them regularly.
*/
class CMixer  
{
public:
	/*
	CMixer::channel
	virtual base for all mixer channel
	implementations. Defined as an inner class
	because it is tightly coupled with
	CMixer.
	*/
	class channel
	{
	private:
		//pInput points to the audio the channel is currently playing.
		CWavePtr* pInput;

		//these parameters are fairly universal, so they
		//are provided in the base class channel implementation.
		//Their exact meaning, of course, is determined by
		//the subclass implementations.
		float gain;//assume 0=off, 1=unity
		float pan;//assume 0=hard left, 1=hard right
	public:
		channel();

		//perform any platform-dependent housekeeping
		virtual void tick(time_t t,CMixer* parent)=0;

		//returns true if wave format is compatible
		virtual bool canPlay(const CWaveDesc& soundType)=0;

		//access functions
		void setInput(CWavePtr* p){pInput=p;}
		CWavePtr* getInput(){return pInput;}
		void setGain(float g){gain=g;}
		float getGain() const {return gain;}
		void setPan(float p){pan=p;}
		float getPan() const {return pan;}
	};

protected:
	std::vector<channel*> channels;	

public:
	CMixer(int _nChannels);
	virtual ~CMixer();

	//allocateChannel finds the first free channel and returns
	//a pointer to it, or NULL if no channel was available.
	virtual channel* allocateChannel(const CWaveDesc& sourceAudioType);
	
	//frees the channel pointed to by p for subsequent
	//reallocation.
	virtual void releaseChannel(channel* p);

	virtual void tick(time_t t)=0; //perform housekeeping
};

#endif // !defined(AFX_MIXER_H__D23055A6_576C_4945_BB65_C35F2744B2EC__INCLUDED_)
