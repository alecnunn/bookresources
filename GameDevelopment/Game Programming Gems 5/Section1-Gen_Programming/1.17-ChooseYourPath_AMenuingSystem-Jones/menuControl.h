#pragma once

class menuControl
{
public:
	static enum controlType { NONE=-1, BUTTON=0, STATIC, SLIDER, LIST };
	static enum controlState { ACTIVE=0, DISABLED };
	static enum controlView { VISIBLE=0, HIDDEN };
	
	menuControl(void);
	virtual ~menuControl(void);

	// virtual methods
	virtual void render(void) = 0;

	// public methods
	void setControlXY(int X, int Y) { locX = X; locY = Y; }
	int getType(void) { return type; }
	
	void activateControl(void) { state = controlState::ACTIVE; }
	void disableControl(void) { state = controlState::DISABLED; }
	bool getControlState(void) { return state; }
	
	void showControl(void) { view = controlView::VISIBLE; }
	void hideControl(void) { view = controlView::HIDDEN; }
	bool getControlView(void) { return view; }

protected:	
	// control properties
	int type;				// the type of control
	int locX;				// the X location
	int locY;				// the Y location
	
	bool state;				// whether the control is active
	bool view;				// whether the control is visible	
};
