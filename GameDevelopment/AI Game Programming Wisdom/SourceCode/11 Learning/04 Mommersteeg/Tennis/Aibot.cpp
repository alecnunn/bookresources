//----------------------------------------------------------------------------------------------
// Sequential Prediction Demo: The positioning pattern
// 
// Author:  Fri Mommersteeg
// Date:    10-09-2001
// File:    AiBot.cpp
//----------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------
// Include files
//----------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "aibot.h"

//----------------------------------------------------------------------------------------------
// External declarations
//----------------------------------------------------------------------------------------------

extern LPDDS imgRedCross;

//----------------------------------------------------------------------------------------------
// PredictorFactory: Produces a predictor
//----------------------------------------------------------------------------------------------

CTennisPredictor * CAiBot::PredictorFactory() {
	// create string matching predictor
	CTennisPredictor * predictor = new CTennisPredictor;

	// set up string matching predictor
	predictor->Setup(WINDOW_SIZE, 2, MIN_PATTERN_SIZE);

	// return predictor
	return predictor;
}

//----------------------------------------------------------------------------------------------
// SetupAI(): Sets up the AI and registers the tennis ball
//----------------------------------------------------------------------------------------------

void CAiBot::SetupAI(CTennisBall * TennisBall, BOOL bTargeting) {
	pHorizontal = PredictorFactory();
	pVertical   = PredictorFactory();
	
	halfwidth = (field.right-field.left) / 2;
	halfheight = (field.bottom-field.top) / 2;
		
	nBounces = 0;
	this->bTargeting = bTargeting;

	ball = TennisBall;
	ball->RegisterObserver(this);
}

//----------------------------------------------------------------------------------------------
// EnableTargeting(): Enables or disables the bot targeting
//----------------------------------------------------------------------------------------------

void CAiBot::EnableTargeting(BOOL bEnabled) {
	bTargeting = bEnabled;
	if (!bTargeting) {
		rotation = 0;
	}
}

//----------------------------------------------------------------------------------------------
// Notify(): Observer notification handler (here the bot receives notifications from the ball)
//----------------------------------------------------------------------------------------------

BOOL CAiBot::Notify(int msg, DWORD param) {
	if (msg == HIT_GROUND_EVENT) {
		POINT pt;
		ball->GetPosition(pt);
		if (PtInRect(&field, pt)) {
			nBounces++;	
			
			// is this the first time that the ball bounces in this field?
			if (nBounces == 1) {
				// generate next elements for sequences
				int next_horizontal, next_vertical;
				
				// update "horizontal" sequence
				next_horizontal = pt.x > field.left + halfwidth;
				pHorizontal->Update(next_horizontal);
				
				// update "vertical" sequence
				next_vertical   = pt.y > field.top + halfheight;
				pVertical->Update(next_vertical);			
			}
		}
	}
	return TRUE;
}

//----------------------------------------------------------------------------------------------
// OnBallCollision(): Used to determine a new ball destination; overriden from CPaddle
//----------------------------------------------------------------------------------------------

void CAiBot::OnBallCollision() {
	
	int Destination;

	// randomly chooce a destination to play the ball to next time
	RandomPredictor.GetPrediction(Destination);

	BallDestination.x = field.left + OUTBOX_WIDTH + (Destination * (field.right-field.left-2*OUTBOX_WIDTH)) / PADDLE_ANGLES;
	BallDestination.y = (int)py + direction * (field.bottom-field.top);
}

//----------------------------------------------------------------------------------------------
// AdjustPaddleAngle(): Adjust paddle angle so that it is directed towards the ball destination
//----------------------------------------------------------------------------------------------

void CAiBot::AdjustPaddleAngle() {	
	float dy;
	float dist;
	
	dy = ((float)BallDestination.y - py);	
	dist = px + (dy*nx) / ny;

	if ((int)dist < BallDestination.x) {
		Rotate(-direction*ROTATE_SPEED);
	} else {
		Rotate(direction*ROTATE_SPEED);
	}
}

//----------------------------------------------------------------------------------------------
// SeekTarget(): Determines the target position to move the paddle to
//----------------------------------------------------------------------------------------------

void CAiBot::SeekTarget() {

	if (ball->GetOwner() != OWNER_SERVICE || (ball->GetOwner() == OWNER_SERVICE && bHasService)) {
		int Prediction;
		
		// get ball position
		POINT pt;
		ball->GetPosition(pt);

		// if ball is not inside AI field, reset ball bounce count
		if (!PtInRect(&field, pt)) {
			nBounces = 0;
		}

		// is this bot the "owner" of the ball? from this we derive the general direction of the ball
		if (ball->GetOwner() != id) {
			if (nBounces == 0 && !(ball->GetOwner() == OWNER_SERVICE && bHasService)) {
				// interpolate the motion of the ball to where it will hit the ground
				ball->GetBouncePosition(target);					
				heuristic = INTERPOLATION;
			} else {
				// move straightly towards the ball
				target = pt;
				heuristic = TOWARDSBALL;
			}
		} else {
			heuristic = TOWARDSCENTER;

			// perform prediction for horizontal coordinate
			if (pHorizontal->GetPrediction(Prediction)) {

				// get the match size from the predictor
				int indicator = pHorizontal->GetMaxMatchSize();

				// limit the match size to the window size
				indicator = (indicator > WINDOW_SIZE) ? WINDOW_SIZE : indicator;

				// convert 0=left,1=right to -1=left,1=right
				Prediction = Prediction ? 1 : -1;

				// determine position
				target.x = field.left + halfwidth + (Prediction*indicator*halfwidth)/(2*WINDOW_SIZE);				
				heuristic = ANTICIPATION;
			} else {
				target.x = field.left + halfwidth;
			}

			// perform prediction for vertical coordinate
			if (pVertical->GetPrediction(Prediction)) {

				// get the match size from the predictor
				int indicator = pVertical->GetMaxMatchSize();

				// limit the match size to the window size
				indicator = (indicator > WINDOW_SIZE) ? WINDOW_SIZE : indicator;

				// convert 0=up,1=down to -1=up,1=down
				Prediction = Prediction ? 1 : -1;

				// determine position... yes, it's a long calculation
				target.y = field.top + halfheight - direction * halfheight / 2 + (Prediction*indicator*halfheight)/(4*WINDOW_SIZE);				
			} else {
				target.y = field.top + halfheight - direction * halfheight / 2;
			}		
		}
	} else {
		// do not move during service
		target.x = (int)px;
		target.y = (int)py;
	}
}

//----------------------------------------------------------------------------------------------
// AdvanceTarget(): Moves the paddle towards the target position
//----------------------------------------------------------------------------------------------

void CAiBot::AdvanceTarget() {
	// we use an extremely simple method to get to the target position

	long dx, dy;
	dx = target.x - (int)px;
	dy = target.y - (int)py;

	if (abs(dx)>abs(dy)) {
		if (dx<0) {			
			// if we can't move to the horizontal, then move vertical
			if (px<=field.left) {
				if (dy<0) {
					Up();
				} else {
					Down();
				}
			} else {
				Left();
			}
		} else {
			// if we can't move to the horizontal, then move vertical
			if (px>=field.right) {
				if (dy<0) {
					Up();
				} else {
					Down();
				}
			} else {
				Right();
			}
		}
	} else {
		if (dy<0) {
			// if we can't move to the vertical, then move horizontal
			if (py<=field.top) {
				if (dx<0) {
					Left();
				} else {
					Right();
				}
			} else {
				Up();
			}			
		} else if (dy>0) {
			// if we can't move to the vertical, then move horizontal
			if (py>=field.bottom) {
				if (dx<0) {
					Left();
				} else {
					Right();
				}
			} else {
				Down();
			}			
		}
	}
}

//----------------------------------------------------------------------------------------------
// Update(): Updates the status of the bot
//----------------------------------------------------------------------------------------------

void CAiBot::Update() {
	SeekTarget();
	AdvanceTarget();

	if (bTargeting) {
		AdjustPaddleAngle();
	}

	CPaddle::Update();
}

//----------------------------------------------------------------------------------------------
// PaintSequence(): Displays sequence information on the screen
//----------------------------------------------------------------------------------------------

void CAiBot::PaintSequence(HDC dc, int x, int y, char * szA, char * szB, CTennisPredictor * pPredictor, int Width) {
	int nWindowSize;
	int nPatternSize;
	int nPatternStart;
	int nPatternEnd;
	int element;
	int i;

	nWindowSize = pPredictor->GetWindowSize();
	nPatternSize = pPredictor->GetMaxMatchSize();
	nPatternEnd = pPredictor->GetMaxMatchPosition()+1;
	nPatternStart = nPatternEnd-nPatternSize;
	for (i = 0; i<nWindowSize; i++) {		
		if (i >= nPatternStart && i < nPatternEnd) {
			SetTextColor(dc, RGB(255, 255, 0));
		} else if (i == nPatternEnd && nPatternSize >= MIN_PATTERN_SIZE) {
			SetTextColor(dc, RGB(0, 255, 0));		
		} else {
			SetTextColor(dc, RGB(255,255,255));
		}
		element = pPredictor->GetWindowEntry(i);
		if (element == 0) {
			TextOut(dc, x+i*Width,y, szA, 1);
		} else {
			TextOut(dc, x+i*Width,y, szB, 1);
		}
	}
}

//----------------------------------------------------------------------------------------------
// ShowStatistics(): Shows AI statistics on the screen
//----------------------------------------------------------------------------------------------

void CAiBot::ShowStatistics(HDC dc) {
	SetBkColor(dc, RGB(0,0,0));
	SetTextColor(dc, RGB(255,255,255));
	TextOut(dc, 10, 10, "Horizontal Predictor:    ", 25);
	TextOut(dc, 10, 30, "Vertical Predictor:      ", 25);

	PaintSequence(dc, 200, 10, "L", "R", pHorizontal);
	PaintSequence(dc, 200, 30, "A", "B", pVertical);
	
	SetTextColor(dc, RGB(255,255,255));
	TextOut(dc, 10, 80, "Positioning strategy: ", 21);

	switch (heuristic) {
	case ANTICIPATION:
		TextOut(dc, 10, 100, "ANTICIPATION   ", 15);
		break;
	case INTERPOLATION:
		TextOut(dc, 10, 100, "INTERPOLATION  ", 15);
		break;
	case TOWARDSBALL:
		TextOut(dc, 10, 100, "TOWARDS BALL   ", 15);
		break;
	case TOWARDSCENTER:
		TextOut(dc, 10, 100, "TOWARDS CENTER ", 15);	
		break;
	}
}

//----------------------------------------------------------------------------------------------
// Paint(): Paints the AI paddle; overriden from CPaddle
//----------------------------------------------------------------------------------------------

void CAiBot::Paint(LPDDS lpdds) {
	
	// display a red cross in case the bot uses anticipation
	if (heuristic == ANTICIPATION) {
		RECT source, dest;
		GetSurfaceSize(imgRedCross, &source);
		CopyRect(&dest, &source);
		OffsetRect(&dest, target.x - CROSS_WIDTH / 2, target.y - CROSS_HEIGHT / 2);
		Blit(imgRedCross, &source, lpddss, &dest, 0);
	}

	// call parent paint 
	CPaddle::Paint(lpdds);

	// show AI statistics
	HDC dc;
	lpdds->GetDC(&dc);
	ShowStatistics(dc);		
	lpdds->ReleaseDC(dc);
}

//----------------------------------------------------------------------------------------------
// ResetForService(): Resets the AI bot for the service
//----------------------------------------------------------------------------------------------

void CAiBot::ResetForService(BOOL HasService) {
	// call default implementation
	CPaddle::ResetForService(HasService);

	// reset predictors
	if (pHorizontal != NULL) {
		pHorizontal->Reset();
	}
	if (pVertical != NULL) {
		pVertical->Reset();
	}

	bHasService = HasService;
	heuristic = TOWARDSCENTER;

	OnBallCollision();
}
