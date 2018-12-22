
import java.awt.*;

/////////////////////////////////////////////////////////////////
//
// RotateablePolygon: encapsulates a MoveablePolygon that also
//                    rotates      
/////////////////////////////////////////////////////////////////

class RotateablePolygon extends MoveablePolygon {

  // variables:

  protected float magnitude[];          // magnitudes of vectors
  protected int theta = 0;              // current angle of
                                        //     orientation
  protected int angle[];                // angles of vectors
                                        // (offset from theta)
  protected int rate = 0;               // rate of rotation


  // methods:

/////////////////////////////////////////////////////////////////
// Constructor: take local vectors, center,color,screen bounds,
//              and approximate max radius of polygon
/////////////////////////////////////////////////////////////////

  public RotateablePolygon(int tx[], int ty[], int n, 
			   int centerx, int centery, Color c,
			   int w, int h, int r) {

                                                
    super(tx,ty,n,centerx,centery,c,w,h,r);           
    magnitude = new float[n];
    angle = new int[n];

    // compute magnitude and angle for each local vector
    // this is the Cartesian ==> Polar coordinate transformation
    for (int i=0; i<n; i++) {
      magnitude[i] = 
	(float)GameMath.computeMagnitude(tx[i],ty[i]);

      angle[i] = 
	(int)Math.round(GameMath.computeAngle(tx[i],ty[i]));
    }

  }


/////////////////////////////////////////////////////////////////
// set the angle of orientation
/////////////////////////////////////////////////////////////////

  public void setAngle(int a) {
    theta = a;
  }

/////////////////////////////////////////////////////////////////
// set rate (in degrees) for continuous rotation
/////////////////////////////////////////////////////////////////

  public void setRotationRate(int r) {
    rate = r;
  }

/////////////////////////////////////////////////////////////////
// update the angle of orientation by the rotation rate
/////////////////////////////////////////////////////////////////

  public void updateAngle() {
    theta = checkAngleBounds(theta + rate);
  }

/////////////////////////////////////////////////////////////////
// update the angle of orientation with a parameter
/////////////////////////////////////////////////////////////////

  public void updateAngle(int a) {
    theta = checkAngleBounds(theta + a);
  }


/////////////////////////////////////////////////////////////////
// check that th is between 0 to 359
/////////////////////////////////////////////////////////////////

  public int checkAngleBounds(int th) {
    if (th >= 360)
      th -= 360;
    else if (th < 0)
      th += 360;
    return th;
  }

/////////////////////////////////////////////////////////////////
// rotate polygon by 'a' degrees.
// this is accomplished by using the polar representation
// of the polygon, then converting back to Cartesian coordinates
/////////////////////////////////////////////////////////////////

  public void rotate(int a) {

    double radians;
    
    // compute new angle of orientation
    theta = checkAngleBounds(theta + a); 

    // for each offset vector, 
    //   compute the new angle that its facing
    //   compute the coords associated with this angle
    //   update the polygon representation

    for (int i=0; i<p.npoints; i++) {
      double tempx, tempy;

      radians = GameMath.DEG_TO_RAD * 
	checkAngleBounds(theta + angle[i]);

      tempx = (magnitude[i]*Math.cos(radians)) ;
      tempy = (magnitude[i]*Math.sin(radians)) ;
      tx[i] = (int)Math.round(tempx);
      p.xpoints[i] = (int)Math.round(tempx + (double)locx);
      ty[i] = (int)Math.round(tempy);
      p.ypoints[i] = (int)Math.round(tempy + (double) locy);
      
    }

  }

/////////////////////////////////////////////////////////////////
// default update -- assumes continuous rotation // 
/////////////////////////////////////////////////////////////////
  public void update() {
    updatePosition();            // update location of sprite
    rotate(rate);                // rotate the sprite
    updateAngle();               // update the orientation
  }
}
