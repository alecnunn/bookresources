import java.awt.*;
import java.applet.*;

/////////////////////////////////////////////////////////////////
//
// EnemyManager -- responsible for the controlling the
//                 Enemy alien sprites, and Enemy Fire
//
/////////////////////////////////////////////////////////////////
 

public class EnemyManager extends Object {

  // constants to define two sizes of enemy ships
  static final int LARGE = 0;
  static final int SMALL = 1;
  static final int SRATX = 3;     // x-ratio of large to small
  static final int SRATY = 2;     // y-ratio of large to small

  // the enemy ship templates
  //   x-coordinate template
  static final int tpx[][] =  {
    {14,11,8,-8,-11,-14,-6,6,14,-14},
    {14/SRATX,11/SRATX,8/SRATX,-8/SRATX,-11/SRATX,-14/SRATX,
     -6/SRATX,6/SRATX,14/SRATX,-14/SRATX} } ;

  //   y-coordinate template
  static final int tpy[][] =  {
    {0,3,6,6,3,0,-4,-4,0,0},
    {0/SRATY,3/SRATY,6/SRATY,6/SRATY,3/SRATY,0/SRATY,-4/SRATY,
     -4/SRATY,0/SRATY,0/SRATY} } ;

  // screen boundaries
  static int width,height;

  // constants parameters for enemy ships
  static final int MAX_ENEMIES = 3;
  static final int MAX_SHOTS_PER_ENEMY = 2;
  static final int MAX_SHOTS = MAX_ENEMIES*MAX_SHOTS_PER_ENEMY;

  // arrays for color, length, and value of various enemy ships
  static final Color ENEMY_COLOR[] = 
    {Color.green,Color.red,Color.orange};
  static final int ENEMY_LENGTH[] = {14,14,5};
  static final int VALUE[] = {500,750,1000};

  // maximum speed
  static final int MAX_VX = 9, MAX_VY = 9;

  // how often enemies appear, and how often they fire
  static double enemy_freq = 0.995 ;
  static double fire_freq = 0.995;

  // array of Enemy sprites
  private Enemy e[] = new Enemy[MAX_ENEMIES];

  // array of Fire sprites
  private Fire f[] = new Fire[MAX_SHOTS_PER_ENEMY*MAX_ENEMIES];

  // references to other game objects
  ShipManager sm;
  EffManager efm;
  Ship ship;              
  Fire[] shipfire;
  GameManager game;       

/////////////////////////////////////////////////////////////////
// EnemyManager constructor
/////////////////////////////////////////////////////////////////

  public EnemyManager(int width,int height,
		      ShipManager sm,
		      EffManager efm,
		      GameManager g) {
    ship = sm.getShip();              // get ship-- needed for
                                      //   collision detection
    shipfire = sm.getFire();          // get ship's fire for
    this.sm = sm;                     //   collision detection
    this.efm = efm;
    game = g;

    // initialize the three enemy sprites
    e[0] = new Enemy(tpx[0],tpy[0],tpx[0].length, // template
		     0,0,                         // initial loc
		     ENEMY_COLOR[0],              // color
		     width,height,              // screen bounds
		     ENEMY_LENGTH[0],VALUE[0]); // length,value
    e[1] = new Enemy(tpx[0],tpy[0],tpx[0].length,
		     0,0,ENEMY_COLOR[1],
		     width,height, 
		     ENEMY_LENGTH[1],VALUE[1]);
    e[2] = new Enemy(tpx[1],tpy[1],tpx[1].length,
		     0,0,
		     ENEMY_COLOR[2],
		     width,height, 
		     ENEMY_LENGTH[2],VALUE[2]);

    // suspend the three enemy sprites
    for (int i=0; i<MAX_ENEMIES; i++) {
      e[i].suspend();
    }

    // create and suspend the Fire sprites
    for ( int i=0 ; i<MAX_SHOTS; i++) {
      f[i] = new Fire(ENEMY_COLOR[i/MAX_SHOTS_PER_ENEMY],
		      width,height);
      f[i].suspend();
    }
		     
    // save screen bounds
    this.width = width;
    this.height = height;

  }


/////////////////////////////////////////////////////////////////
// initialize for new game
/////////////////////////////////////////////////////////////////

  public void newGame() {
    for (int i=0; i<MAX_ENEMIES; i++) {
      e[i].suspend();
    }

    for (int i=0 ; i<MAX_SHOTS; i++) {
      f[i] = new Fire(ENEMY_COLOR[i/MAX_SHOTS_PER_ENEMY],
		      width,height);
      f[i].suspend();
    }
  }


/////////////////////////////////////////////////////////////////
// update enemy ships and enemy fire
//    CHECK FOR COLLISIONS WITH SHIP OR SHIP FIRE
/////////////////////////////////////////////////////////////////

  public void update() {                


    // check if any of the fire sprites hit the Player's ship. 
    // If so, tell the ShipManager to destroy the ship

    for ( int i=0 ; i<MAX_SHOTS; i++) {
      f[i].update();
      if (f[i].intersect(ship)) {
	sm.destroyShip();               

      }
    }

/////////////////////////////////////////////////////////////////
    // cycle through all enemies
    for ( int i=0 ; i<MAX_ENEMIES; i++) {

      // place a new enemy on the screen at random intervals
      if (!e[i].isActive()) {
	if (Math.random() > enemy_freq) {
	  NewEnemy(i);
	}
      }

      // else update and check for collisions
      else {
	e[i].update();

	// if it intersects the ship
	if (e[i].intersect(ship)) {

	  // increase score
	  game.updateScore(e[i].value);

	  // tell sm to destroy ship 
	  sm.destroyShip();                

	  // suspend enemy and create explosion
	  e[i].suspend();
	  efm.addEnemyExplosion(e[i].locx,e[i].locy);
	}

	// check if enemy intersect's ship's fire
	for (int j=0; j<shipfire.length; j++) {
	  if (e[i].intersect(shipfire[j])) {
	    sm.stopFire(j);              // stop fire sprite
	    e[i].suspend();              // stop enemy sprite
	    efm.addEnemyExplosion(e[i].locx,e[i].locy);
	    game.updateScore(e[i].value);
	  }
	}

	// create new enemy fire at random intervals
	if (Math.random() > fire_freq) {
	  Fire(i);
	}
      }
    }
  }


/////////////////////////////////////////////////////////////////
// Accessor methods
/////////////////////////////////////////////////////////////////

  public Enemy[] getEnemy() {
    return e;
  }

  public Fire[] getFire() {
    return f;
  }

  public void destroyEnemy(int i) {
    e[i].suspend();
  }

  public void stopFire(int i) {
    f[i].suspend();
  }

/////////////////////////////////////////////////////////////////
// make new enemy at slot i of the Enemy array e
/////////////////////////////////////////////////////////////////

  private void NewEnemy(int i) {
    // set the enemy speed
    e[i].setVelocity(GameMath.getRand(2*MAX_VX)-MAX_VX,
		     GameMath.getRand(2*MAX_VY)-MAX_VY);
    // set the enemy position
    int px = (Math.random() > 0.5) ? 0 : width;
    int py = GameMath.getRand(height);
    e[i].setPosition(px,py);
    // restore the sprite
    e[i].restore();
  }

/////////////////////////////////////////////////////////////////
// initialize enemy fire for enemy ship i
/////////////////////////////////////////////////////////////////

  private void Fire(int i) {
    for (int j = i*MAX_SHOTS_PER_ENEMY; 
	 j < (i+1)*MAX_SHOTS_PER_ENEMY; 
	 j++) {

      // if there's a slot in enemy fire array,
      //   initialize and restore the fire sprite
      if (!f[j].isActive()) {
	f[j].initialize(e[i].locx,e[i].locy,
			GameMath.getRand(360));  // random angle
	f[j].restore();

      }
    }
  }

/////////////////////////////////////////////////////////////////
// paint the enemy ships and their fire
/////////////////////////////////////////////////////////////////


  public void paint(Graphics g) {
    for ( int i=0 ; i<MAX_ENEMIES; i++) {
      e[i].paint(g);
    }

    for ( int i=0 ; i<MAX_SHOTS; i++) {
      f[i].paint(g);
    }
  }
}
