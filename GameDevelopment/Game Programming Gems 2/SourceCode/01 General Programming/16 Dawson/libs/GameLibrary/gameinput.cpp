/* Copyright (C) Bruce Dawson, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Bruce Dawson, 2001"
 */
#include "priv_precompiled.h"

#define	WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>

#include "gameinput.h"

#include <vector>
using namespace std;

//#define	USING_MULTIMEDIA_TIMERS

#ifdef	USING_MULTIMEDIA_TIMERS
#pragma comment(lib, "winmm.lib")
#endif

// Don't queue up a ridiculous amount of input data - otherwise the
// behaviour when single stepping in the debugger gets very annoying.
const int k_maxPendingFrames = 10;
const int k_minResolution = 1;

class GetCriticalSection
{
public:
	GetCriticalSection(CRITICAL_SECTION& lock)
		: m_lock(lock)
	{
		EnterCriticalSection(&m_lock);
	}
	~GetCriticalSection()
	{
		LeaveCriticalSection(&m_lock);
	}
private:
	CRITICAL_SECTION&	m_lock;
};

GameInput::GameInput()
{
}

void GameInput::UpdateInput(HWND window, bool* oldState, int xOffset /*= 0*/, int yOffset /*= 0*/)
{
	// To use this from a multi-media timer you need to use DirectInput, because
	// GetKeyboardState returns the state of the keyboard in the current thread,
	// which is dependent on pulling messages from the queue. In a multimedia
	// timer thread, with no input queue, the keyboard state never changes.
	// That is why this is actually called from the main thread.
	uint8_t input[k_numInputs];
	// These functions are not exactly the fastest way to get input. A real
	// game will use DirectInput or some other more efficient method.
	BOOL Result = FALSE;
	Result = GetKeyboardState(input);
	assert(Result);

	for (int i = 0; i < k_numInputs; ++i)
	{
		// Yes, I could just read the input directly into the
		// m_state array, but GetKeyboardState() wants an array
		// of bytes, and I want an array of bools, and I hate
		// casting.
		m_state[i] = (input[i] & 0x80) != 0;
		m_event[i] = m_state[i] && !oldState[i];
		oldState[i] = m_state[i];
	}
	POINT	point;
	GetCursorPos(&point);
	assert(window);
	assert(IsWindow(window));
	ScreenToClient(window, &point);
	m_mouseX = point.x - xOffset;
	m_mouseY = point.y - yOffset;
}

// This class exists so that I can adjust the internal representation
// without forcing a recompile. It's a trade off.
class GameInputManagerImp : public GameInputManager
{
public:
	GameInputManagerImp();
	~GameInputManagerImp();
	// Returns how many items are in the queue - if there is one or more
	// then NextInput() will return true.
	virtual int GetQueueSize();
	// Returns the current input - must call NextInput() at least once
	// before calling this. This function does NOT advance to the next
	// input.
	virtual GameInput& GetCurrentInput();
	// This function advances to the next input. It returns false if there
	// is no more input.
	virtual bool NextInput();

	// This function empties out the recording queue. The frequency is the
	// desired input frequency in Hz. GameInput objects will be added to the
	// input queue at the desired rate, for pealing off with
	// NextInput();
	virtual void StartRecordingInput(HWND hWnd, int frequency);
	// The xOffset and yOffset will be subtracted off of mouse coordinates.
	// This is useful if your active window area does not start at 0, 0.
	virtual void SetOffset(int xOffset, int yOffset);
	virtual void StopRecordingInput();

	// PlayingData() returns true if playing back previously recorded input
	// information. If it returns false then we must be recording information.
	virtual bool PlayingData();
	// Add some application specific data to be stored with this frame's input.
	// Will do nothing when playing back input.
	virtual void AppendCustomData(const char* label, const char* data);
	// Get some application specific data by name. Will return false if
	// none is found or if recording input. vector is an appropriate
	// method of returning variable length data, but it's quite a pain
	// to forward declare.
	virtual bool GetCustomData(const char* label, std::vector& data);

	virtual bool SaveToFile(const char* filename);

	virtual bool LoadFromFile(const char* filename);

	static void CALLBACK timerProc(UINT wTimerID, UINT msg, DWORD dw0, DWORD dw1, DWORD dw2);
private:
	GameInput	m_lastInput;
	HWND		m_window;
	bool		m_oldState[GameInput::k_numInputs];
	int			m_xOffset, m_yOffset;
	MMRESULT	m_timerID;

	// Don't touch m_inputs without first grabbing m_lock!
	vector<GameInput>	m_inputs;
	CRITICAL_SECTION	m_lock;
};

GameInputManagerImp::GameInputManagerImp()
{
#ifdef	USING_MULTIMEDIA_TIMERS
	verify(TIMERR_NOERROR == timeBeginPeriod(k_minResolution));
#endif
	InitializeCriticalSection(&m_lock);
	m_window = 0;
	memset(m_oldState, 0, sizeof(m_oldState));
	m_xOffset = m_yOffset = 0;
	m_timerID = 0;
}

GameInputManagerImp::~GameInputManagerImp()
{
	StopRecordingInput();
#ifdef	USING_MULTIMEDIA_TIMERS
	timeEndPeriod(k_minResolution);
#endif
}

#ifdef	USING_MULTIMEDIA_TIMERS
void CALLBACK GameInputManagerImp::timerProc(UINT wTimerID, UINT msg, DWORD dw0, DWORD dw1, DWORD dw2)
{
	GameInput	input;
	GameInputManagerImp* pThis = (GameInputManagerImp*)dw0;
	GetCriticalSection	lockHolder(pThis->m_lock);
	if (pThis->GetQueueSize() >= k_maxPendingFrames)
		return;	// Too much input already - let's give up.
	// Increment frame counter here?
	input.UpdateInput(pThis->m_window, pThis->m_oldState, pThis->m_xOffset, pThis->m_yOffset);
	pThis->m_inputs.push_back(input);
}
#endif

int GameInputManagerImp::GetQueueSize()
{
	GetCriticalSection	lockHolder(m_lock);
	return m_inputs.size();
}

GameInput& GameInputManagerImp::GetCurrentInput()
{
	return m_lastInput;
}

bool GameInputManagerImp::NextInput()
{
#ifdef	USING_MULTIMEDIA_TIMERS
	GetCriticalSection	lockHolder(m_lock);
	if (m_inputs.empty())
		return false;
	m_lastInput = m_inputs[0];
	// I should be using a queue here. Later.
	m_inputs.erase(m_inputs.begin(), m_inputs.begin() + 1);
	return true;
#else
	m_lastInput.UpdateInput(m_window, m_oldState, m_xOffset, m_yOffset);
	return true;
#endif
}

// The xOffset and yOffset will be subtracted off of mouse coordinates.
// This is useful if your active window area does not start at 0, 0.
void GameInputManagerImp::SetOffset(int xOffset, int yOffset)
{
	m_xOffset = xOffset;
	m_yOffset = yOffset;
}

void GameInputManagerImp::StartRecordingInput(HWND hWnd, int frequency)
{
	assert(!m_window);
	m_window = hWnd;
#ifdef	USING_MULTIMEDIA_TIMERS
	StopRecordingInput();
	assert(frequency);
	assert(hWnd);
	const int delay = 1000 / frequency;
	assert(delay >= k_minResolution);
	m_timerID = timeSetEvent(delay, k_minResolution, timerProc, (unsigned long)this, TIME_PERIODIC);
	assert(m_timerID);
#endif
}

void GameInputManagerImp::StopRecordingInput()
{
#ifdef	USING_MULTIMEDIA_TIMERS
	if (m_timerID)
	{
		timeKillEvent(m_timerID);
		m_timerID = 0;
	}
#endif
	m_window = 0;
}

// PlayingData() returns true if playing back previously recorded input
// information. If it returns false then we must be recording information.
bool GameInputManagerImp::PlayingData()
{
	return false;
}

// Add some application specific data to be stored with this frame's input.
// Will do nothing when playing back input.
void GameInputManagerImp::AppendCustomData(const char* label, const char* data)
{
}

// Get some application specific data by name. Will return false if
// none is found or if recording input. vector is an appropriate
// method of returning variable length data, but it's quite a pain
// to forward declare.
bool GameInputManagerImp::GetCustomData(const char* label, std::vector& data)
{
	return false;
}

bool GameInputManagerImp::SaveToFile(const char* filename)
{
	return false;
}

bool GameInputManagerImp::LoadFromFile(const char* filename)
{
	return false;
}

GameInputManager& GameInputManager::GetSingleton()
{
	static GameInputManagerImp	object;
	return object;
}
