
import java.applet.*;
import java.awt.*;

/////////////////////////////////////////////////////////////////
public class UFO extends BitmapLoop implements Intersect {
  
  byte state;
  
  // UFO states
  static final byte STANDBY = 0;
  static final byte ATTACK = 1;
  static final byte RETREAT = 2;
  static final byte LAND = 3;
  static final byte EXPLODE = 4;

  // probability of state transitions
  static final double STANDBY_EXIT = .95;
  static final double ATTACK_EXIT = .95;
  static final double RETREAT_EXIT = .95;
  static final double LAND_EXIT = .95;
  static final double FLIP_X = 0.9;
  static final int RETREAT_Y = 33;

  // bitmap animations
  protected Image ufo[];             // ufo animation
  protected Image attack[];          // attack animation
  protected Image explode[];         // explosion sequence
  
  // instance vars
  int max_x, max_y;                  // max coords of this UFO
  int explosion_counter;             // counter for explosion 
                                     //   bitmaps
  UFOManager um;

  // class vars
  static Intersect target;           // refers to the gun
  static int gun_y;                  // the y-coord of gun
  static GameManager game;           // ptr to game manager

  // constructor: initialize image references, instance vars
  public UFO(Image ufoImages[],
	     Image attackImages[],
	     Image explodeImages[],
	     int max_x,int max_y,
	     UFOManager um,
	     Applet a) {
    super(0,0,null,ufoImages,a);
    this.max_x = max_x;
    this.max_y = max_y;
    currentImage = getRand(ufoImages.length);
    ufo = ufoImages;
    attack = attackImages;
    explode = explodeImages;
    game = (GameManager)a;
    this.um = um;
    startStandby();

  }

  // finish initializing info about the player's gun
  //   this way, the ufo can communicate with the gun
  static public void initialize(GunManager gm) {
    target = gm.getGun();            // refers to gun sprite
    gun_y = gm.getGunY();            // get gun y-coordinate
  }

  // implement Intersect interface:
  public boolean intersect(int x1,int y1,int x2,int y2) {
    
    return visible && (x2 >= locx) && (locx+width >= x1)
      && (y2 >= locy) && (locy+height >= y1);
    
  }

  // this is called if a missile hits the alien
  public void hit() {
    // alien is invulnerable when it's attacking
    //   but it gets "pushed back"
    if (state == ATTACK) {
      locy -= 17;
    }
    // otherwise explode!
    else if (state != EXPLODE) {
      startExplode();              // start explode state
      game.incrementScore();       // add to score
      um.killed();                 // tell UFOManager
                                   //  another UFO's dead
    }
    
  }


  // set state and images loop
  public void init() {
    startStandby();
    images = ufo;                  
    restore();
  }
    
  // this implements the state machine
  public void update() {

    // if alien hits target, notify target, and explode if
    //    it's not in attack or explode mode
    //    otherwise retreat
    if ((locy + height >= gun_y) && state != EXPLODE &&
	target.intersect(locx,locy,locx+width,locy+height)) {
	target.hit();
	if (state != ATTACK ) {
	  startExplode();
	  return;
	}
	else {
	  startRetreat();
	}
      }

    // otherwise, update alien state
    
    double r1 = Math.random();
    double r2 = Math.random();
    switch (state) {
    case STANDBY:
      if (r1 > STANDBY_EXIT) {
	if (r2 > 0.5) {
	  startAttack();
	}
	else {
	  startLand();
	}

      }
      // flip ufo's x-direction if it goes too far right
      //   or left, or if random variable passes threshold
      else if ((locx < width) || (locx > max_x - width) ||
	  (r2 > FLIP_X)) {
	vx = -vx;
      }
      break;
    case ATTACK:

      if ((r1 > ATTACK_EXIT) || (locy > gun_y - 17)) {
	startRetreat();
      }
	
      else if ((locx < width) || (locx > max_x - width) ||
	       (r2 > FLIP_X)) {
	vx = -vx;
      }

      break;
    case RETREAT:
      if (r1 > RETREAT_EXIT) {
	if (r2 > 0.5) {
	  startAttack();
	}
	else {
	  startStandby();
	}
      }
      else if (locy < RETREAT_Y) { 
	startStandby();
      }
      break;
    case LAND:

      if (r1 > LAND_EXIT) {
	startStandby();
      }
      else if (locy >= max_y-height) {
	landingRoutine();
      }
      break;

    case EXPLODE:
      explosion_counter++;     // bump counter

                               // suspend once animation 
                               //   is finished  
      if (explosion_counter == explode.length) {
	suspend();             
      }

    }

    super.update();            // call BitmapLoop's update()
  }

  // when the alien lands successfully
  protected void landingRoutine() {
    game.alienLanded();        // tell game manager that 
                               //   the UFO's landed
    suspend();
  }

  // start standby state
  protected void startStandby() {
    vx = getRand(8)-4 ;
    vy = 0;
    state = STANDBY;
  }

  // start attack state
  protected void startAttack() {
    vx = getRand(10)-5;       
    vy = getRand(5)+7;
    images = attack;
    state = ATTACK;
  }

  // start retreating state
  protected void startRetreat() {
    vx = 0;
    vy = -getRand(3) - 2;
    images = ufo;
    state = RETREAT;
  }

  // start landing state
  protected void startLand() {
    vx = 0;
    vy = getRand(3) + 2;
    state = LAND;
  }

  // start explosion state
  protected void startExplode() {
    images = explode;        // set bitmap to explosion sequence
    currentImage = 0;        // start at beginning of animation
    explosion_counter = 0;   // count the number of frames
    um.playExplosion();      // play explosion sound
    state = EXPLODE;

  }

  // return a random int from 0 to x
  static public int getRand(int x) {
    return (int)(x * Math.random());
  }
}
