/* Copyright (C) Bruce Dawson, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Bruce Dawson, 2001"
 */
#ifndef	GAMEINPUT_H
#define	GAMEINPUT_H

#include <vector>

/*
A GameInput object holds the input for one frame.
*/
class GameInput
{
public:
	enum { k_numInputs = 256 };	// This should be a const int, but VC++ doesn't support that yet.

	// GetKeyEvent returns 1 for a mouse press, -1 for a mouse release,
	// and 0 for no change. If you are interested in mouse down events,
	// be sure to compare for > 0.
	char GetKeyEvent(int keyID) const {return m_event[keyID];}
	// GetKeyPressed() returns true if the specified key
	// is currently down.
	bool GetKeyPressed(int keyID) const {return m_state[keyID];}
	void GetMousePos(int& x, int & y) const {x = m_mouseX; y = m_mouseY;}

	// Helper functions for getting the mouse state and events.
	char GetLeftMouseEvent() const {return GetKeyEvent(VK_LBUTTON);}
	char GetRightMouseEvent() const {return GetKeyEvent(VK_RBUTTON);}
	bool GetLeftMouseState() const {return GetKeyPressed(VK_LBUTTON);}
	bool GetRightMouseState() const {return GetKeyPressed(VK_RBUTTON);}

	GameInput();
private:
	friend class GameInputManagerImp;
	void UpdateInput(HWND window, bool* oldState, int xOffset = 0, int yOffset = 0);
	int		m_mouseX, m_mouseY;
	char	m_event[k_numInputs];
	bool	m_state[k_numInputs];
};

class GameInputManager
{
public:
	// Returns how many items are in the queue - if there is one or more
	// then NextInput() will return true.
	virtual int GetQueueSize() = 0;
	// Returns the current input - must call NextInput() at least once
	// before calling this. This function does NOT advance to the next
	// input.
	virtual GameInput& GetCurrentInput() = 0;
	// This function advances to the next input. It returns false if there
	// is no more input.
	virtual bool NextInput() = 0;

	// This function empties out the recording queue. The frequency is the
	// desired input frequency in Hz. GameInput objects will be added to the
	// input queue at the desired rate, for peeling off with
	// NextInput();
	virtual void StartRecordingInput(HWND hWnd, int frequency) = 0;
	// The xOffset and yOffset will be subtracted off of mouse coordinates.
	// This is useful if your active window area does not start at 0, 0.
	virtual void SetOffset(int xOffset, int yOffset) = 0;
	virtual void StopRecordingInput() = 0;


	// PlayingData() returns true if playing back previously recorded input
	// information. If it returns false then we must be recording information.
	virtual bool PlayingData() = 0;
	// Add some application specific data to be stored with this frame's input.
	// Will do nothing when playing back input.
	virtual void AppendCustomData(const char* label, const char* data) = 0;
	// Get some application specific data by name. Will return false if
	// none is found or if recording input. vector is an appropriate
	// method of returning variable length data, but it's quite a pain
	// to forward declare.
	virtual bool GetCustomData(const char* label, std::vector& data) = 0;

	virtual bool SaveToFile(const char* filename) = 0;
	virtual bool LoadFromFile(const char* filename) = 0;

	static GameInputManager& GetSingleton();
protected:
	GameInputManager() {}
	virtual ~GameInputManager() {}
};

#endif	// GAMEINPUT_H
