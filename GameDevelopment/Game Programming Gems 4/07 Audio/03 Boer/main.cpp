
#include <windows.h>
#include <complex>
#include <iostream>
#include "dsutil.h"
#include "dynavar.h"

using namespace std;

static int LinearToLogVol(double fLevel)
{
	// Clamp the value
	if(fLevel <= 0.0f)
		return DSBVOLUME_MIN;
	else if(fLevel >= 1.0f)
		return 0;
	return (long) (-2000.0 * log10(1.0f / fLevel));
}
/*
static float LogToLinearVol(int iLevel)
{
	// Clamp the value
	if(iLevel <= -9600)
		return 0.0f;
	else if(iLevel >= 0)
		return 1.0f;
	return pow(10, double(iLevel + 2000) / 2000.0f) / 10.0f;
}
*/

// Random number utility function
inline float GetRandom(float fMin, float fMax)
{
	return (float(rand() % RAND_MAX) / float(RAND_MAX)) * (fMax - fMin) + fMin;
}


void main()
{
	cout << "You are listening to two short wind loops, interpolating volume and pitch" << endl
		<< "over time with the use of a dynamic variable utility class." << endl << endl;

	cout << "Please close the window to exit the demo..." << endl;

	srand(timeGetTime());

	CSoundManager mgr;
	mgr.Initialize(::GetDesktopWindow(), DSSCL_PRIORITY);

	CSound* loop1;
	CSound* loop2;

	mgr.Create(
		&loop1, 
		"Windy_Forest_loop_01.wav", 
		DSBCAPS_CTRLVOLUME | 
		DSBCAPS_CTRLFREQUENCY | 
		DSBCAPS_GETCURRENTPOSITION2 | 
		DSBCAPS_LOCSOFTWARE | 
		DSBCAPS_GLOBALFOCUS);
	mgr.Create(
		&loop2, 
		"Windy_Forest_loop_02.wav", 
		DSBCAPS_CTRLVOLUME | 
		DSBCAPS_CTRLFREQUENCY | 
		DSBCAPS_GETCURRENTPOSITION2 | 
		DSBCAPS_LOCSOFTWARE | 
		DSBCAPS_GLOBALFOCUS);

	loop1->Play(0, DSBPLAY_LOOPING);
	loop2->Play(0, DSBPLAY_LOOPING);

	DWORD lastTime = timeGetTime();
	DWORD currentTime;

	DynamicVar volume1;
	DynamicVar volume2;
	DynamicVar pitch1;
	DynamicVar pitch2;

	volume1 = 0.75f;
	volume2 = 0.75f;
	pitch1 = 22050.0f;
	pitch2 = 22050.0f;

	while(loop1->IsSoundPlaying() || loop2->IsSoundPlaying())
	{
		// Calculate delta time to update the dynamic variables
		currentTime = timeGetTime();
		DWORD delta = currentTime - lastTime;
		float secondDelta = float(delta) / 1000.0f;
		
		// Update the dynamic variables
		volume1.update(secondDelta);
		volume2.update(secondDelta);
		pitch1.update(secondDelta);
		pitch2.update(secondDelta);

		// When each dynamic variable hits the target, calculate where
		// it should interpolate to next, and how long it should take
		// to get to its new destination.
		if(volume1.hasReachedTarget())
			volume1.setVar(GetRandom(0.3f, 1.0f), GetRandom(3.0f, 8.0f));
		if(volume2.hasReachedTarget())
			volume2.setVar(GetRandom(0.0f, 1.0f), GetRandom(2.0f, 5.0f));
		if(pitch1.hasReachedTarget())
			pitch1.setVar(GetRandom(16050.0f, 28050.0f), GetRandom(2.0f, 6.0f));
		if(pitch2.hasReachedTarget())
			pitch2.setVar(GetRandom(16050.0f, 28050.0f), GetRandom(2.0f, 6.0f));

		loop1->GetBuffer(0)->SetVolume(LinearToLogVol(volume1));
		loop2->GetBuffer(0)->SetVolume(LinearToLogVol(volume2));
		loop1->GetBuffer(0)->SetFrequency(pitch1);
		loop2->GetBuffer(0)->SetFrequency(pitch2);

		lastTime = currentTime;
	}

}

