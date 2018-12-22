
import java.applet.*;
import java.awt.*;

/////////////////////////////////////////////////////////////////
public class GunManager {

  private GunSprite gun;               // your gun
  private int gun_width;               // width of gun
  private int gun_height;
  private MissileSprite missile;       // missile
  private int min_x,max_x;             // min and max x coords
                                       //   for gun movement
  private int gun_min_x,gun_max_x;     
  private int mis_min_x,mis_max_x;
  private int gun_y;

  private boolean displayHit;
  private int energy;

  private int maxEnergy;
  private int energyDec;
  
  private GameManager game;            // ptr to game manager
  static int width, height;            // applet dimensions

  static final int ENERGY_PER_HIT = 5; // energy used per hit
   
  static final int MISSILE_WIDTH = 3;
  static final int MISSILE_HEIGHT = 27;
  static final int MISSILE_SPEED = -27; // missile flies upward
  static final Color MISSILE_COLOR= Color.red;

  public GunManager(int maxEnergy,int energyDec,int width,int height,
		    Image gunImage,Intersect target[],Applet a) {
    this.maxEnergy = maxEnergy;
    this.energyDec = energyDec;
    this.width = width;
    this.height = height;
    gun = new GunSprite(gunImage,a,this);

    gun_width = gunImage.getWidth(a)/2;
    gun_height = gunImage.getHeight(a);

    gun_y = height - gun_height;
    min_x = gun_width;
    max_x = width - gun_width;
    gun_min_x = 0;
    gun_max_x = width - 2*gun_width;
    mis_min_x = min_x-2;
    mis_max_x = max_x-2;
    gun.setPosition(width/2-gun_width,gun_y);
    missile = new MissileSprite(MISSILE_WIDTH,MISSILE_HEIGHT,
				MISSILE_COLOR,MISSILE_SPEED,
				height-gun_height+13,
				0,target);

    game = (GameManager)a;              // set ptr to GameManager
  }

  // set parameters for the new game
  public void newGame() {
    gun.setPosition(width/2-gun_width,gun_y);
    gun.restore();
    displayHit = false;               
    energy = maxEnergy;
    
  }


  // move gun to the given x coordinate
  public void moveGun(int x) {
    if (x <= min_x) {
      gun.setPosition(gun_min_x,gun_y);
      }
    else if (x >= max_x) {
      gun.setPosition(gun_max_x,gun_y);
      }
    else {
      gun.setPosition(x-gun_width,gun_y);
    }
  }

  // fire missile from given x coordinate
  public void fireMissile(int x) {
    if (!missile.isActive()) {     // if missile sprite
                                   //   isn't active
      if (x <= min_x) {
	missile.init(mis_min_x);
      }
      else if (x >= max_x) {
	missile.init(mis_max_x);
      }
      else {
	missile.init(x-1);             // initialize missile
      }
    }
  }

  // update all the parameters associated with the
  //   gun. In this case, only the missile needs to move
  //   automatically. 

  public void update() {
    missile.update();
  }


  // paint all sprites associated with gun
  // also paint status display for amount of energy left

  String energyString = "Energy";
  public void paint(Graphics g) {
    // if gun is hit, flash a red rectangle
    //   instead of painting gun
    if (displayHit) {
      g.setColor(Color.red);
      
      g.fillRect(0,gun_y,width,gun_height);
      displayHit = false;
    }
    else {
      gun.paint(g);
    }
    missile.paint(g);

    // display energy left
    g.setColor(Color.red);
    g.drawString(energyString,3,13);
    g.fillRect(0,17,energy,10);
  }

  // accessor function for gun
  public GunSprite getGun() {
    return gun;
  }
  // get the y-coordinate of the gun
  public int getGunY() {
    return gun_y;
  }

  // handles a hit from an alien
  public void handleHit() {
    displayHit = true;             // set display hit flag
    energy -= energyDec;           // update energy
    if (energy <= 0) {             // game over if energy <= 0
      game.gameOver();             // notify game manager
      gun.suspend();               // turn off sprites
      missile.suspend();
    }
  }

  public void setEnergyDec(int dec) {
    energyDec = dec;
  }

}

class GunSprite extends BitmapSprite 
implements Moveable,Intersect {
  
  protected GunManager gm;         // pointer to manager class
    
  public GunSprite(Image i, Applet a,GunManager gm) {
    super(i,a);
    this.gm = gm;
  }

  // the following methods implement Moveable:

  public void setPosition(int x,int y) {
    locx = x;
    locy = y;
  }

  public void setVelocity(int x,int y) {

  }

  public void updatePosition() {

  }

  // the following methods implement Intersect:
  
  // compare bounding boxes
  public boolean intersect(int x1,int y1,int x2,int y2) {
    
    return visible && (x2 >= locx) && (locx+width >= x1)
      && (y2 >= locy) && (locy+height >= y1);
    
  }


  // tell applet to display the hit
  public void hit() { 

    gm.handleHit();           // notify manager of hit

  }
}

/////////////////////////////////////////////////////////////////
class RectSprite extends Sprite2D {

  protected int width, height;    // dimensions of rectangle

  public RectSprite(int w,int h,Color c) {
    locx = 0;
    locy = 0;
    width = w;
    height = h;
    color = c;
    restore();
  }
  
  public RectSprite(int x,int y,int w,int h,Color c) {
    locx = x;
    locy = y;
    width = w;
    height = h;
    color = c;
    fill = false;                 // default: don't fill
    restore();                    // restore the sprite
  }

  // provide implementation of abstract methods:

  public void update() {
   
    // does nothing

  }


  // check if sprite's visible before painting
  public void paint(Graphics g) {
    if (visible) {
      g.setColor(color);

      if (fill) {
	g.fillRect(locx,locy,width,height);
      }

      else {
	g.drawRect(locx,locy,width,height);
      }

    }

  }

}

/////////////////////////////////////////////////////////////////
class MissileSprite extends RectSprite  {
  protected int vy;           // velocity in y coordinate
  protected int start_y;      // starting y coord
  protected int stop_y;       // stop at y coord
  Intersect target[];
  public MissileSprite(int w,int h,Color c,int vy,
		       int start_y,int stop_y,
		       Intersect target[]) {
    super(w,h,c);
    setFill(true);            // fill rect sprite
    this.vy = vy;             // initialize speed
    this.start_y = start_y;   // initialize starting point
    this.stop_y = stop_y;     // initialize stopping point
    this.target = target;     // initialize targets
    suspend();
  }

  // start the missile at the given x coordinate
  public void init(int x) {
    locx = x;
    locy = start_y;
    restore();
  }

  public void update() {

    if (active) {

      // move missile
      locy += vy;
      if (locy < stop_y) {
	suspend();
	
      }
      // else if missile hits target, suspend it
      else {
	for (int i=0; i<target.length; i++) {
	  if (target[i].intersect(locx,locy,
				  locx+width,locy+height)) {

	    target[i].hit();  // tell target it's been hit

	    suspend();
	    break;
	  }
	}
      }
    }
  }

}

interface Intersect {
  public boolean intersect(int x1,int y1,int x2,int y2);
  public void hit();
}












