/* Copyright (C) François Dominic Laramée, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) François Dominic Laramée, 2001"
 */
/*****************************************************************
 *
 * SAMMY.h - FSM and audio-visual media handler
 *
 ******************************************************************/

#ifndef SAMMY_H
#define SAMMY_H

#include <iostream>
using namespace std;
#include <windows.h>

/******************************************************************
 *
 * STRUCT EntitySnapshot
 *
 * This struct contains a logical description of the state of a game
 * world entity.  Contains FSM states, current keyframe and sound
 * FX information.
 *
 *******************************************************************/

struct EntitySnapshot
{
	// An index to the Entity's array of keyframes in VRAM; assumes
	// a maximum of 256 keyframes per entity, but this is arbitrary.
	BYTE currentAnimBitmapIndex;	

	// The entity's current 2D Velocity vector
	char velocityX;
	char velocityY;

	// Which animation loop are we in, where in the loop are we, and how
	// much longer should we stick with this current keyframe before moving
	// on to the next? 
	BYTE currentAnimFrameIndex;
	BYTE currentAnimSequence;		
	BYTE repeatCurrentAnimFrame;	

	// The entity's behavioral class state
	BYTE entityState;			

	// An index into the entity's table of sound effects; unless this 
	// is set to a sentinel value, SAMMY will ask DirectSound to play
	// the sound file associated with this index.
	BYTE soundEffectIndex;

};


/*********************************************************************
 *
 * STRUCT AnimationFrame
 *
 * A description of a keyframe in an animation loop.  Several of its 
 * fields match EntitySnapshot's in size and type; keep them
 * in synch.  Also note that this struct is exactly 32 bits long, the
 * Pentium's "perfect size"; if you change it, pad it to a multiple
 * of 32 bits for maximum efficiency!
 *
 *********************************************************************/

struct AnimationFrame
{
	// An index pointing to the bitmap this keyframe refers to, in the
	// entity's VRAM table of bitmaps
	BYTE bitmapIndex;

	// How many FPS to spend on this keyframe; usually 2, but may vary
	// according to how an artist wants to tweak things.
	BYTE animFrameRepeat;

	// Is there a sound file to launch when reaching this keyframe?  Usually, this
	// will only be used at the very beginning or the very end of an anim
	// sequence, but you never know; better keep it general
	BYTE animFrameSFX;

  // Padding to 32 bits
	BYTE nothing;

public:
	// Construction; since there is no dynamic memory allocation here, no need
	// for a destructor
	AnimationFrame();
	AnimationFrame( istream & is );

	// input-output
	void Load( istream & is );
	void Save( ostream & os );
};


/********************************************************************
 *
 * STRUCT AnimationLoop
 *
 * This is an array describing the sequence of keyframes in an
 * animation loop.  Pretty straightforward stuff.  
 *
 ********************************************************************/

struct AnimationLoop
{
	// The list of keyframes forming the sequence
	AnimationFrame * frames;
	BYTE numFrames;

	// Is this animation sequence an endless loop, which can only be
	// interrupted if the Behavioral Class indicates a state change?
	// A good example of this is the "idle" animation loop which all
	// entities are going to need.  
	// If so, great; otherwise, we will need to know which new loop
	// to fire up when this one is over.
	BYTE isRepeating;
	BYTE nextLoop;

	// We may want to automatically change the state of an entity's FSM
	// when we end an animation.  
	BYTE newEntityStateOnExit;

public:
	// Construction and destruction; remember: in C++ a struct is like a class
	// with everything public by default!
	AnimationLoop() : frames( 0 ), numFrames( 0 ), isRepeating( 1 ) {}
	AnimationLoop( istream & is );
	virtual ~AnimationLoop();
	void Cleanup();

	// Input/output
	void Load( istream & is );
	void Save( ostream & os );

};


/********************************************************************
 *
 * CLASS SAMMY
 *
 * This is the main controller object for an entity's routine FSM 
 * operations.  As is, it only handles graphics and sound in a very
 * abstract manner, telling you for example that Sound Effect #3 should
 * be played, or that the entity is currently showing Animation Frame #45
 * taken from such-and-such an animation sequence.  You will need to add
 * your own models and WAV's for it to be useful.
 *
 * Two possible ways to do it:
 * - Use SAMMY as is, i.e., as an abstract controller.  
 *   Do this by adding a SAMMY object to the definition of your Entity
 *   superclass.  An entity would then query SAMMY for information,
 *   look up its own media files and provide the appropriate data to
 *   the renderer or to DirectSound.
 * - Extend SAMMY to own the media files itself.
 *
 * I suggest you pick the first option, which allows you to have 
 * multiple pointers to a single SAMMY object assigned to a bunch of
 * entities which look different (i.e., have different media files)
 * but act the same.  Plus, it's more object-oriented this way.  But
 * it's largely a matter of taste.
 *
 * Note that this version of SAMMY was designed for 2D sprites, and
 * may require substantial work before it can be adapted to 3D objects,
 * depending on your exact data structures.
 *
 *********************************************************************/

class SAMMY
{
	// A table of animation sequences to fire up when the Behavioral
	// Class requires SAMMY to implement a high-level state change.
	int * stateToAnimTransitions;
	int numStateToAnimTransitions;

	// The various animation sequences available for the family of entities
	AnimationLoop * sequences;
	int numAnimSequences;

public:
  // A sentinel value used to mark keyframes as "silent", i.e., no
	// sound effect file should be played when showing the keyframe
	enum { SAMMY_NOSFX };

	// Construction and destruction
  // SAMMY is read in from a text file, thus the istream& parameter.
	// Note that I use an istream& and not an ifstream&;
	// this is partly out of habit, and partly because you may want to
	// test the functionality by passing it info from standard input
	// (i.e., from "cin"), which is not an ifstream
	SAMMY() : sequences( 0 ), numAnimSequences( 0 ), numStateToAnimTransitions( 0 ),
		stateToAnimTransitions( 0 ) {}
	SAMMY( istream & is );
	virtual ~SAMMY();
	void Cleanup();

	// Input-output functions; Save should only be used by SAMMY-editing tools
	void Load( istream & is );
	void Save( ostream & os );

	// Look at an entity's current situation, step it forward by one
	// frame, and update the state information accordingly (i.e., which
	// animation keyframe is now visible, and is there a sound bite to
	// launch?)
	void UpdateEntityState( EntitySnapshot * state );

	// If the Behavioral class has just requested a state change,
	// SAMMY must interrupt its current animation loop and reset
	// the EntitySnapshot accordingly
	void AlignWithNewFSMState( EntitySnapshot * state );
};

#endif
