import java.awt.*;

/////////////////////////////////////////////////////////////////
//
// ShipManager class -- responsible for tracking ship data
//                      (e.g. shipsLeft, shield)
//
//                   -- responsible for translating player input
//                      to Ship sprite methods
/////////////////////////////////////////////////////////////////


public class ShipManager extends Object {

  // coordinates of the Ship sprite polygon
  static final int tpx[] = { 0, -7, 7, 0};
  static final int tpy[] = { 13, -7, -7, 13};

  // 5 shots at a time
  static final int MAX_SHOTS = 5;       

  // length of ship's nose
  static final int SHIP_LENGTH = 13;


  static int width,height;                 // screen dimensions
  private int shipsLeft;
  private boolean shipAlive;
  private boolean shieldOn;
  private int shieldStrength;
  private boolean gameOver;

  private Ship s;                          // Ship sprite
  private Fire f[] = new Fire[MAX_SHOTS];  // Fire sprites
  EffManager efm;                          
  GameManager game;                        

/////////////////////////////////////////////////////////////////
// ShipManager constructor
/////////////////////////////////////////////////////////////////
  public ShipManager(int width,int height,
		     EffManager efm,GameManager game) {


    // initialize Ship sprite 
    s = new Ship(tpx,tpy,tpx.length,       // coords of Ship
		 width/2,height/2,         // initial position
		 Color.yellow,             // color
		 width,height,             // screen bounds
		 SHIP_LENGTH);             // length of ship

    // initialize Fire sprites
    for ( int i=0 ; i<MAX_SHOTS; i++) {
      f[i] = new Fire(Color.white,width,height);
      f[i].suspend();
    }
	
    // initialize variables
    this.efm = efm;
    this.game = game;
    this.width = width;
    this.height = height;
    shipsLeft = 2;
    shipAlive = true;
    shieldOn = false;
    shieldStrength = 100;
    gameOver = false;

    // activate Ship sprite
    s.restore();              
  }
  

/////////////////////////////////////////////////////////////////
// Accessor methods : provide access to privates:
//                    Ship and Fire sprites
/////////////////////////////////////////////////////////////////

  // return Ship sprite
  public Ship getShip() {
    return s;
  }

  // return array of Fire sprites
  public Fire[] getFire() {
    return f;
  }

  // suspend the given Fire sprite
  public void stopFire(int i) {
    f[i].suspend();
  }

  // give an extra ship
  public void extraShip() {
    shipsLeft++;
  }


/////////////////////////////////////////////////////////////////
// Destroy the Ship
/////////////////////////////////////////////////////////////////

  // waiting period for the next ship
  static int waitPeriod;                    

  public void destroyShip() {
    if (gameOver) return;

    // if there's no shield on, destroy the ship
    if (!(keyState[SHIELDINDEX] && shieldStrength > 0)) {

      shipAlive = false;              // set boolean
      s.suspend();                    // suspend sprite
      shipsLeft--;                    // update count
      if (shipsLeft < 0) {            // if no ships left
	game.setGameOver();           //   then game's over
	gameOver = true;
      }
      waitPeriod = 50;                // set wait period
      clearKeyState();                // clear key buffer
                                      // create explosion
      efm.addShipExplosion(s.locx,s.locy);
    }
  }

/////////////////////////////////////////////////////////////////
// clear key buffers
/////////////////////////////////////////////////////////////////
  private void clearKeyState() {
    for (int i=0; i<keyState.length; i++) {
      keyState[i] = false;
    }
  }

/////////////////////////////////////////////////////////////////
// Pad a number n with zeroes, up to the given length l
/////////////////////////////////////////////////////////////////

  public String PadNumber(int n,int l) {
    StringBuffer s;
    s = new StringBuffer(Integer.toString(n));
    while (s.length () < l) {
      s.insert(0,"0");
    }
    return s.toString();
  }




/////////////////////////////////////////////////////////////////
// Paint all information related to the Ship: 
//   Ship sprite
//   Fire sprites
//   Shield
//   Shield status
//   Ships left
/////////////////////////////////////////////////////////////////


  // text Strings needed
  String shieldString = new String("Shield:");
  String noShieldString = new String("No Shield");
  String shipsLeftString = new String("Ships Left:");

  public void paint(Graphics g) {
    g.setColor(Color.yellow);

    // draw ships left indicator
    g.drawString(shipsLeftString,69,20);
    g.drawString(PadNumber(shipsLeft,1),135,20);

    // if ship's alive:
    if (shipAlive) {

      // if shield's on, draw it!
      if (keyState[SHIELDINDEX] && shieldStrength > 0) {
	g.setColor(Color.green);
	g.drawOval(s.locx-15,s.locy-15,30,30);
	g.drawOval(s.locx-17,s.locy-17,34,34);
      }

      // draw shield status
      if (shieldStrength > 0) {
	g.setColor(Color.green);
	g.drawString(shieldString,153,20);
	g.fillRect(199,15,shieldStrength, 5);
      }

      
      else {
	g.setColor(Color.red);
	g.drawString(noShieldString,153,20);
      }    
     

      // paint the Ship sprite
      s.paint(g);

      // paint the Fire sprites
      for ( int i=0 ; i<MAX_SHOTS; i++) {
	f[i].paint(g);
      }
    }
  }
				  

/////////////////////////////////////////////////////////////////
// Translate keyboard input into Ship sprite actions
/////////////////////////////////////////////////////////////////

  // declare the key buffer
  static boolean keyState[] = new boolean[5];

  // key bindings
  static final int LEFTINDEX = 0;
  static final int LEFT = 'q';         // rotate left

  static final int RIGHTINDEX = 1;
  static final int RIGHT = 'w';        // rotate right

  static final int THRUSTINDEX = 2;
  static final int THRUST = 'o';       // thrust 

  static final int FIREINDEX = 3;
  static final int FIRE = 'p';         // fire 


  static final int SHIELDINDEX = 4;
  static final int SHIELD = ' ';       // shield





/////////////////////////////////////////////////////////////////
// update Ship and Fire sprites based on key buffer
/////////////////////////////////////////////////////////////////

  public void update() {        

    // update fire sprites
    for ( int i=0 ; i<MAX_SHOTS; i++) {
      f[i].update();
    }

    // if ship's alive
    if (shipAlive) {

      // check the key buffer and perform the
      //   associated action

      if (keyState[LEFTINDEX]) 
	s.rotateLeft();
      if (keyState[RIGHTINDEX]) 
	s.rotateRight();
      if (keyState[THRUSTINDEX]) 
	s.thrust();
      if (keyState[FIREINDEX]) 
	handleFire();
      if (keyState[SHIELDINDEX]) 
	updateShield();

      // update the Ship sprite
      s.update();
    }

    // otherwise, the ship's not alive!
    else {

      // if the waiting period's over,
      //   initialize a new Ship sprite
      if (!gameOver && (waitPeriod-- <= 0)) {
	s.setAngle(270);
	s.setPosition(width/2,height/2);
	s.setVelocity(0,0);
	s.rotate(0);
	shieldStrength = 100;
	shipAlive = true;
	s.restore();
      }
    }
  }


/////////////////////////////////////////////////////////////////
// initialize parameters for a new game
/////////////////////////////////////////////////////////////////

  public void newGame() {
	s.setAngle(270);
	s.setPosition(width/2,height/2);
	s.setVelocity(0,0);
	s.rotate(0);
	shieldStrength = 100;
	shipAlive = true;
	shipsLeft = 2;
	gameOver = false;
	s.restore();
      }


/////////////////////////////////////////////////////////////////
// increase Shield strength (when the Ship touches a powerup
/////////////////////////////////////////////////////////////////

  public void increaseShield() {
    shieldStrength += 30;
    if (shieldStrength > 250)
      shieldStrength = 250;
  }

/////////////////////////////////////////////////////////////////
// handle keyDown events by setting the appropriate
//   field in the key buffer
///////////////////////////////////////////////////////////////// 

  public void keyDown(Event e,int k) {
    switch(k) {
    case LEFT: 
      keyState[LEFTINDEX] = true;
      break;
    case RIGHT: 
      keyState[RIGHTINDEX] = true;
      break;
    case FIRE:
      keyState[FIREINDEX] = true;
      break;
    case THRUST:
      keyState[THRUSTINDEX] = true;
      break;
    case SHIELD:
      keyState[SHIELDINDEX] = true;
      break;

    default:
      break;
    }
  }   


/////////////////////////////////////////////////////////////////
// handle keyDown events by setting the appropriate
//   field in the key buffer
///////////////////////////////////////////////////////////////// 

  public void keyUp(Event e,int k) {
    switch(k) {
    case LEFT: 
      keyState[LEFTINDEX] = false;
      break;
    case RIGHT: 
      keyState[RIGHTINDEX] = false;
      break;
    case FIRE:
      keyState[FIREINDEX] = false;
      break;
    case THRUST:
      keyState[THRUSTINDEX] = false;
      break;
    case SHIELD:
      keyState[SHIELDINDEX] = false;
      break;

    default:
      break;
    }
  }   

/////////////////////////////////////////////////////////////////
// update Shield strength (when the player's activating Shield)
/////////////////////////////////////////////////////////////////

  private void updateShield() {
    if (--shieldStrength <= 0) {
      shieldStrength = 0;
    }
  }

/////////////////////////////////////////////////////////////////
// start a new Fire sprite
/////////////////////////////////////////////////////////////////
    
  private void handleFire() {
    for (int i=0; i<MAX_SHOTS; i++) {
      if (!f[i].isActive()) {

	// start fire from ship's nose at angle theta
	f[i].initialize(s.p.xpoints[0],s.p.ypoints[0],s.theta);
	break;
      }
    }
  }
	  
}
