import java.awt.*;

/////////////////////////////////////////////////////////////////
// 
// Ship sprite: adds a few methods to RotateablePolygon 
//
/////////////////////////////////////////////////////////////////


public class Ship extends RotateablePolygon {

  // default rate of rotation is 15 degrees
  static final int ROT_RATE = 15 ;

  // has ship rotated?
  boolean rotated = false;                 

  // actual amount of rotation
  int rotation_amount;




/////////////////////////////////////////////////////////////////
// Ship constructor
/////////////////////////////////////////////////////////////////

  public Ship(int tx[], int ty[], int n,   // coordinates 
	      int centerx, int centery,    // location
	      Color c,                     // color
	      int w, int h,                // screen bounds 
	      int r) {                     // ship length

    super(tx,ty,n,centerx,centery,c,w,h,r);           
                       
    // align the nose of the ship with theta = 0
    // this is necessary because the direction of ship's
    // thrust is based on theta, so the angles must be
    // rotated so that the angle of the ship's nose
    // and theta match.
    for (int i=0 ; i < angle.length; i++) {
      angle[i] = checkAngleBounds(angle[i]-90);
    }
    rotate(-90);                           // initially ship
                                           //   faces up
  }



/////////////////////////////////////////////////////////////////
//
// Rotation methods: called by ShipManager
//
/////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////
// rotate left => decreasing theta in screen coordinate system
/////////////////////////////////////////////////////////////////
  public void rotateLeft() {
    rotated = true;
    rotation_amount = -ROT_RATE;
  }


/////////////////////////////////////////////////////////////////
// rotate right => increasing theta in screen coordinate system
/////////////////////////////////////////////////////////////////
  public void rotateRight() {
    rotated = true;
    rotation_amount = ROT_RATE;
  }


/////////////////////////////////////////////////////////////////
// override update to prevent continuous rotation
/////////////////////////////////////////////////////////////////

  public void update() {                 
    if (isActive()) {
      updatePosition();

      if (rotated) {
	rotate(rotation_amount);
	rotated = false;
      }
      else {
	updatePoints();
      }

    }
  }


/////////////////////////////////////////////////////////////////
//
// handle ship thrusting:
//   update ship's velocity based on direction it's pointed
//
/////////////////////////////////////////////////////////////////

  static final double ACCEL_FACTOR = 1.0;

  public void thrust() {

    vx = (int)Math.round((double)vx + 
			 GameMath.cos(theta)*ACCEL_FACTOR);
    vy = (int)Math.round((double)vy + 
			 GameMath.sin(theta)*ACCEL_FACTOR);

  }

}

  
