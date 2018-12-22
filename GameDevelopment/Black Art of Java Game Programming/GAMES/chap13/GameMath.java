
/////////////////////////////////////////////////////////////////
//
// the GameMath class contains mathematical routines for our
// graphics and games programs
//
/////////////////////////////////////////////////////////////////


  public final class GameMath {        
  
    // constants
    static final float DEG_TO_RAD = (2.0f*3.14159f)/360.0f;
    static final double RAD_TO_DEG = 1.0/DEG_TO_RAD;

    static final int TABLE_SIZE = 360;

    // storage for lookup tables
    static float cos_table[];
    static float sin_table[];
  
/////////////////////////////////////////////////////////////////
// initialize cos and sin tables
/////////////////////////////////////////////////////////////////

    static {
 	cos_table  = new float[TABLE_SIZE];
	sin_table  = new float[TABLE_SIZE];
	
	double temp;
	for (int i=0; i<TABLE_SIZE; i++) {
	  temp = DEG_TO_RAD*(double)i;
	  cos_table[i] = (float)Math.cos(temp);
	  sin_table[i] = (float)Math.sin(temp);
	}

    }

/////////////////////////////////////////////////////////////////
// return value of cos from lookup table
/////////////////////////////////////////////////////////////////

    public static float cos(int degree) {
        if (degree >= 360) {
	  degree = degree % 360;
	}
	else if (degree < 0) {
	  degree = (-degree)%360;
	}
	 
	return cos_table[degree];
    }

/////////////////////////////////////////////////////////////////
// return value of sin from lookup table
/////////////////////////////////////////////////////////////////

    public static float sin(int degree) {
        if (degree >= 360) {
	  degree = degree % 360;
	}
	else if (degree < 0) {
	  degree = (-degree)%360;
	}
	return sin_table[degree];
    }
/////////////////////////////////////////////////////////////////
// computes angle, in degrees, from the x-axis
// returns angle in degrees between 0.0 and 360.0
// where 90.0 degrees is the y-axis.
/////////////////////////////////////////////////////////////////

  public static double computeAngle(int v1x,int v1y) {
    double t= Math.atan2((double) v1y, (double)v1x);
    t *= RAD_TO_DEG;
    if (t < 0.0) t += 360.0;
    return (double)t;
  }

/////////////////////////////////////////////////////////////////
// compute magnitude of a vector
/////////////////////////////////////////////////////////////////

    public static float computeMagnitude(int v1x,int v1y) {
      return (float)Math.sqrt((double)(v1x*v1x+v1y*v1y)); 
    }

/////////////////////////////////////////////////////////////////
// return random numbers of specified type between 0 and Max
/////////////////////////////////////////////////////////////////
    public static int getRand(int Max) {
      return (int)(Math.random() * Max);
    }

    public static float getRand(float Max) {
      return (float)(Math.random() * Max);


    }

    public static double getRand(double Max) {
      return (Math.random() * Max);


    }
 

}
