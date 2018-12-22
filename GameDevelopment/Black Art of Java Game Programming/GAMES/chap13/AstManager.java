import java.awt.*;
import java.applet.*;
import java.util.*;

/////////////////////////////////////////////////////////////////
//
// the Asteroid Manager class -- handles all things relating
//                                 to asteroids, as well as 
//                                 collisions with other 
//                                 game objects
//
/////////////////////////////////////////////////////////////////



public class AstManager extends Object {


/////////////////////////////////////////////////////////////////
// constants -- these control the size, basic shape, and speed
//              of the asteroids
/////////////////////////////////////////////////////////////////
  
  // max speed
  static final int MAX_VX = 4;
  static final int MAX_VY = 4;

  // max size
  static final int MAX_RADIUS = 30;

  // size ratio between large:medium, and medium:small asteroids
  static final int SIZE_RATIO = 2;  

  // constants that refer to asteroid size
  static final int SMALL = 2;             
  static final int MEDIUM = 1;             
  static final int LARGE = 0;             

  // how much each asteroid's worth
  static final int VAL_SMALL = 90;             
  static final int VAL_MEDIUM = 60;             
  static final int VAL_LARGE = 30;             

  // large:small asteroid size ratio
  static final int SRAT = SMALL*SIZE_RATIO;  
  
  // large:medium asteroid size ratio
  static final int MRAT = MEDIUM*SIZE_RATIO;      

/////////////////////////////////////////////////////////////////
// templates for the asteroids
/////////////////////////////////////////////////////////////////

  static final int PX0 = 
    (int)(Math.cos(0.0*GameMath.DEG_TO_RAD)*MAX_RADIUS);
  static final int PX1 = 
    (int)(Math.cos(72.0*GameMath.DEG_TO_RAD)*MAX_RADIUS);
  static final int PX2 = 
    (int)(Math.cos(144.0*GameMath.DEG_TO_RAD)*MAX_RADIUS);
  static final int PX3 = 
    (int)(Math.cos(216.0*GameMath.DEG_TO_RAD)*MAX_RADIUS);
  static final int PX4 = 
    (int)(Math.cos(288.0*GameMath.DEG_TO_RAD)*MAX_RADIUS);
  static final int PY0 = 
    (int)(Math.sin(0.0*GameMath.DEG_TO_RAD)*MAX_RADIUS);
  static final int PY1 = 
    (int)(Math.sin(72.0*GameMath.DEG_TO_RAD)*MAX_RADIUS);
  static final int PY2 = 
    (int)(Math.sin(144.0*GameMath.DEG_TO_RAD)*MAX_RADIUS);
  static final int PY3 = 
    (int)(Math.sin(216.0*GameMath.DEG_TO_RAD)*MAX_RADIUS);
  static final int PY4 = 
    (int)(Math.sin(288.0*GameMath.DEG_TO_RAD)*MAX_RADIUS);

  // the x-coordinates of large, medium, and small template

  static final int px[][] =  {
    {0,MAX_RADIUS/2,MAX_RADIUS,MAX_RADIUS/2,0,-MAX_RADIUS/2,
     -MAX_RADIUS,-MAX_RADIUS/2,0},
    {PX0/MRAT,PX1/MRAT,PX2/MRAT,PX3/MRAT,PX4/MRAT},
    {PX0/SRAT,PX1/SRAT,PX2/SRAT,PX3/SRAT,PX4/SRAT}
 };

  // the y-coordinates of large, medium, and small template

  static final int py[][] = { 
    {MAX_RADIUS,MAX_RADIUS/2,0,-MAX_RADIUS/2,-MAX_RADIUS,
     -MAX_RADIUS/2,0,MAX_RADIUS/2,MAX_RADIUS},
    {PY0/MRAT,PY1/MRAT,PY2/MRAT,PY3/MRAT,PY4/MRAT},
    {PY0/SRAT,PY1/SRAT,PY2/SRAT,PY3/SRAT,PY4/SRAT}

 } ;

/////////////////////////////////////////////////////////////////
// coordinates of the powerup
/////////////////////////////////////////////////////////////////

  static final int powerx[] = {
    10,2,0,-2,-10,-2,0,2,10
    };
  static final int powery[] = {
    0,2,10,2,0,-2,-10,-2,0
    };

  
  // variance in asteroid shape
  static final double AVAR = 1.7;      

  // used for intersection calcs 
  static final int MAX_RAD_LARGE = 
    MAX_RADIUS+(int)(MAX_RADIUS/AVAR);
  static final int MAX_RAD_MEDIUM = 
    MAX_RADIUS/MRAT+(int)(MAX_RADIUS/AVAR/MRAT);
  static final int MAX_RAD_SMALL = 
    MAX_RADIUS/SRAT+(int)(MAX_RADIUS/AVAR/SRAT);

  // num of large asteroids
  static final int NUM_LARGE = 6;

  // num of medium asteroids
  static final int NUM_MEDIUM = 2*NUM_LARGE;

  // num of small asteroids
  static final int NUM_SMALL = 2*NUM_MEDIUM;

  // total number of asteroids
  static final int NUM_ASTS = NUM_LARGE+NUM_MEDIUM+NUM_SMALL;

  // color of asteroids
  static final Color LARGE_COLOR = new Color(150,150,233);
  static final Color MEDIUM_COLOR = Color.cyan;
  static final Color SMALL_COLOR = Color.magenta;


/////////////////////////////////////////////////////////////////
// variables
/////////////////////////////////////////////////////////////////

  // level of play 
  static int level;

  // array of asteroids
  private Asteroid a[] = new Asteroid[NUM_ASTS];

  // the powerup
  private Asteroid powerup;

  // screen bounds
  static int width,height;


  // references to other game objects
  // the ship
  ShipManager sm;
  Ship ship;     
  Fire[] fire;
  
  // the enemies
  EnemyManager em;
  Enemy[] enemy;
  Fire[] enemyfire;

  // the effect manager
  EffManager efm;
  // game manager
  GameManager game;    


/////////////////////////////////////////////////////////////////
// AstManager constructor: 
//    create all Asteroids used in game. This is done 
//    at beginning to improve performance
/////////////////////////////////////////////////////////////////

  public AstManager(int w, int h,
		    ShipManager sm,EnemyManager em,
		    EffManager efm, GameManager g) {
    ship = sm.getShip();              // get ship-- needed for
                                      // collision detection
    fire = sm.getFire();              // -- same
    this.sm = sm;                      
    enemy = em.getEnemy();            // get enemies-- needed for
                                      // collision detection
    enemyfire = em.getFire();         // -- same
    this.em = em;                      
    this.efm = efm;
    game = g;
    width = w;
    height = h;
    
    // create storage for actual asteroids:

    // storage for large ast
    int tx0[] = new int[px[LARGE].length];
    int ty0[] = new int[px[LARGE].length];

    // storage for medium ast
    int tx1[] = new int[px[MEDIUM].length];
    int ty1[] = new int[px[MEDIUM].length];
    
    // storage for small ast
    int tx2[] = new int[px[SMALL].length];
    int ty2[] = new int[px[SMALL].length];
    

    // initialize powerup sprite
    powerup = new Asteroid(powerx,powery,powerx.length,
			   0,0,
			   Color.green,w,h,
			   8,0);

    powerup.setRotationRate(3);

    // make large asts
    for (int i=0; i<NUM_LARGE; i++) {
      MakeVectors(tx0,ty0,LARGE);
      a[i] = new Asteroid(tx0,ty0,px[LARGE].length,
			  0,0,LARGE_COLOR,w,h,
			  MAX_RAD_LARGE-
			  (int)(MAX_RADIUS/AVAR/2.0),
			  VAL_LARGE);
    }

    // make medium asts
    for (int i=NUM_LARGE; i<NUM_LARGE+NUM_MEDIUM; i++) {
      MakeVectors(tx1,ty1,MEDIUM);
      a[i] = new Asteroid(tx1,ty1,px[MEDIUM].length,
			  0,0,MEDIUM_COLOR,w,h,
			  MAX_RAD_MEDIUM-
			  (int)(MAX_RADIUS/AVAR/MRAT/2.0),
			  VAL_MEDIUM);
    }


    // make small asts
    for (int i=NUM_MEDIUM; 
	 i<NUM_LARGE+NUM_MEDIUM+NUM_SMALL; 
	 i++) {
      MakeVectors(tx2,ty2,SMALL);
      a[i] = new Asteroid(tx2,ty2,px[SMALL].length,
			  0,0,SMALL_COLOR,w,h,
			  MAX_RAD_SMALL-
			  (int)(MAX_RADIUS/AVAR/SRAT/2.0),
			  VAL_SMALL);
    }
    
    newGame();
  }


/////////////////////////////////////////////////////////////////
// initialize a new Game: set the level, and create 
//   a new board of asts at that level
/////////////////////////////////////////////////////////////////

  public void newGame() {
    level = 1;
    newBoard(level);
  }

/////////////////////////////////////////////////////////////////
//
// The Update method:
//     handle collisions between:
//       asts and (ship, ship fire, enemies, enemy fire)
//       powerup and ship
/////////////////////////////////////////////////////////////////

  public void update() {

    // update powerup
    if (powerup.isActive()) {
      powerup.update();

      // if ship touches powerup, increase shield
      if (powerup.intersect(ship)) {
	powerup.suspend();
	sm.increaseShield();
      }
    }

    ////////////////////////////////
    // iterate through all asteroids
    ////////////////////////////////

    boolean asts_left = false;

    for (int i=0; i<NUM_ASTS; i++) {
      if (a[i].isActive()) {
	asts_left = true;
	a[i].update();

	///////////////////////////////////
	// CHECK FOR SHIP INTERSECTION
	// if the ship's grace period isn't over,
	//   update the grace period
	if (gracePeriod > 0) {
	  gracePeriod--;
	}

	// else check if the ast hits the ship
	else {
	  if (a[i].intersect(ship)) {
	    // divide Asteroid i
	    Divide(i);           
	    
	    game.updateScore(a[i].value);
	    // tell ShipManager to destroy ship 
	    sm.destroyShip();                
	  }
	}

	///////////////////////////////////
	// CHECK FOR ENEMY INTERSECTION
	//
	for (int j=0; j<enemy.length; j++) {
	  if (a[i].intersect(enemy[j])) {
	    Divide(i);
	    em.destroyEnemy(j);
	  }
	}

	////////////////////////////////////////
	// CHECK FOR INTERSECTION WITH SHIP FIRE
	//

	for (int j=0; j<fire.length; j++) {
	  if (a[i].intersect(fire[j])) {
	    // stop fire sprite
	    sm.stopFire(j);              
	    Divide(i);

	    game.updateScore(a[i].value);
	  }
	}

	/////////////////////////////////////////
	// CHECK FOR INTERSECTION WITH ENEMY FIRE
	//
        for (int j=0; j<enemyfire.length; j++) {
	  if (a[i].intersect(enemyfire[j])) {
	    // stop fire sprite
	    em.stopFire(j);              
	    Divide(i);
	  
	  }
	}
      }
    }

    // IF NO ASTEROIDS LEFT, CREATE A NEW BOARD
    if (!asts_left) {
      newBoard(++level);
    }
  }

/////////////////////////////////////////////////////////////////
// set the level of play
/////////////////////////////////////////////////////////////////

  public void setLevel(int i) {
    level = i;
  }


/////////////////////////////////////////////////////////////////
// split an asteroid into two smaller ones
/////////////////////////////////////////////////////////////////

  private void Divide(int i) {
    int child1, child2;
    int vertex;
    // suspend asteroid i, and create an explosion there
    a[i].suspend();
    efm.addAstExplosion(a[i].locx,a[i].locy);

    // don't do anything if it's a small asteroid
    if (i >= NUM_LARGE + NUM_MEDIUM)    
      return;

    // if it's a large ast, split it into two mediums
    if (i < NUM_LARGE) {                
      child1 = NUM_LARGE + 2*i;         //  indices of med asts
      child2 = NUM_LARGE + 2*i + 1;
    }
    // else medium ast => two smalls
    else {                            //  indices of small asts
      child1 = (NUM_LARGE+NUM_MEDIUM) + 2*(i-NUM_LARGE);
      child2 = (NUM_LARGE+NUM_MEDIUM) + 2*(i-NUM_LARGE) + 1;
    }


    vertex = a[i].p.npoints - 1;      // last vertex

    // set location of children to midpt betw center and vertex
    a[child1].setPosition(a[i].tx[vertex]/2 + a[i].locx,
			  a[i].ty[vertex]/2 + a[i].locy);

    a[child2].setPosition(a[i].tx[0]/2 + a[i].locx,
			  a[i].ty[0]/2 + a[i].locy);

    // set velocity of children
    a[child1].setVelocity(a[i].vx + 
			  GameMath.getRand(2*MAX_VX)-MAX_VX,
			  a[i].vy + 
			  GameMath.getRand(2*MAX_VY)-MAX_VY);

    a[child2].setVelocity(a[i].vx + 
			  GameMath.getRand(2*MAX_VX)-MAX_VX,
			  a[i].vy + 
			  GameMath.getRand(2*MAX_VY)-MAX_VY);

    // restore these sprites
    a[child1].restore();
    a[child2].restore();
    
  }
   

/////////////////////////////////////////////////////////////////
// paint the powerup and asteroids
/////////////////////////////////////////////////////////////////
    
  public void paint(Graphics g) {
    powerup.paint(g);
    for (int i=0; i<NUM_ASTS; i++) {
      a[i].paint(g);
    }
  }

/////////////////////////////////////////////////////////////////
// Accessor methods
/////////////////////////////////////////////////////////////////

  public Asteroid[] getAsts() {
    return a;
  }
  public Asteroid getPowerup() {
    return powerup;
  }

/////////////////////////////////////////////////////////////////
// Create a new board of asteroids
//   based on the given level 'n'
/////////////////////////////////////////////////////////////////

  // period of invulnerability for ship
  //   every time there's a new board
  static int gracePeriod;
  


  public void newBoard(int n) {
    int x,y;
    int vx,vy;

    // define the number of large asts in this board
    int num_asts = n/2 + 1;
    if (num_asts >= NUM_LARGE) {
      num_asts = NUM_LARGE;
    }

    // set the position and speed of these large asteroids
    //   and restore these sprites
    for (int i=0; i<num_asts; i++) {
      x = GameMath.getRand(width-2*MAX_RADIUS)+MAX_RADIUS ;
      y = GameMath.getRand(height-2*MAX_RADIUS)+MAX_RADIUS ; 
      vx = GameMath.getRand(2*MAX_VX)-MAX_VX;
      vy = GameMath.getRand(2*MAX_VY)-MAX_VY;
      a[i].setPosition(x,y);
      a[i].setVelocity(vx,vy);
      a[i].restore();             
    }

    // set the powerup
    if (!powerup.isActive()) {
      x = GameMath.getRand(width-2*MAX_RADIUS)+MAX_RADIUS ;
      y = GameMath.getRand(height-2*MAX_RADIUS)+MAX_RADIUS ; 
      powerup.setPosition(x,y);
      powerup.restore();
    }

    // set the grace period for the ship
    gracePeriod = 50;

    // suspend the remaining sprites
    for (int i= num_asts; i<NUM_ASTS; i++) {
      a[i].suspend();
    }
  }


/////////////////////////////////////////////////////////////////
// Make the vectors of the individual Asteroids,
//   by randomly perturbing the given template
/////////////////////////////////////////////////////////////////

  private void MakeVectors(int tpx[], int tpy[],int size) {
    int n = tpx.length;        // number of points in asteroid
    int deviation;             // max deviation from template  

    // compute max deviation from template
     
    if (size == LARGE) {
      deviation = (int)(MAX_RADIUS/AVAR);
    } 
    else {
      // else a small or medium size asteroid
      //   so reduce max deviation by the corresponding amount
      int factor = size*SIZE_RATIO;
      deviation = (int)(MAX_RADIUS/factor/AVAR);
    }

    // compute the first and final point of asteroid
    int fx =  
      (int)(GameMath.getRand(2*deviation)-deviation) +
	px[size][0]; 
    int fy = 
      (int)(GameMath.getRand(2*deviation)-deviation) + 
	py[size][0];      

    // close polygon
    tpx[0] = tpx[n-1] = fx;
    tpy[0] = tpy[n-1] = fy;


    // compute the rest of the points in asteroid
    for (int i = 1; i < n-1 ; i++) {
      tpx[i] =  
	(int)(GameMath.getRand(2*deviation)-deviation) +
	  px[size][i];
      tpy[i] =
	(int)(GameMath.getRand(2*deviation)-deviation) + 
	  py[size][i];

    }


  }

}



