import java.awt.*;
import java.applet.*;

/////////////////////////////////////////////////////////////////
//
// Fire sprite class: this sprite automatically after a
//                    certain number of updates
//
/////////////////////////////////////////////////////////////////

public class Fire extends MoveablePolygon {
  

  // fire is a polygon with two points, i. e. a straight line
  static final int firex[] = { 0, 0};
  static final int firey[] = { 0, 0};

  // length of the fire sprite
  static final int DEFAULT_LENGTH = 23;
  int fire_length = DEFAULT_LENGTH;

  // fire sprite lasts for this many updates
  int max_updates = 14;

  // count the number of updates
  int count = 0;


/////////////////////////////////////////////////////////////////
// Fire sprite constructors
/////////////////////////////////////////////////////////////////

  public Fire(Color c,int w, int h ) {
    super(firex,firey,2,
	  0,0,c,
	  w,h,DEFAULT_LENGTH);           
    setFill(false);
  }


  public Fire(Color c,int w, int h,int length,int updates ) {
    super(firex,firey,2,0,0,c,w,h,length);        
    fire_length = length;
    max_updates = updates;
    setFill(false);
  }

/////////////////////////////////////////////////////////////////
//initialize a fire sprite from x,y at specified angle
/////////////////////////////////////////////////////////////////

  public void initialize(int x, int y, int angle) {
    tx[1] = (int)Math.round((fire_length*GameMath.cos(angle)));
    ty[1] = (int)Math.round((fire_length*GameMath.sin(angle)));
    setPosition(x,y);
    setVelocity(tx[1],ty[1]);
    count = 0;
    restore();
  }

/////////////////////////////////////////////////////////////////
// update fire sprite: bump counter, suspend sprite if
//                     counter's high enough
/////////////////////////////////////////////////////////////////

  public void update() {
    if (isActive()) {
      count++;
      if (count == max_updates) {
	suspend();
      }
      else 
	super.update();
    }
  }

  // check if fire intersects the ship
  public boolean intersect(Ship s) {
    int midptx = (p.xpoints[1] + p.xpoints[0]) / 2;
    int midpty = (p.ypoints[1] + p.ypoints[0]) / 2;
    return isActive() && s.isActive() &&
      (Math.abs(s.locx - midptx+2) < s.max_radius) &&
	(Math.abs(s.locy - midpty+2) < s.max_radius) ;
  }

}
    
