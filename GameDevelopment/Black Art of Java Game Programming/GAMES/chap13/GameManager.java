import java.awt.*;
import java.applet.*;
import java.util.*;
import java.net.*;

/////////////////////////////////////////////////////////////////
//
// GameManager:
// responsible for tracking status of game and keeping score
//
/////////////////////////////////////////////////////////////////

public class GameManager extends Applet implements Runnable {


  // variables for double buffering and animation
  Thread animation;
  Graphics gbuf;
  Image im;
  static final int REFRESH_RATE = 72;       // in ms

  // size of the game applet: change as needed
  static final int MAX_HEIGHT = 400;
  static final int MAX_WIDTH = 400;

  // game parameters
  private int score;                        // score
  private boolean playing;                  // playing or not
  private boolean gameOver;                 // is game over
  private int intro_screen;                 // which screen
  static final int EXTRA_SHIP = 10000;   // #pts for extra ship 
  private int current_extra = 0;            // counts extras

  // constants for the introduction part of the game
  static final int INSTRUCTIONS  = 1;
  static final int TEST_DRIVE  = 2;
  static final int INTRO_OVER  = 3;

  // references to manager classes
  AstManager am;		            // asteroids
  ShipManager sm;                           // ship
  EnemyManager em;                          // enemy ships
  EffManager efm;                           // effects

  // sound
  URL codebase;
  AudioClip expsound = null;

  // variables to monitor performance during game
  Date time;
  long t1,t2,dt;

  // fonts
  static Font bigfont = new Font("TimesRoman", Font.BOLD,24);
  static Font medfont = new Font("TimesRoman", Font.PLAIN,18);
  static Font smallfont = new Font("TimesRoman", Font.PLAIN,12);
  
/////////////////////////////////////////////////////////////////
// initialize applet
/////////////////////////////////////////////////////////////////
  public void init() {

    // initialize screen
    setBackground(Color.black);
    setFont(smallfont);

    // initialize double buffer
    im = createImage(MAX_WIDTH,MAX_HEIGHT);
    gbuf = im.getGraphics();

    // initialize sound
    codebase = getCodeBase();
  // System.out.println(codebase.toString());

    resize(MAX_WIDTH,MAX_HEIGHT);
    
    // initialize game state
    gameOver = true;
    intro_screen = 1;
    playing = false;

  }


/////////////////////////////////////////////////////////////////
// start applet
/////////////////////////////////////////////////////////////////
  public void start() {
    // load sound

   try {
     expsound = getAudioClip(getCodeBase(),"Explosion.au");

   }  
  catch (Exception exc) {
    System.out.println("Sound not loaded");
  };

   // initialize manager classes

   efm = new EffManager(expsound);

   sm = new ShipManager(MAX_WIDTH,MAX_HEIGHT,efm,this);

   em = new EnemyManager(MAX_WIDTH,MAX_HEIGHT,sm,efm,this);

   am = new AstManager(MAX_WIDTH,MAX_HEIGHT,sm,em,efm,this);



   // start animation
   if (animation == null) {
     animation = new Thread(this);
     if (animation != null) {
       animation.start();
     }
     else
       System.out.println("Insufficient memory to fork thread!");
   }
 }


/////////////////////////////////////////////////////////////////
// stop applet 
/////////////////////////////////////////////////////////////////
  public void stop() {
    if (animation != null) {
      animation.stop();
      animation = null;
    }
  }

/////////////////////////////////////////////////////////////////
// set gameOver flag
/////////////////////////////////////////////////////////////////
  public void setGameOver() {
    gameOver = true;
  }

/////////////////////////////////////////////////////////////////
// start a new game
/////////////////////////////////////////////////////////////////

  private void newGame() {
    // tell managers to start a new game
    em.newGame();
    am.newGame();
    sm.newGame();

    // set font for game
    gbuf.setFont(smallfont);

    // initialize parameters
    current_extra = 0;
    score = 0;
    gameOver = false;
    playing = true;

  }


/////////////////////////////////////////////////////////////////
//
// Event Handlers
//
/////////////////////////////////////////////////////////////////

  // if mouseUp occurs, proceed to next screen of introduction.
  // if game's being played, restart the game.

  public boolean mouseUp(Event e,int x,int y) {

    if (intro_screen == INSTRUCTIONS) {
      intro_screen = TEST_DRIVE;
      sm.getShip().setPosition(385,75);
      sm.getShip().restore();
    }
    // else restart the game
    else if (intro_screen == TEST_DRIVE) {
      intro_screen = INTRO_OVER;
      newGame();
    }
    else 
      newGame();

    return true;
  }

  // pass key events to the ShipManager
  public boolean keyDown(Event e, int k) {
    sm.keyDown(e,k);
    return true;
  }
  // pass key events to the ShipManager
  public boolean keyUp(Event e, int k) {
    sm.keyUp(e,k);
    return true;
  }



/////////////////////////////////////////////////////////////////
// the game driver
/////////////////////////////////////////////////////////////////
  public void run() {
    startInstructions();

    // the Video Game Loop
    while (true) {
      
      if (playing) {
	time = new Date();            // track time
	t1 = time.getTime();
	if (!gameOver) {
	  sm.update();                // update ship
	}
	em.update();                  // update enemies
	am.update();                  // update asteroids
	efm.update();                 // update effects
	repaint();

	Thread.currentThread().yield();
	time = new Date();            // track time
	t2 = time.getTime();
	dt = t2-t1;                   // how long did it take?

                                      // sleep 
	try {
	  Thread.sleep (REFRESH_RATE-(int)dt < 0 
			? 8 
			: REFRESH_RATE-(int)dt);
	} catch (Exception exc) { };

      }

      // else not playing
      else {
	updateOpening();              // update opening
	repaint();                   
	Thread.currentThread().yield();
	try {                         
	  Thread.sleep (REFRESH_RATE);// sleep
	} catch (Exception exc) { };
      }
    }
  }

/////////////////////////////////////////////////////////////////
// initialize the AstManager and EnemyManager for the
// INSTRUCTIONS screen.
/////////////////////////////////////////////////////////////////
  private void startInstructions() {
    Asteroid a[] = am.getAsts();
    Asteroid p = am.getPowerup();
    // set asteroids in instruction screen
    a[0].setPosition(270,98);
    a[0].setRotationRate(4);
    a[0].setVelocity(1,2);
    a[0].restore();
    a[am.NUM_LARGE].setPosition(330,138);
    a[am.NUM_LARGE].setRotationRate(-4);
    a[am.NUM_LARGE].setVelocity(-1,-1);
    a[am.NUM_LARGE].restore();
    a[am.NUM_LARGE+am.NUM_MEDIUM].setPosition(370,178);
    a[am.NUM_LARGE+am.NUM_MEDIUM].setRotationRate(5);
    a[am.NUM_LARGE+am.NUM_MEDIUM].setVelocity(2,-1);
    a[am.NUM_LARGE+am.NUM_MEDIUM].restore();

    // set powerup in intro screen
    p.setPosition(330,340);
    p.restore();

    // set enemies for intro screen
    Enemy e[] = em.getEnemy();
    e[0].setPosition(10,214);
    e[0].setVelocity(2,0);
    e[0].restore();
    e[1].setPosition(390,254);
    e[1].setVelocity(-3,0);
    e[1].restore(); 
    e[2].setPosition(7,294);
    e[2].setVelocity(4,0);
    e[2].restore();
  }


/////////////////////////////////////////////////////////////////
// update the opening screens
/////////////////////////////////////////////////////////////////
  private void updateOpening() {
    if (intro_screen == INSTRUCTIONS) {
      am.update();
      Enemy e[] = em.getEnemy();
      e[0].update();
      e[1].update();
      e[2].update();
      efm.update();
    }
    else if (intro_screen == TEST_DRIVE) {
      sm.update();
    }
  }


/////////////////////////////////////////////////////////////////
// update the score. If the score passes threshold
// (every EXTRA_SHIP points) give an extra ship!
/////////////////////////////////////////////////////////////////
  public void updateScore(int val) {
    score += val;
    if ((score / EXTRA_SHIP) > current_extra) {
      sm.extraShip();                   // give extra ship
      current_extra = score/EXTRA_SHIP;
    }
  }


/////////////////////////////////////////////////////////////////
// override update() to eliminate flicker
/////////////////////////////////////////////////////////////////
  public void update(Graphics g) {
    paint(g);                            // don't clear screen
  }


/////////////////////////////////////////////////////////////////
// return a string of n that is 0-padded with the given length 
/////////////////////////////////////////////////////////////////
  public String PadNumber(int n,int len) {
    StringBuffer s;
    s = new StringBuffer(Integer.toString(n));
    while (s.length () < len) {
      s.insert(0,"0");
    }
    return s.toString();
  }



/////////////////////////////////////////////////////////////////
// define Strings used in the Opening screens
/////////////////////////////////////////////////////////////////

  // Strings used in INSTRUCTIONS screen
  String javaroidsString = new String("JAVAroids!");
  String galleryString = new String("The Lineup...");
  String largeAstString = 
    new String("30 points for Big Asteroid");
  String mediumAstString = 
    new String("60 points for Medium Asteroid");
  String smallAstString = 
    new String("90 points for Small Asteroid");
  String greenAlienString = 
    new String("500 points for Green Alien");
  String redAlienString = 
    new String("750 points for Red Alien");
  String orangeAlienString = 
    new String("1000 points for Orange Alien");
  String clickMouseString1 = 
    new String("Click mouse to continue...");
  String powerupString1 = 
    new String("Touch PowerUp for");
  String powerupString2 = 
    new String("extra shield strength:");

  // Strings used in the TEST_DRIVE screen
  String shipControlString = 
    new String("Test Drive your ship NOW ---> ");
  String rotLeftString = new String("Press 'q' to Rotate Left");
  String rotRightString = 
    new String("Press 'w' to Rotate Right");
  String thrustString = new String("Press 'o' to Thrust");
  String fireString = new String("Press 'p' to Fire");
  String shieldString = 
    new String("Press <space bar> for Shield");

  String extraShipString = 
    new String("Extra Ship every 10,000 points");
  String goodLuckString = 
    new String("GOOD LUCK!");
  String clickMouseString = 
    new String("Click mouse to begin!");
  String byMeString = new String("by Joel Fan");
  
  String gameOverString = new String("GAME OVER");
  String clickMouseString2 = 
    new String("Click mouse to play again!");

/////////////////////////////////////////////////////////////////
// paint to the screen, depending on mode of game
/////////////////////////////////////////////////////////////////
  public void paint(Graphics g) {
    
    // clear offscreen buffer
    gbuf.setColor(Color.black);
    gbuf.fillRect(0,0,MAX_WIDTH,MAX_HEIGHT);  

    if (playing) {

      // if game's not over, show the ship.
      if (!gameOver) {
	sm.paint(gbuf);
      }
      
      // otherwise, display the Game Over message
      else {
	gbuf.setFont(bigfont);
	gbuf.setColor(Color.cyan);
	gbuf.drawString(byMeString, 
			MAX_WIDTH/2 - 55, MAX_HEIGHT/2-60);
	
	gbuf.setColor(Color.magenta);
	gbuf.drawString(gameOverString, 
			MAX_WIDTH/2 - 65, MAX_HEIGHT/2);


	gbuf.drawString(clickMouseString2, 
			MAX_WIDTH/2 - 120, MAX_HEIGHT/2+35);
	gbuf.setFont(smallfont);
      }
      
      // tell other manager classes to paint themselves
      em.paint(gbuf);                     // paint enemies
      am.paint(gbuf);                     // paint asteroids
      efm.paint(gbuf);                    // paint effects
    
      // draw the score
      gbuf.drawString(PadNumber(score,6),20,20);

      // draw offscreen image to screen
      g.drawImage(im,0,0,this);
    }
  
    else if (intro_screen == INSTRUCTIONS) {
      paintInstructions(g);
    }

    else if (intro_screen == TEST_DRIVE) {
      paintTestDrive(g);
    }
  }


/////////////////////////////////////////////////////////////////
// paint Instructions screen
//   coordinates are hardcoded for simplicity
/////////////////////////////////////////////////////////////////
  public void paintInstructions(Graphics g) {

    gbuf.setFont(bigfont);

    // set a random color
    gbuf.setColor(new Color(GameMath.getRand(155)+100,
			    GameMath.getRand(155)+100,
			    GameMath.getRand(155)+100));
    gbuf.drawString(javaroidsString,MAX_WIDTH/2 - 60, 30);

    gbuf.setColor(Color.yellow);
    gbuf.setFont(medfont);
    gbuf.setColor(Color.magenta);
    gbuf.setFont(bigfont);
    gbuf.drawString(largeAstString,25,80);
    gbuf.drawString(mediumAstString,25,120);
    gbuf.drawString(smallAstString,25,160);
    gbuf.setColor(Color.green);
    gbuf.drawString(greenAlienString,25,200);
    gbuf.setColor(Color.red);
    gbuf.drawString(redAlienString,25,240);
    gbuf.setColor(Color.orange);
    gbuf.drawString(orangeAlienString,25,280);
    gbuf.setColor(Color.green);
    gbuf.drawString(powerupString1,25,320);
    gbuf.drawString(powerupString2,50,350);
    gbuf.setColor(Color.yellow);
    gbuf.drawString(clickMouseString1,MAX_WIDTH/2-120,385);
    am.paint(gbuf);   
    Enemy e[] = em.getEnemy();
    e[0].paint(gbuf);
    e[1].paint(gbuf);
    e[2].paint(gbuf);
    efm.paint(gbuf);
    // dump offscreen buffer to screen
    g.drawImage(im,0,0,this);

  }


/////////////////////////////////////////////////////////////////
// paint the test drive screen
//   coordinates are hardcoded for simplicity
/////////////////////////////////////////////////////////////////
  public void paintTestDrive(Graphics g) {

    gbuf.setFont(smallfont);
    sm.paint(gbuf);            // paint the ship
    gbuf.setFont(bigfont);

    gbuf.setColor(new Color(GameMath.getRand(155)+100,
			    GameMath.getRand(155)+100,
			    GameMath.getRand(155)+100));
    gbuf.drawString(javaroidsString,MAX_WIDTH/2 - 60, 50);
    gbuf.setColor(Color.magenta);
    gbuf.drawString(shipControlString,25,80);
    gbuf.setColor(Color.orange);  
    gbuf.drawString(rotLeftString,25,130);
    gbuf.drawString(rotRightString,25,170);
    gbuf.drawString(thrustString,25,210);
    gbuf.drawString(fireString,25,250);
    gbuf.drawString(shieldString,25,290);
    gbuf.setColor(Color.cyan);
    gbuf.drawString(extraShipString,25,340);
    gbuf.setColor(Color.green);
    gbuf.drawString(clickMouseString,MAX_WIDTH/2-120,385);
    g.drawImage(im,0,0,this);

  }

}


