/* Copyright (C) Ian Lewis, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Ian Lewis, 2001"
 */
// WavePtr.h: interface for the CWavePtr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WAVEPTR_H__E8763579_97C3_4545_80EB_90DECD1D6B34__INCLUDED_)
#define AFX_WAVEPTR_H__E8763579_97C3_4545_80EB_90DECD1D6B34__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WaveDesc.h"

class CWave;
class CAudioBuffer;

/*
CWavePtr
Encapsulates a read pointer in a wave.
Could be subclassed to:
	-	encapsulate a read pointer that spans
		a group of waves, rather than just one
	
	-	buffer data from a wave that is
		streamed from slower memory or
		from secondary storage
	
	-	provide on-demand decompression of
		compressed waveforms

	-	etc...
*/
class CWavePtr  
{
protected:
	CWaveDesc description;//description of the data that can be retrieved
						//from this object. This is not necessarily the
						//same as the description of the wave data
						//that pWave points to, because CWavePtr (or
						//a subclass thereof) might transform the
						//audio in some way--decompression, for example.

	CWave* pWave;//source data
	unsigned int position;//read pointer position, in samples
	bool loop;//if loop is set, wave loops and never finishes
	int priority;
public:
	CWavePtr(CWave* pWave);
	virtual ~CWavePtr();
	
	//access functions
	void setLoop(bool l){loop=l;}
	bool getLoop()const{return loop;}
	const CWaveDesc& getDescription() const {return description;}

	//the one non-constant thing in the description is
	//priority, so make a special function to set that
	void setPriority(int p){description.priority=p;}

	//finished returns true if wave is no longer playing
	bool finished();

	//getNext transfers wave data from pWav into the
	//buffer. The buffer's size determines how many samples
	//are transferred. If the buffer is larger than the remaining
	//wave data and loop is not set, the remaining buffer is
	//filled with zeroes.
	virtual void getNext(CAudioBuffer& buffer);

	//reset simply sets the wave read pointer position to zero.
	//Why is it virtual? Because derived classes may need to
	//reset a decompressor, or do some kind of memory management,
	//or any number of other tasks that can't be anticipated here.
	virtual void reset(){position=0;}
};

#endif // !defined(AFX_WAVEPTR_H__E8763579_97C3_4545_80EB_90DECD1D6B34__INCLUDED_)
