/*
	This program and its graphics are copyright (c) 1996 by Ethan Koehler.

	-- See the 'readme.txt' file for info on the appropriate parameters in HTML --

	Please DO NOT modify and redistribute this program under the name "Daleks!"
	without consent of the author.  If you make modifications to my design, please
	give it another name so as not to confuse the two programs.

	The code and the techniques used in Daleks! may otherwise be used freely.
	And the lovely graphics are also fair game.  Share and enjoy.

	Questions or comments?   Send e-mail: 5q89koehlere@vms.csd.mu.edu
*/



import java.awt.*;
import java.applet.*;


public class daleks14 extends java.applet.Applet implements Runnable {
	// program control flags & other stuff
	boolean screwdriverUsed = false;   //  did the player use screwdriver?
	boolean lastStand = false;         //  player making a "last stand"?
	boolean allDead = false;           //  are all the daleks dead?
	boolean animLevel = false;         //  animate a level change?
	boolean animRegen = false;         //  animate a regeneration?
	boolean animIntro = false;         //  is the intro screen running?
	boolean animTele = false;          //  animate teleportation to new location?
	boolean newRubble = false;         //  is there new rubble on the board?
	boolean doneLoading = false;       //  are pictures done loading?
	boolean levelComplete = false;     //  has current level been cleared?
	boolean playerDead = false;        //  is the player dead?

	Thread kicker = null;              //  used to kick off animation thread
	MediaTracker tracker;              //  used to wait until pics are loaded
	int currentPic = 0;                //  current picture being loaded by tracker
	animatedIntroClass animateIntro = new animatedIntroClass(); //  intro screen object

	int numDaleks = 0;                 //  number of daleks on current level
	int levelMultiple = 4;             //  number of daleks added each level
	int baseNumDaleks = 2;             //  minimum number of daleks every level
	int highScore = 150;               //  player's high score this session
	int level = 1;                     //  current level number
	int maxX = 30;                     //  number of "squares" horizontally on board
	int maxY = 20;                     //  number of "squares" vertically
	int screwdrivers = 2;              //  number of sonic screwdrivers remaining
	int score = 0;                     //  score of current game

	//   vars to hold and adjust GIF images
	int imgH = 20;                     //  height in pixels of dalek pictures (from HTML)
	int imgW = 20;                     //  width of dalek pictures (from HTML)
	Image dalekPic[] = new Image[2];   //  left- and right-facing dalek pics
	Image doctorPic[] = new Image[10]; //  left- and right-facing player pics (5 pairs)
	int whichDoc = 0;                  //  which doctor pic should be displayed?  (depends on level)
	Image rubblePic[] = new Image[2];	//  left- and right-facing dalek rubble piles
	Image deadPic;                     //  graphic for "dead" player
	Image digit[] = new Image[10];     //  graphics for digits 0 .. 9
	Image nextScreen;                  //  behind-the-scenes bitmap
	Graphics offScreenGC;              //  "graphics context" of the off-screen bitmap

	int faceLeft = 0;                  //  pseudo-constants
	int faceRight = 1;                 //  "

	// variables holding player position and status
	String title, flyingWidget;		//  strings to hold values from param tags
	int numWidgets = 50;               //  number of flying "widgets" on title screen
	int drStartX = 15;                 //  horizontal starting position for player
	int drStartY = 10;                 //  vertical starting position for player
	int drX;                           //  current player horizontal position
	int drY;                           //  current player vertical position
	int oldX, oldY;                    //  last coordinates occupied by player
	int drF = faceLeft;                //  starting orientation (left or right) of player
	boolean drA = true;                //  is the player still alive?

	// arrays for dalek positions and status
	int dalX[];                        //  dalek X position on board
	int dalY[];                        //  dalek Y position on board
	int dalF[];                        //  facing direction (0=left, 1=right)
	int maxEnemies = 100;              //  maximum number of enemies allowed
	boolean dalR[] = new boolean[100]; //  is this dalek rubbled?
	boolean dalA[] = new boolean[100]; //  is this dalek alive?

	//---------------------------------------------------------------
	//           "daleks" Class Methods
	//---------------------------------------------------------------

	public boolean mouseDown(Event evt, int x, int y)  {
		repaint();
		return false;
		}

	public synchronized boolean keyDown(Event evt, int x)  {
		if (doneLoading)
			handleKeyPress(x);
	                return false;
	                }

	public void handleKeyPress(int whichKey)  {
		if (animIntro)  {	//   if the intro screen is running..
			animIntro = false;
			kicker.stop();	//   stop the intro animation thread
			resetGame();	//   reset the game data
			setupBoard();	//   initialize next level
			repaint();
			}
		else if (levelComplete)  {	//   if this level was cleared..
			oldX = drX;
			oldY = drY;
			animLevel = true;
			repaint();
			setupBoard();
			repaint();
			}
		else if (playerDead)  {	//   player died..
			playerDead = false;
			prepareScreen();
			animateIntro.updateBar(nextScreen);
			animateIntro.startFadeIn();
			animateIntro.start();
			animIntro = true;
			repaint();
			}
		else {	//   player is still alive and daleks remain..
			char ASCIImove = (char)whichKey;
			boolean validMove = movePlayer(ASCIImove);
		
			if (validMove)  {
				if (lastStand)
					repaint();
				else  {
					if (screwdriverUsed)  {
						calcSonic();
						play(getCodeBase(), "sonic.au");
						}
					moveDaleks();
					if (newRubble)
						play(getCodeBase(), "crash.au");
					}
				}
			repaint();
			checkDaleks();
			if ((drA) && (allDead))
				levelComplete = true;
			else if (!drA)  {
//				prepareScreen();	//   daleks may die during player's final move, so....
//				repaint();		//   this ensures the *latest* high score is displayed
				playerDead = true;
				}
			}
		}

	public void getHTMLParameters()  {
		//  read the parameters within the HTML applet tags
		if (getParameter("maxX") != null)
			maxX = Integer.parseInt(getParameter("maxX"));
		if (getParameter("maxY") != null)
			maxY = Integer.parseInt(getParameter("maxY"));
		if (getParameter("imgH") != null)
			imgH = Integer.parseInt(getParameter("imgH"));
		if (getParameter("imgW") != null)
			imgW = Integer.parseInt(getParameter("imgW"));
		if (getParameter("levelMultiple") != null)
			levelMultiple =  Integer.parseInt(getParameter("levelMultiple"));
		if (getParameter("baseNum") != null)
			baseNumDaleks =  Integer.parseInt(getParameter("baseNum"));
		if (getParameter("maxEnemies") != null)
			maxEnemies = Integer.parseInt(getParameter("maxEnemies"));
		if (getParameter("numWidgets") != null)
			numWidgets = Integer.parseInt(getParameter("numWidgets"));
		if (getParameter("flyingWidget") != null)
			flyingWidget = getParameter("flyingWidget");
		else
			flyingWidget = "?";
		if (getParameter("title") != null)
			title = getParameter("title");
		else
			title = "Daleks!";
		}

	public void init()  {
		//  get parameters from the calling HTML file
		getHTMLParameters();

		//  initialize dalek arrays
		dalX = new int[maxEnemies];
		dalY = new int[maxEnemies];
		dalF = new int[maxEnemies];

		drStartX = maxX / 2;
		drStartY = maxY / 2;

		//  create enough room to fit gameboard and status bar
		resize(maxX*imgW,maxY*imgH+25);

		// load game graphics
		doctorPic[0] = getImage(getCodeBase(),"dw1l.gif");
		doctorPic[1] = getImage(getCodeBase(),"dw1r.gif");
		doctorPic[2] = getImage(getCodeBase(),"dw2l.gif");
		doctorPic[3] = doctorPic[2];
		doctorPic[4] = getImage(getCodeBase(),"dw3l.gif");
		doctorPic[5] = doctorPic[4];
		doctorPic[6] = getImage(getCodeBase(),"dw4l.gif");
		doctorPic[7] = getImage(getCodeBase(),"dw4r.gif");
		doctorPic[8] = getImage(getCodeBase(),"dw5l.gif");
		doctorPic[9] = getImage(getCodeBase(),"dw5r.gif");
		dalekPic[0] = getImage(getCodeBase(),"dalekl.gif");
		dalekPic[1] = getImage(getCodeBase(),"dalekr.gif");
		rubblePic[0] = getImage(getCodeBase(),"daldl.gif");
		rubblePic[1] = getImage(getCodeBase(),"daldr.gif");
		deadPic = getImage(getCodeBase(),"drdead.gif");

		//   load graphic digits
		for (int j=0; j<10; j++)
			digit[j] = getImage(getCodeBase(),"green"+j+".gif");

		tracker = new MediaTracker(this);
		for (int j=0; j<10; j++)  {
			tracker.addImage(doctorPic[j], j);
			tracker.addImage(digit[j], j+10);
			}
		for (int j=0; j<2; j++)  {
			tracker.addImage(dalekPic[j], 20+j);
			tracker.addImage(rubblePic[j], 22+j);
			}
		tracker.addImage(deadPic, 24);

		//   start loading images
		tracker.checkAll(true);

		// create the off-screen bitmap
		nextScreen = createImage(maxX*imgW,maxY*imgH+25);
		offScreenGC = nextScreen.getGraphics();

		prepareScreen();

		animateIntro.setInfo(nextScreen, maxX, maxY, imgH, imgW, title, flyingWidget, numWidgets);
		animateIntro.start();
		animIntro = true;
		repaint();
		}

	void resetGame()  {
		level = 0;
		score = 0;
		whichDoc = 0;
		screwdrivers = 1;
		}

	public void setupBoard()  {
		level++;
		levelComplete = false;
		// based on level num, decide which player graphics to use...
		switch (level)  {
			case 1: whichDoc = 0; break;		// doc number 1
			case 6: whichDoc = 2; break;		// doc number 2
			case 7: whichDoc = 4;  break;		// doc number 3
 			case 10: whichDoc = 6; break;	// doc number 4
			case 13: whichDoc = 8; break;	// doc number 5
			}
		screwdrivers += 1;
		if (level > 5)
			screwdrivers +=1;	//  help player out above L5!
		drX = drStartX;
		drY = drStartY;
		drA = true;
		allDead = false;
		animRegen = true;

		numDaleks = level * levelMultiple + baseNumDaleks;
		if (numDaleks > maxEnemies)
			numDaleks = maxEnemies;

		// randomly place daleks and set their initial properties
		for (int j=0; j<numDaleks; j++)  {
			boolean badSpot = true;
			while (badSpot)  {
				badSpot = false;
				dalX[j] = (int)(Math.random()*maxX);
				dalY[j] = (int)(Math.random()*maxY);
				for (int i=0; i<j; i++)
					if ((dalX[i]==dalX[j]) && (dalY[i]==dalY[j]))
						badSpot = true;
				if ((dalX[j]==drX) && (dalY[j]==drY))
					badSpot=true;
				}
			dalA[j] = true;  // dalek starts out alive
			dalR[j] = false; // dalek starts out as NOT rubble
			}
		}

	public void checkFace(int whichDal)  {
		//   face dalek number 'whichDal' to face in the direction of the player
		int temp;
		if (dalX[whichDal] < drX)
			temp = faceRight;
		else if (dalX[whichDal]> drX)
			temp = faceLeft;
		else
			temp = dalF[whichDal];
		dalF[whichDal] = temp;
		}

	public void calcSonic()  {
		//  destroy daleks immediately adjacent to player
		screwdrivers--;
		for (int j=0; j<numDaleks; j++)
			if ((Math.abs(drX-dalX[j])<=1) && (Math.abs(drY-dalY[j])<=1))
				if (dalA[j])  {
					dalA[j] = false;
					dalR[j] = false;
					}
		}

	public void pause(int howLong)  {
		//   this routine should be dynamically calibrated according to processor speed
		//   so that the animation looks the same in all environments, but oh well
		for (int j=0; j<howLong; j++);
		}

	public void animateRegen(Graphics g)  {
		//   this doesn't technically animate a "regeneration" - in this version
		//   the routine is being used at the start of every level
		g.clipRect(0,0,maxX*imgW,maxY*imgH+1);
		g.setColor(Color.white);
		g.fillRect(0,0,maxX*imgW,maxY*imgH+1);

		int distanceToMiddle = (maxX > maxY) ? (int)(maxX/2) + 2 : (int)(maxY/2) + 2;

		for (int j=0; j<distanceToMiddle; j+=1)  {
			int distHori = (j * imgW);
			int distVert = (int)(j * imgH * maxY / maxX);
			g.drawImage(doctorPic[whichDoc+drF],distHori,distVert,this);
			g.drawImage(doctorPic[whichDoc+drF],maxX*imgW-distHori,distVert,this);
			g.drawImage(doctorPic[whichDoc+drF],maxX*imgW-distHori,maxY*imgH-distVert,this);
			g.drawImage(doctorPic[whichDoc+drF],distHori,maxY*imgH-distVert,this);
			distHori = ((j-3)*imgW);
			distVert = (int)((j-3) * imgH * maxY / maxX);
			g.fillRect(distHori, distVert, imgW, imgH);
			g.fillRect(distHori, maxY*imgH-distVert, imgW, imgH);
			g.fillRect(maxX*imgW-distHori, maxY*imgH-distVert, imgW, imgH);
			g.fillRect(maxX*imgW-distHori, distVert, imgW, imgH);
			pause(3000);
			}
		//   erase the doctor pics remaining on screen from above loop
		for (int j=distanceToMiddle-3; j<distanceToMiddle; j++)  {
			int distHori = (j*imgW);
			int distVert = (int)(j * imgH * maxY / maxX);
			g.fillRect(distHori, distVert, imgW, imgH);
			g.fillRect(distHori, maxY*imgH-distVert, imgW, imgH);
			g.fillRect(maxX*imgW-distHori, maxY*imgH-distVert, imgW, imgH);
			g.fillRect(maxX*imgW-distHori, distVert, imgW, imgH);
			pause(4000);
			}
		animRegen = false;
		}

	public void animateTele(Graphics g)  {
		//  animate pairs of gray lines when teleporting
		g.clipRect(0,0,maxX*imgW,maxY*imgH);
		g.setColor(Color.lightGray);
		for (int j=maxX*imgW+8; j>8; j-=8)  {
			g.drawLine(drX*imgW-j,drY*imgH-j,drX*imgW+j,drY*imgH-j);
			g.drawLine(drX*imgW-j,drY*imgH+j,drX*imgW+j,drY*imgH+j);
			pause(800);
			}
		animTele = false;
		}

	public void animateSonic(Graphics g)  {
		//  draw concentric rings when sonic screwdriver is used
		g.clipRect(0,0,maxX*imgW,maxY*imgH);
		g.setColor(Color.blue);
		for (int j=4; j<24; j+=2)  {
			g.drawOval(drX*imgW-j+imgW/2,drY*imgH-j+imgH/2,j*2,j*2);
			pause(2000);
			}
		g.setColor(Color.white);
		screwdriverUsed = false;
		}

	public void animateLevel(Graphics g)  {
		//  clear the screen with rectangles between levels
		g.clipRect(0,0,maxX*imgW,maxY*imgH);
		g.setColor(Color.blue);
		for (int j=0; j<maxX*imgW; j+=3)  {
			g.drawRect(oldX*imgW+imgW/2-j,oldY*imgH+imgH/2-j,j*2,j*2);
			pause(500);
			}
		animLevel = false;
		}

	public synchronized void update(Graphics g)  {
		//   draw between-move animations and repaint the game board, or
		//   call the intro screen's drawing routine
		if (animIntro)  {
			animateIntro.drawIntroScreen(g, doneLoading, currentPic);
			if (!doneLoading)  {
				if (tracker.checkID(currentPic, true))
					currentPic ++;
				if (currentPic > 24)  {
					doneLoading = true;
					prepareScreen();
					}
				}
			repaint();
			}
		else  {
			if (animLevel)  {
				g.drawImage(nextScreen,0,0,this);
				animateLevel(g);
				}
			if (animRegen)
				animateRegen(g);
			if (screwdriverUsed)
				animateSonic(g);
			if (animTele)
				animateTele(g);
			if (lastStand)  {
		                                while ((!allDead) && (drA))  {
					moveDaleks();
					checkDaleks();
					if (newRubble)
						play(getCodeBase(), "crash.au");
					prepareScreen();
					g.drawImage(nextScreen,0,0,this);
					}
				if ((allDead) && (drA))
					levelComplete = true;
				else
					playerDead = true;
				lastStand = false;
				}
			prepareScreen();
			g.drawImage(nextScreen,0,0,this);
			}
		}

	public void paint(Graphics g)  {
		//   repaint the screen as necessary
		if (!animIntro)  {
			prepareScreen();
			g.drawImage(nextScreen,0,0,this);
			}
		else
			animateIntro.drawIntroScreen(g, doneLoading, currentPic);
		}

	public void paintNumber(int num, int xPos, int yPos)  {
		//   draw the number as a graphic at the specified position
		Integer numObj = new Integer(num);
		String stringNum = numObj.toString();
		for (int j=0; j < stringNum.length(); j++)  {
			char charDigit = stringNum.charAt(j);
			int intDigit = Character.digit(charDigit, 10);
			offScreenGC.drawImage(digit[intDigit], xPos + j*12, yPos-13, this);
			}
		}

	public synchronized void prepareScreen()  {
		//   draw the current game status on the off-screen bitmap

		offScreenGC.setColor(Color.white);
		offScreenGC.clipRect(0,0,maxX*imgW,maxY*imgH+25);
		offScreenGC.fillRect(0,0,maxX*imgW,maxY*imgH+4);

		// draw info bar on bottom
		offScreenGC.setColor(Color.darkGray);
		offScreenGC.drawLine(0, maxY*imgH+1, maxX*imgW, maxY*imgH+1);
		offScreenGC.drawLine(0, maxY*imgH+3, maxX*imgW, maxY*imgH+3);
		offScreenGC.setColor(Color.lightGray);
		offScreenGC.drawLine(0, maxY*imgH+2, maxX*imgW, maxY*imgH+2);
		offScreenGC.setColor(Color.black);
		offScreenGC.fillRect(0, maxY*imgH+4, maxX*imgW, 21);
		if (doneLoading)  {
			offScreenGC.setFont(new Font("TimesRoman", Font.PLAIN, 12));
			if (score > highScore)
				highScore = score;
			int yPos = maxY*imgH + 19;
			offScreenGC.setColor(Color.white);
			offScreenGC.drawString("SCORE: ", 5, yPos);
			paintNumber(score, 51, yPos);
			offScreenGC.drawString("SCREWDRIVERS: ", 124, yPos);
			paintNumber(screwdrivers, 221, yPos);
			offScreenGC.drawString("LEVEL: ", 260, yPos);
			paintNumber(level, 303, yPos);
			offScreenGC.drawString("HIGHSCORE: ", 338, yPos);
			paintNumber(highScore, 415, yPos);

			//   draw the daleks
			for (int j=0; j<numDaleks; j++)  {
				if (dalA[j])  {  // if the dalek is alive..
					checkFace(j);
	 				offScreenGC.drawImage(dalekPic[dalF[j]],dalX[j]*imgW,dalY[j]*imgH,this);
					}
				else if (dalR[j])  // if dalek is rubble..
					offScreenGC.drawImage(rubblePic[dalF[j]],dalX[j]*imgW,dalY[j]*imgH,this);
				}

			//   draw the doctor
			if (!playerDead)
				offScreenGC.drawImage(doctorPic[drF+whichDoc],drX*imgW,drY*imgH,this);
			else  {
				offScreenGC.setColor(Color.white);
				offScreenGC.fillRect(drX*imgW, drY*imgH, imgW, imgH);
				offScreenGC.drawImage(deadPic, drX*imgW, drY*imgH, this);
				offScreenGC.setColor(Color.black);
				offScreenGC.setFont(new Font("Helvetica", Font.PLAIN, 36));
				offScreenGC.drawString("GAME OVER",maxX*imgW/2-100,75);
				}
			}
		}

	public void start()  {
		if (kicker == null)  {
			kicker = new Thread(this);
			kicker.start();
			}
		repaint();
		}

	public void stop()  {
		kicker.stop();
		kicker = null;
		}

	public void run()  {
		repaint();
		}

	public boolean movePlayer(char input)  {
		int newX = drX;
		int newY = drY;
		int newF = drF;
		boolean pass = false;	//   player may elect to not move (hit '5')
		boolean teleport = false;	//   player may teleport (hit 'T' or 't')
		boolean validMove = true;
		switch (input)  {
			//   numeric keypad movement options
			case '1': newX--;	newY++;	newF = faceLeft;	break;
			case '2':		newY++;			break;
			case '3': newX++;	newY++;	newF = faceRight;	break;
			case '4': newX--;		newF = faceLeft;	break;
			case '5': pass = true;			break;
			case '6': newX++;		newF = faceRight;	break;
			case '7': newX--;	newY--;	newF = faceLeft;	break;
			case '8':		newY--;			break;
			case '9':	newX++;	newY--;	newF = faceRight;	break;
			//   alternative keys for movement
			case 'N': newX--;	newY++;	newF = faceLeft;	break;
			case 'n': newX--;	newY++;	newF = faceLeft;	break;
			case 'M':		newY++;			break;
			case 'm':		newY++;			break;
			case ',': newX++;	newY++;	newF = faceRight;	break;
			case 'H': newX--;		newF = faceLeft;	break;
			case 'h': newX--;		newF = faceLeft;	break;
			case 'J': pass = true;			break;
			case 'j': pass = true;			break;
			case 'K': newX++;		newF = faceRight;	break;
			case 'k': newX++;		newF = faceRight;	break;
			case 'Y': newX--;	newY--;	newF = faceLeft;	break;
			case 'y': newX--;	newY--;	newF = faceLeft;	break;
			case 'U':		newY--;			break;
			case 'u':		newY--;			break;
			case 'I':	newX++;	newY--;	newF = faceRight;	break;
			case 'i':	newX++;	newY--;	newF = faceRight;	break;
			//   other valid commands
			case 'T':	teleport = true;			break;
			case 't':	teleport = true;			break;
			case 'S':	screwdriverUsed = true;		break;
			case 's':	screwdriverUsed = true;		break;
			case 'L':	lastStand = true;			break;
			case 'l':	lastStand = true;			break;
			default:	validMove = false;
			}

		//  check if move is out-of-bounds
		if ((newX<0) || (newX>maxX-1) || (newY<0) || (newY>maxY-1))
			validMove = false;

		//   find an empty location if teleporting
		if (teleport)  {
			boolean okLoc = false;
			while (!okLoc)  {  // loop until valid coordinates are found
				okLoc = true;
				newX = (int)(Math.random()*maxX);
				newY = (int)(Math.random()*maxY);
				for (int j=0; j<numDaleks; j++)
					if ( ((newX==dalX[j])&&(newY==dalY[j])) && ((dalA[j])||(dalR[j])) )
						okLoc = false;
				}
			animTele = true;
			}

		//   check if player is allowed to use the screwdriver
		if ((screwdriverUsed) && (screwdrivers==0))  {
			validMove = false;
			screwdriverUsed = false;
			}

		//   check if attempting to move to a location containing rubble
		for (int j=0; j<numDaleks; j++)
			if ((newX==dalX[j]) && (newY==dalY[j]) && (dalR[j]))
				validMove = false;

		//   if everything's ok, record new position & orientation
		if (validMove)  {
			drX = newX;
			drY = newY;
			drF = newF;
			}
		return validMove;
		}

	public void checkDaleks()  {
		//   evaluate whether all the daleks are dead
		//   and set the 'allDead' variable accordingly.
		allDead = true;
		for (int j=0; j<numDaleks; j++)
			if (dalA[j])
				allDead = false;
		}

	public void moveDaleks()  {
		//   move each dalek toward player
		for (int j=0; j<numDaleks; j++)
			if (dalA[j])  {	// only move if dalek is alive
				if (dalX[j] < drX) dalX[j]++;
				if (dalY[j] < drY) dalY[j]++;
				if (dalX[j] > drX) dalX[j]--;
				if (dalY[j] > drY) dalY[j]--;
				if ((dalX[j]==drX) && (dalY[j]==drY))
					drA = false;	// player has died
				}

		//   check for collisions between daleks
		newRubble = false;
		for (int j=0; j<numDaleks; j++)  {
			for (int k=j+1; k<numDaleks; k++)
				if (    (dalX[j]==dalX[k]) && (dalY[j]==dalY[k]) &&
				        ((dalA[j]) || (dalR[j])) && ((dalA[k] || dalR[k]))   )  {
					if (dalA[j])  { score += level * 2; newRubble = true; }
					if (dalA[k]) { score += level * 2; newRubble = true; }
					dalA[j] = dalA[k] = false;	//   both daleks are dead
					dalR[j] = dalR[k] = true;	//   both daleks are rubble
					}
			}
		}	//   end of drwho.moveDaleks()

	}	//   end of class 'daleks14'



class animatedIntroClass  {
	public Graphics offScreenGC;
	public Image offScreen;
	public Graphics onScreenGC;

	Font titleFont;
	String gameTitle;
	int titleX, titleY;
	Font authorFont;
	int versionX, versionY;
	int byX, byY;
	int authorX, authorY;
	Font statusFont;
	int statusX1, statusY;
	int statusX2;

	String flyer;

	int maxX, maxY;
	int imgH, imgW;
	int radius;
	int barSegmentW;

	int numQmarks = 50;
	int qMarkX[];
	int qMarkY[];
	int qMarkXinc[];
	int qMarkYinc[];
	int qMarkCnt[];
	int qMarkSteps[];
	Color qMarkColor[];
	Color loadingBar = new Color(65, 100, 65);
	Color textPalette[] = new Color[6];
	boolean fadeOut = false;
	boolean fadeIn = false;
	int fadeIndex;

	public void start()  {
		//   determine question mark trajectory, velocity, and color *before*
		//   they are ever drawn
		for (int j=0; j<numQmarks; j++)  {
			int angle = (int)(Math.random()*360);
			int numSteps = (int)(Math.random()*30) + 20;
			qMarkSteps[j] = numSteps;
			int bright = 255 - numSteps * 3;
			qMarkColor[j] = new Color(85, bright, 85);
			qMarkXinc[j] = (int)(Math.cos(angle) * radius / numSteps);
			qMarkYinc[j] = (int)(Math.sin(angle) * radius / numSteps);
			qMarkCnt[j] = 0 - (int)(j/2);
			qMarkX[j] = maxX * 8 + 2 + qMarkXinc[j];
			qMarkY[j] = maxY * 8 + 2 + qMarkXinc[j];
			}
		}

	void updateBar(Image lastScreen)  {
		fadeIn = true;
		fadeOut = false;
		fadeIndex = 0;

		offScreen = lastScreen;
		}

	void setInfo(Image lastScreen, int newX, int newY, int newH, int newW, String newTitle,
		    String newFlyer, int newNumFlyers)  {
		offScreen = lastScreen;
		offScreenGC = offScreen.getGraphics();

		maxX = newX;
		maxY = newY;
		imgH = newH;
		imgW = newW;

		gameTitle = newTitle;

		flyer = newFlyer;
		numQmarks = newNumFlyers;
		qMarkX = new int[numQmarks];
		qMarkY = new int[numQmarks];
		qMarkXinc = new int[numQmarks];
		qMarkYinc = new int[numQmarks];
		qMarkCnt = new int[numQmarks];
		qMarkSteps = new int[numQmarks];
		qMarkColor = new Color[numQmarks];

		barSegmentW = maxX*imgW/24;

		int hH = newH / 2;
		int hW = newW / 2;
		radius = (int)(Math.sqrt((maxX*hW*maxX*hW)+(maxY*hH*maxY*hH))) + imgW;

		fadeIn = true;
		fadeOut = false;
		fadeIndex = 0;

		for (int j=0; j < 6; j++)
			textPalette[j] = new Color(51*j, 51*j, 51*j);

		//  handle Font creation and sizing
		int halfWayX = maxX * imgW / 2;
		int totalHeight = maxY * imgH;

		titleFont = new Font("Helvetica", Font.PLAIN, 36);
		offScreenGC.setFont(titleFont);
		titleX = halfWayX - offScreenGC.getFontMetrics().stringWidth(gameTitle) / 2;
		titleY = (int)(totalHeight * 0.3);

		authorFont = new Font("Helvetica", Font.PLAIN, 18);
		offScreenGC.setFont(authorFont);
		versionX = halfWayX - offScreenGC.getFontMetrics().stringWidth("v1.4") / 2;
		versionY = (int)(totalHeight * 0.4);
		byX = halfWayX - offScreenGC.getFontMetrics().stringWidth("by") / 2;
		byY = (int)(totalHeight * 0.55);
		authorX = halfWayX - offScreenGC.getFontMetrics().stringWidth("Ethan Koehler") / 2;
		authorY = byY + 21;

		statusFont = new Font("Helvetica", Font.PLAIN, 12);
		offScreenGC.setFont(statusFont);
		statusX1 = (halfWayX - offScreenGC.getFontMetrics().stringWidth("(please wait, loading data)") / 2);
		statusX2 = (halfWayX - offScreenGC.getFontMetrics().stringWidth("(press any key to begin)") / 2);
		statusY = (int)(totalHeight * 0.8);
		}

	void startFadeIn()  {
		fadeIn = true;
		}

	void runIntro(Graphics newgc)  {
		onScreenGC = newgc;
		}

	public void drawIntroScreen(Graphics g, boolean doneLoading, int currentPic)  {
		onScreenGC = g;
		offScreenGC.setColor(Color.black);
		offScreenGC.fillRect(0,0,maxX*imgW,maxY*imgH+1);
		if (doneLoading)
			offScreenGC.clipRect(0,0,maxX*imgW,maxY*imgH+1);
		offScreenGC.setFont(new Font("Helvetica", Font.PLAIN, 10));

		//   draw and move each question mark
		for (int j=0; j<numQmarks; j++)  {
			if (qMarkCnt[j]<1)  {
				//   return this flying widget to center of screen
				qMarkCnt[j] = qMarkSteps[j];
				qMarkX[j] = maxX * imgW/2 + 2 + qMarkXinc[j];
				qMarkY[j] = maxY * imgH/2 + 2 + qMarkXinc[j];
				}
			offScreenGC.setColor(qMarkColor[j]);
			offScreenGC.drawString(flyer, qMarkX[j], qMarkY[j]);
			qMarkX[j] += qMarkXinc[j];
			qMarkY[j] += qMarkYinc[j];
			qMarkCnt[j]--;
			}

		//   select and adjust (if necessary) color of intro screen text
		offScreenGC.setColor(textPalette[fadeIndex]);
		if (fadeIn)  {
			fadeIndex++;
			if (fadeIndex == 5)
				fadeIn = false;
			}

		//   draw text onto the off-screen bitmap
		offScreenGC.setFont(titleFont);
		offScreenGC.drawString(gameTitle, titleX, titleY);
		offScreenGC.setFont(authorFont);
		offScreenGC.drawString("v1.4", versionX, versionY);
		offScreenGC.drawString("by", byX, byY);
		offScreenGC.drawString("Ethan Koehler", authorX, authorY);

		offScreenGC.setFont(statusFont);
		if (!doneLoading)  {
			offScreenGC.drawString("(please wait, loading data)", statusX1, statusY);
			offScreenGC.setColor(Color.darkGray);
			offScreenGC.drawLine(0, maxY*imgH+1, maxX*imgW, maxY*imgH+1);
			offScreenGC.drawLine(0, maxY*imgH+3, maxX*imgW, maxY*imgH+3);
			offScreenGC.setColor(Color.lightGray);
			offScreenGC.drawLine(0, maxY*imgH+2, maxX*imgW, maxY*imgH+2);
			offScreenGC.setColor(loadingBar);
			offScreenGC.fillRect(0, maxY*imgH+4, maxX*imgW, 21);
			offScreenGC.setColor(Color.black);
			offScreenGC.fillRect(0, maxY*imgH+4, currentPic*barSegmentW, 21);
			}
		else
			offScreenGC.drawString("(press any key to begin)", statusX2, statusY);

		//   paint next frame to screen
		onScreenGC.drawImage(offScreen,0,0,null);
		}
	}
