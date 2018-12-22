
import java.awt.*;

/////////////////////////////////////////////////////////////////
// 
// MoveablePolygon: encapsulates a moveable PolygonSprite
//
/////////////////////////////////////////////////////////////////

class MoveablePolygon extends PolygonSprite 
implements Moveable

{   

  protected int tx[],ty[];                     // offset vectors
  protected int vx;                            // velocity x;
  protected int vy;                            // velocity y;
  protected int height = 1000,
                width = 1000;           // default screen limits
  protected int max_radius = 0;         // max radius of polygon

  // methods:

/////////////////////////////////////////////////////////////////
// Constructor: construct with offset vectors
/////////////////////////////////////////////////////////////////

  public MoveablePolygon(int tx[], int ty[], int n, 
		       int centerx, int centery, Color c) {
    super(tx,ty,n,centerx,centery,c);           // constructor
    vx = vy = 0;
    this.tx = new int[n];
    this.ty = new int[n];
    for (int i=0; i<n; i++) {
      this.tx[i] = tx[i];                       // save offset vectors
      this.ty[i] = ty[i];
    }

  }


/////////////////////////////////////////////////////////////////
// Constructor: construct with offset vectors,
//              initialize screen bounds and max_radius
/////////////////////////////////////////////////////////////////



  public MoveablePolygon(int tx[], int ty[], int n, 
			 int centerx, int centery, Color c,
			 int w, int h, int r) {
    super(tx,ty,n,centerx,centery,c);           // constructor
    vx = vy = 0;
    this.tx = new int[n];
    this.ty = new int[n];
    for (int i=0; i<n; i++) {
      this.tx[i] = tx[i];                       // save offset vectors
      this.ty[i] = ty[i];
    }
    height = h;
    width = w;
    max_radius = r;
 
  }

/////////////////////////////////////////////////////////////////
// implement Moveable methods
/////////////////////////////////////////////////////////////////

  // move polygon to specifiedlocation
  public void setPosition(int x, int y) {
    locx = x;
    locy = y;

    // compute screen coords of polygon from local coords
    for (int i=0; i<p.npoints; i++) {
      p.xpoints[i] = locx + tx[i];
      p.ypoints[i] = locy + ty[i];
    }
  }
 

  // set velocity of polygon
  public void setVelocity(int x, int y) {
    vx = x;
    vy = y;
  }


  public void scale(double factor) {
    for (int i=0; i<p.npoints; i++) {   // scale offset vectors
      tx[i] = (int)Math.round(factor*tx[i]);
      ty[i] = (int)Math.round(factor*ty[i]);
    }
    updatePoints();
  }


  // update position of center of polygon. Wrap center
  //   to other side of screen if polygon exits screen bounds
  public void updatePosition() {
    locx += vx;
    locy += vy;

    // if center of polygon is off the rhs of display
    // move it to the left
    if (locx - max_radius > width) {
      locx -= (width + 2*max_radius);
    }

    // if center of polygon is off the lhs of display
    // move it to the right
    else if (locx < -max_radius ) {
      locx += (width + 2*max_radius);
    }

    // if center of polygon is off the bottom of display
    // move it to the top
    if (locy - max_radius > height) {
      locy -= (height + 2*max_radius);
    }

    // if center of polygon is off the top of display
    // move it to the bottom
    else if (locy < -max_radius ) {
      locy += (height + 2*max_radius);
    }
  }

/////////////////////////////////////////////////////////////////
// convert from local coords to screen coords of the 
//   polygon points
/////////////////////////////////////////////////////////////////


  public void updatePoints() {

    for (int i=0; i<p.npoints; i++) {
      p.xpoints[i] = locx + tx[i];
      p.ypoints[i] = locy + ty[i];

    }
  }

/////////////////////////////////////////////////////////////////
// default update method
/////////////////////////////////////////////////////////////////

  public void update() {
    if (isActive()) {
      updatePosition();      // move center
      updatePoints();        // compute polygon points
    }
  }

}


// Moveable interface. Note that the declarations public 
// and abstract are optional. All methods specified in an
// interface are automatically public and abstract!

interface Moveable {
  public abstract void setPosition(int x, int y);
  public abstract void setVelocity(int x, int y);
  public abstract void updatePosition();
}










