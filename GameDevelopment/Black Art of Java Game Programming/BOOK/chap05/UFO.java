
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

  // probability of state transitions
  static final double STANDBY_EXIT = .95;
  static final double ATTACK_EXIT = .95;
  static final double RETREAT_EXIT = .95;
  static final double LAND_EXIT = .95;
  static final double FLIP_X = 0.9;
  static final int RETREAT_Y = 17;

  static protected Image images[];   // now shared by the class
  int max_x, max_y;                  // max coords of this UFO
  static Intersect target;           // refers to the gun
  static int gun_y;                  // the y-coord of gun

  public UFO(Image ufoImages[],int max_x,int max_y,
	    Applet a) {
    super(0,0,null,ufoImages,a);
    this.max_x = max_x;
    this.max_y = max_y;
    currentImage = getRand(5);
    startStandby();

  }

  // finish initializing info about the player's gun
  static public void initialize(GunManager gm) {
    target = gm.getGun();            // refers to gun sprite
    gun_y = gm.getGunY();            // get gun y-coordinate
  }

  // implement Intersect interface:

  public boolean intersect(int x1,int y1,int x2,int y2) {
    
    return visible && (x2 >= locx) && (locx+width >= x1)
      && (y2 >= locy) && (locy+height >= y1);
    
  }

  public void hit() {
    // alien is invulnerable when it's attacking
    //  otherwise, suspend the sprite
    if (state != ATTACK) {
      suspend();
    }
  }


  // this implements the state machine
  public void update() {

    // if alien hits target

    if ((locy + height >= gun_y) &&
	target.intersect(locx,locy,locx+width,locy+height)) {
	target.hit();
	suspend();
	return;
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


    }

    super.update();
  }

  protected void landingRoutine() {
    System.out.println("ufo landed") ;
    suspend();
  }

  protected void startStandby() {
    vx = getRand(8)-4 ;
    vy = 0;
    state = STANDBY;
  }

  protected void startAttack() {
    vx = getRand(10)-5;       
    vy = getRand(5)+4;
    state = ATTACK;
  }

  protected void startRetreat() {
    vx = 0;
    vy = -getRand(3) - 2;
    state = RETREAT;
  }

  protected void startLand() {
    vx = 0;
    vy = getRand(3) + 2;
    state = LAND;
  }

  static public int getRand(int x) {
    return (int)(x * Math.random());
  }
}
