/* Copyright (C) Ian Lewis, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Ian Lewis, 2001"
 */
// Mixer.cpp: implementation of the CMixer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LowLevelAPI.h"
#include "Mixer.h"

#include "WavePtr.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CMixer::channel::channel():
	pInput(NULL),//input of NULL==not playing
	gain(0.f),
	pan(.5f)//pan of .5==center
{
}

CMixer::CMixer(int n)
{
	//IMPORTANT: this allocates memory for the channel POINTERS,
	//not for the channels themselves. Because subclasses of CMixer 
	//may want to use their own subclasses of CMixer::channel,
	//initialization of these pointers is left to the subclass
	//constructors. Don't forget to init these pointers in
	//your subclass!
	channels.resize(n);
}

CMixer::~CMixer()
{

}

CMixer::channel* CMixer::allocateChannel(const CWaveDesc& sourceAudioType)
{
	std::vector<CMixer::channel*>::iterator i;
	CMixer::channel* pLowestPriorityChannel=NULL;

	//The channel allocation algorithm is straightforward:
	//find a channel that 1) can support this type of 
	//sound, and 2) is either a) not playing or b) playing a sound
	//that has finished or c) playing a sound with a lower priority
	for(i=channels.begin();i!=channels.end();i++)
	{
		CMixer::channel* pChannel=*i;
		if(pChannel->canPlay(sourceAudioType))
		{
			CWavePtr* pIn=pChannel->getInput();
			if(pIn==NULL)
				return pChannel;
			else if(pIn->finished())
				return pChannel;
			else if(pIn->getDescription().priority < sourceAudioType.priority)
			{
				//if we find a low priority channel, don't return just
				//yet--we may still find a channel that is not playing
				//or one with even lower priority.
				if(pLowestPriorityChannel==NULL ||
					pIn->getDescription().priority <
					pLowestPriorityChannel->getInput()->getDescription().priority)
				{
					pLowestPriorityChannel=pChannel;
				}
			}
		}
	}
	//lowestPriorityChannel will be NULL if no such channel
	//was found--that's what we want because a NULL return from
	//this function means no channel available.
	return pLowestPriorityChannel;
}

void CMixer::releaseChannel(CMixer::channel *p)
{
	//check whether p is really a valid channel for
	//this mixer; if it is, set its input to NULL
	std::vector<CMixer::channel*>::iterator i;
	for(i=channels.begin();i!=channels.end();i++)
	{
		if(*i==p)
			p->setInput(NULL);
	}	
}

