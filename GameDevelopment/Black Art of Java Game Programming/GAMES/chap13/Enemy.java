import java.awt.*;
import java.applet.*;

/////////////////////////////////////////////////////////////////
//
// Enemy sprite class
//
/////////////////////////////////////////////////////////////////

public class Enemy extends MoveablePolygon {
  public int value;   // value of this enemy

  public Enemy(int tx[], int ty[], int n, 
	       int centerx, int centery, Color c,
	       int w, int h, int r,int v) {
    super(tx,ty,n,centerx,centery,c,w,h,r); 
    value = v;
    setFill(false);  // don't fill polygon
  }

/////////////////////////////////////////////////////////////////
// intersection routines
/////////////////////////////////////////////////////////////////

  // check for intersection with fire: 
  //    compute midpt of the fire
  //    and see if it's within the max_radius of this enemy
  public boolean intersect(Fire f) {
    if (isActive() && f.isActive()) {
      int midptx = (f.p.xpoints[1] + f.p.xpoints[0]) / 2;
      int midpty = (f.p.ypoints[1] + f.p.ypoints[0]) / 2;
      return (Math.abs(midptx - locx) < max_radius) &&
	(Math.abs(midpty - locy) < max_radius);
    }
    else
      return false;
  }

  // check for intersection with Ship
  //    see if ship's center is within max_radius of this
  //    asteroid, with 2 pixels of leeway on each side
  public boolean intersect(Ship s) {
    return isActive() && s.isActive() &&
      (Math.abs(s.locx - locx+2) < max_radius) &&
	(Math.abs(s.locy - locy+2) < max_radius) ;
  }

}

  
