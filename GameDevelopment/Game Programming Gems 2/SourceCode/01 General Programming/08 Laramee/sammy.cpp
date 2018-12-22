/* Copyright (C) François Dominic Laramée, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) François Dominic Laramée, 2001"
 */
#include "SAMMY.h"
#include <iostream>

/****************************************************************
 *
 * Implementation of class SAMMY
 *
 ***************************************************************/

SAMMY::SAMMY( istream & is )
{
	sequences = 0;
	stateToAnimTransitions = 0;
	Load( is );
}


SAMMY::~SAMMY()
{
	Cleanup();
}

void SAMMY::Cleanup()
{
	numAnimSequences = 0;

	if ( sequences )
		delete[] sequences;

	numStateToAnimTransitions = 0;
	if ( stateToAnimTransitions )
		delete[] stateToAnimTransitions;
}


// void SAMMY::Load
// Read a description of the entity's various animation sequences from a text file
void SAMMY::Load( istream & is )
{
	// Make sure that we are not leaking memory!
	Cleanup();

	// Then, allocate memory for the sequence descriptors
	is >> numAnimSequences;
	sequences = new AnimationLoop[ numAnimSequences ];

	// And read the anim sequences
	for( int seq = 0; seq < numAnimSequences; seq++ )
	{
		sequences[ seq ].Load( is );
	}

	// Then, take care of the state-to-anim-sequence transition table similarly
	is >> numStateToAnimTransitions;
	stateToAnimTransitions = new int[ numStateToAnimTransitions ];
	for( seq = 0; seq < numStateToAnimTransitions; seq++ )
	{
		is >> seq;
		is >> stateToAnimTransitions[ seq ];
	}

}


// void SAMMY::Save
// Write down an entity's animation sequence descriptors into a text file
void SAMMY::Save( ostream & os )
{
	os << (int) numAnimSequences << endl;
	for( int seq = 0; seq < numAnimSequences; seq++ )
	{
		sequences[ seq ].Save( os );
	}

	// And now, the state-to-anim transition table
	os << numStateToAnimTransitions << endl;
	for( seq = 0; seq < numStateToAnimTransitions; seq++ )
	{
		os << seq << " " << stateToAnimTransitions[ seq ] << endl;
	}
}


// void SAMMY::UpdateEntityState
// the routine case, when the FSM is not messing with the animation ;-)
void SAMMY::UpdateEntityState( EntitySnapshot * state )
{
	// First, check whether we must simply stay put and do nothing
	if ( state->repeatCurrentAnimFrame )
	{
		state->repeatCurrentAnimFrame--;
		state->soundEffectIndex = SAMMY_NOSFX;
		return;
	}

	// Otherwise, move on to the next keyframe in the current loop,
	// if there is one
	state->currentAnimFrameIndex++;
	AnimationLoop * seq; // a shortcut to avoid re-indexing many times
	seq = &sequences[ state->currentAnimSequence ];
	if ( state->currentAnimFrameIndex >= seq->numFrames )
	{
		// When we reach the end of the loop, two things can happen: either
		// we run through it again, or we move on to another loop
		if ( seq->isRepeating )
			state->currentAnimFrameIndex = 0;
		else
		{
			state->entityState = seq->newEntityStateOnExit;
			state->currentAnimSequence = seq->nextLoop;
			state->currentAnimFrameIndex = 0;
		}
	}

	// Now that we know about the current keyframe, take its data and stuff it into
	// the EntitySnapshot
	AnimationFrame * frm;
	frm = &sequences[ state->currentAnimSequence ].frames[ state->currentAnimFrameIndex ];

	state->currentAnimBitmapIndex = frm->bitmapIndex;
	state->soundEffectIndex = frm->animFrameSFX;
	state->repeatCurrentAnimFrame = frm->animFrameRepeat;
}


// void SAMMY::AlignWithNewFSMState
// re-balance the animation when the FSM forces a state change
void SAMMY::AlignWithNewFSMState( EntitySnapshot * state )
{
	// Here, we assume that state contains the newly valid entityState member
	state->currentAnimSequence = stateToAnimTransitions[ state->entityState ];
	state->currentAnimFrameIndex = 0;

	// Now that we have a valid keyframe, take its data and stuff it into
	// the EntitySnapshot
	AnimationFrame * frm;
	frm = &sequences[ state->currentAnimSequence ].frames[ state->currentAnimFrameIndex ];

	state->currentAnimBitmapIndex = frm->bitmapIndex;
	state->soundEffectIndex = frm->animFrameSFX;
	state->repeatCurrentAnimFrame = frm->animFrameRepeat;
}


/****************************************************************
 *
 * Implementation of AnimationLoop
 *
 ***************************************************************/

AnimationLoop::AnimationLoop( istream & is )
{
	frames = 0;
	Load( is );
}

AnimationLoop::~AnimationLoop()
{
	Cleanup();
}

void AnimationLoop::Cleanup()
{
	numFrames = 0;
	if ( frames )
		delete []frames;
}

void AnimationLoop::Load( istream & is )
{
	Cleanup();

	is >> newEntityStateOnExit;
	is >> isRepeating;
	if ( !isRepeating )
	{
		is >> nextLoop;
	}
	is >> numFrames;
	frames = new AnimationFrame[ numFrames ];
	for( int fr = 0; fr < numFrames; fr++ )
		frames[ fr ].Load( is );
}

void AnimationLoop::Save( ostream & os )
{
	os << "\t" << (int) newEntityStateOnExit << endl;
	os << "\t" << (int) isRepeating << endl;
	if ( !isRepeating )
		os << "\t" << (int) nextLoop << endl;
	os << "\t" << (int) numFrames << endl;
	for( int fr = 0; fr < numFrames; fr++ )
		frames[ fr ].Save( os );
}

/****************************************************************
 *
 * Implementation of AnimationFrame
 *
 ***************************************************************/

AnimationFrame::AnimationFrame( istream & is )
{
	Load( is );
}


AnimationFrame::AnimationFrame()
{
	bitmapIndex = 0;
	animFrameRepeat = 0;
	animFrameSFX = SAMMY::SAMMY_NOSFX;
}

void AnimationFrame::Load( istream & is )
{
	is >> bitmapIndex;
	is >> animFrameRepeat;
	is >> animFrameSFX;
}


void AnimationFrame::Save( ostream & os )
{
	os << "\t\t" << (int) bitmapIndex << " ";
	os << (int) animFrameRepeat << " ";
	os << (int) animFrameSFX << endl;
}