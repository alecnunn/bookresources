/* Copyright (C) Dante Treglia II, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Dante Treglia II, 2000"
 */
//===========================================================================
//===========================================================================
#ifdef WIN32
#include "win32.h"
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#include "text.h"
#include "cube.h"
#include "def.h"
#include <cmath>

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------
extern char * quotes[55];
extern char help[];

//---------------------------------------------------------------------------
// Globals
//---------------------------------------------------------------------------
TextBox * textBox1, * textBox2, * textBox3;
Cube * cube;
COLOR colors[8] = {
	// White
	{255, 255, 255, 255},
	// Grey
	{128, 128, 128, 255},
	// Red
	{255,   0,   0, 255},
	// Green
	{  0, 255,   0, 255},
	// Blue
	{  0,   0, 255, 255},
	// Purple
	{255,   0, 255, 255},
	// Cyan
	{  0, 255, 255, 255},
	// Yellow
	{255, 255,   0, 255},
};

//---------------------------------------------------------------------------
// Main
//---------------------------------------------------------------------------
void main ( void ) {
	long currTime;
	long nextTime1 = 0;
	long nextTime3 = 0;
	long lastTime = 0;
	int i;
	int colorIndex = 0;
	int count = 0;
	float fps = 0;
	char string[64];
	char * strP = 0;
	
	// Create Text Box
	textBox1 = new TextBox(0, 400, 0, 200);
	textBox1->SetMode(TEXT_DRAW_BACKGROUND | TEXT_DRAW_SPACES | TEXT_WRAP_SPACES);
	textBox2 = new TextBox(0, 140, 400, 480);
	textBox2->SetMode(TEXT_DRAW_BACKGROUND | TEXT_DRAW_BOX);
	textBox3 = new TextBox(200, 640, 400, 480);
	textBox3->SetMode(TEXT_NONE);

	// Create Cube
	cube = new Cube(1, 1, 1);
	strP = help;

	srand(12345);

	// Enable state for cube (state is preserved by text module)
	glClearColor(0.5, 0.5, 0.5, 0.5);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(33.0, 640.0F/480.0F, 1, 300);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0, 0.0, 10.0,
		0.0, 0.0, 0.0,
		0.0, 1.0, 0.0);
	glEnable(GL_DEPTH_TEST);

	while(1)
	{
		// Add quotes
		currTime = GetTickCount();
		if (currTime > nextTime1) {
			textBox1->Printf("\n\n%s", quotes[rand() % 55]);
			nextTime1 = currTime + 2000;
		}
		// Draw Text
		if (currTime > nextTime3) {
			textBox3->Printf("%c", *strP);
			switch (*strP) {
				case '.':
					nextTime3 = currTime + 1500;
				break;
				// Toggle Box
				case 1:
					textBox1->mode ^= TEXT_DRAW_BOX;
					nextTime3 = currTime + 2000;
				break;
				// Increment Colors
				case 2:
					colorIndex = (colorIndex + 1) % 8;
					textBox1->SetColor(colors[colorIndex]);
					nextTime3 = currTime + 400;
				break;
				// Toggle Background
				case 3:
					textBox1->mode ^= TEXT_DRAW_BACKGROUND;
					nextTime3 = currTime + 2000;
				break;
				// Long Wait
				case 4:
					nextTime3 = currTime + 10000;
				break;
				// Type
				default:
					nextTime3 = currTime + (rand() % 50) + 50;
			}
			if (*(++strP) == '\0') strP = help;
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Draw Cube
		glPushMatrix();
		glRotatef(currTime / 500.0F, (float)cos(currTime/1000.0F), 0, (float)sin(currTime/1000.0F));
		cube->DrawPrimitives();
		glPopMatrix();

		// Draw Text
		textBox1->Draw();
		textBox2->Draw();
		textBox3->Draw();

		// FPS
		sprintf(string, "FPS: %0.1f", fps);
		textBox1->Begin();
		textBox1->DrawStr(550, 4, string);
		textBox1->End();

		glFinish();

		WinSwapBuffers();

		// Capture Keyboard
		for (i = 0; i < winKey.downCnt; i++) {
			switch(winKey.down[i]) {
			case 'C':
				colorIndex = (colorIndex + 1) % 8;
				textBox1->SetColor(colors[colorIndex]);
			break;
			case 'X':
				textBox1->Clear();
			break;
			case VK_NEXT:
				textBox1->PageDown();
			break;
			case VK_PRIOR:
				textBox1->PageUp();
			break;
			}
		}

		// FPS
		if (count++ % 100 == 0) {
			fps = 100.0F / ((currTime - lastTime) / 1000.0F);
			lastTime = currTime;
		}
		textBox2->Printf("\n%i", count);
	}
}

char * quotes[55] = {
	"Our lives begin to end the day we become silent about things that matter.\n  --Dr. Martin Luther King Jr. ",
	"If it weren't for the last minute, nothing would get done.\n  --Anonymous ",
	"Education is a progressive discovery of our own ignorance.\n  --Will Durant ",
	"And in knowing that you know nothing, makes you the smartest of all.\n  --Socrates ",
	"The greatest oak was once a little nut who held it's ground.\n  --Unknown ",
	"Nothing happens unless first a dream.\n  --Carl Snaburg ",
	"Rivers know this: there is no hurry.  We shall get there some day.\n  --Pooh's Little Instruction Book ",
	"When you have confidence, you can have allot of fun.  And when you have fun you can do amazing things.\n  --Joe Namath ",
	"Experience is a hard teacher because she gives the test first, the lesson afterward.\n  --Unknown ",
	"Sometimes if you stand on the bottom rail of a bridge and lean over to watch the river slipping away beneath you, you will suddenly know everything that there is to be known.\n  --Pooh's Little Instruction Book ",
	"Do not seek to follow in the footsteps of old men; seek what they sought.\n  --Basho ",
	"Know people for who they are, rather than for what they are.\n  --Anthony J. D'Angelo, The College Blue Book ",
	"If life doesn't offer a game worth playing, then invent a new one.\n  --Anthony J. D'Angelo, The College Blue Book ",
	"Don't fear change, embrace it. --Anthony J. D'Angelo, The College Blue Book ",
	"Promise yourself to live your life as a revolution and not just a process of revolution.\n  --Anthony J. D'Angelo, The College Blue Book ",
	"If you believe everything you read, better not read.\n  --Japanese Proverb ",
	"The most wasted of all days is one without laughter.\n  --E. E. Cummings ",
	"If you can do a half-assed job of anything, you're a one eyed man in the kingdom of the blind.\n  --Kirt Vonnegut Jr. ",
	"Two roads diverged in a wood, and I -- I took the the one less traveled by, and that has made all the difference.\n  --Robert Frost, The Road Not Taken ",
	"Once you've accumulated sufficient knowledge to get by, you're too old to remember it.\n  --Unknown ",
	"Duct tape is like the force.  It has a light side and a dark side, and it holds the universe together.\n  --Carl Zwanzig ",
	"People forget how fast you do a job -- but they remember how well you did it.\n  --Howard W. Newton ",
	"Life is like a box of chocolates... you never know what you are going to get.\n  --Tom Hanks, Forrest Gump ",
	"To solve the problems of today, we must focus on tomorrow.\n  --Erik Nupponen ",
	"Expect everything, and anything seems nothing.  Expect nothing, and anything seems everything.\n  --Samuel Hazo ",
	"Mistakes are the portals of discovery.  -James Joyce ",
	"I am not in this world to live up to other peoples expectations, nor do I feel that the world should live up to mine.\n  --Fritz Perls ",
	"The only thing we have to fear is fear itself -- nameless, unreasoning, and unjustified terror which paralyzes needed efforts to convert retreat into advance.\n  --Franklin D. Roosevelt, First Inaugural Address, 3/4/33 ",
	"We must become the change we want to see.\n  --Mahatma Gandhi ",
	"All that we are is the result of what we have thought.\n  --Buddha ",
	"This is true in everyday life as it is in battle: we are given one life and the decision is ours whether to wait for circumstances to make up our mind, or whether to act, and in acting, to live.\n  --General Omar Bradley ",
	"Every man is the architect of his own future.\n  --Appius Cladius ",
	"life is short, live it up.\n  --Khrushchev ",
	"Honor isn't about making the right choices.  It's about dealing with the consequences.\n  --Midori Koto ",
	"Opportunity is missed by most people because it is dresses in overalls and looks like work.\n  --Thomas Edison ",
	"The future belongs to those who dare.\n  --Unknown ",
	"Life it too important to be taken seriously.\n  --Oscar Wilde ",
	"We gain strength, and courage, and confidence by each experience in which we really stop to look fear in the face... we must do that which we cannot.\n  --Eleanor Roosevelt ",
	"There are people that make things happen, and those who watch what happens, and those who wonder what happened.\n  --Unknown ",
	"Life is like a sewer... what you get out of it depends on what you put in it.\n  --Tom Lehrer ",
	"The greatest way to live with honor in this world is to be what we pretend to be.\n  --Socrates ",
	"We are what we repeatedly do.  Excellence, then, is not an act, but a habit.\n  --Aristotle ",
	"It is a wonder that curiosity survives a formal education.\n  --A. Einstein ",
	"When I get to where I am going, will somebody please tell me where I am?\n  --Gary Dulude ",
	"I'll sleep when I am dead.\n  --Warren Zevon ",
	"All I really need to know... I learned in kindergarten.\n  --Robert Fulghum ",
	"Never doubt that a small, group of thoughtful, committed citizens can change the world.  Indeed it is the only thing that ever has.\n  --Margaret Mead ",
	"If a man does not keep peace with his companions, perhaps it is because he hears a different drummer. Let him step to the music which he hears, however measured or far away.\n  --Henry David Thoreau ",
	"Men are born ignorant, not stupid. They are made stupid by education.\n  --Bertrand russell ",
	"Education is what survives when what has been learned has been forgotten.\n  --B. F. Skinner ",
	"We must not believe the many, who say that only free people ought to be educated, but we should rather believe the philosophers who say that only the educated are free.\n  --Epictetus, Discourses ",
	"Only the educated are free.\n  --Epictetus ",
	"I have never let schooling interfere with my education.\n  --Mark Twain ",
	"Study as if you were going to live forever; live as if you were going to die tomorrow.\n  --Maria Mitchell ",
	"If ever there's a tomorrow when we're not together, there's something you must remember-you're braver than you believe, and stronger than you seem, and smarter than you think.\n  --Christopher Robin ",
};

char help[] = 
"This is a demo of a simple and efficient text module\n\
that allows you to create scrolling text boxes (like\n\
this one). It also provides methods for drawing text\n\
at a specified screen coordinate (such as the FPS\n\
indicator above). The former method uses a printf\n\
function similar to the standard C function. The text\n\
can wrap on each character or word. You can display\n\
a text box\1, or not\1. You can also change the\n\
text color\2\2\2\2\2\2\2\2 and make the background disappear\3\3.\n\
The texture used is a 64 x 128 intensity-alpha map\n\
that is only 16K. It contains the following\n\
characters:\n\n\
ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#$%^&?*-+=_()[]{}<>\\/|'`~:;,.\n\n\
Under the hood, the module is designed to preserve\n\
the current OpenGL state, so, there is no need to\n\
change your states back and forth. This message will\n\
repeat. Enjoy! :-)\4\n\n\n\n\n\n";
