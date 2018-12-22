
import java.applet.*;
import java.awt.*;

/////////////////////////////////////////////////////////////////
public class GunManager {

  private GunSprite gun;               // your gun
  private int gun_width;               // width of gun
  private int gun_height;
  private MissileSprite missile;       // missile
  static int width, height;            // applet dimensions
  private int min_x,max_x;             // min and max x coords
                                       //   for gun movement
  private int gun_min_x,gun_max_x; 
  private int mis_min_x,mis_max_x;
  private int gun_y;
   
  static final int MISSILE_WIDTH = 3;
  static final int MISSILE_HEIGHT = 27;
  static final int MISSILE_SPEED = -27; // missile flies upward
  static final Color MISSILE_COLOR= Color.red;

  public GunManager(int width,int height,
		    Image gunImage,Intersect target[],Applet a) {
    this.width = width;
    this.height = height;
    gun = new GunSprite(gunImage,a);

    gun_width = gunImage.getWidth(a)/2;
    gun_height = gunImage.getHeight(a);
    System.out.println(gun_width);

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
				height-gun_height,
				0,target);
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
	missile.init(x-2);             // initialize missile
      }
    }
  }

  // update all the parameters associated with the
  //   gun. In this case, only the missile needs to move
  //   automatically. Also the gun manager checks if the
  //   missile hits anything

  public void update() {
    missile.update();
  }

  // paint all sprites associated with gun
  public void paint(Graphics g) {
    gun.paint(g);
    missile.paint(g);
  }

  // accessor function for gun
  public GunSprite getGun() {
    return gun;
  }

  public int getGunY() {
    return gun_y;
  }

}

class GunSprite extends BitmapSprite 
  implements Moveable,Intersect {

  public GunSprite(Image i, Applet a) {
    super(i,a);
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


  public void hit() { 
    System.out.println("HIT!");
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












