
public class fAngle3d{
	double x;
	double y;
	double z;

	public fAngle3d (double x0, double y0, double z0) {
		x=x0; y=y0; z=z0;
	}

	public fAngle3d () {
		x=y=z=0;
	}

	public fAngle3d (fAngle3d p1, fAngle3d p2) {
		x=p2.x-p1.x; y=p2.y-p1.y; z=p2.z-p1.z;
	}

	fAngle3d (fAngle3d a) {
		x=a.x; y=a.y; z=a.z;
	}

	boolean equals (fAngle3d a) {
		return (a.x==x)&&(a.y==y)&&(a.z==z);
	}

	void set (fAngle3d a) {
		x=a.x; y=a.y; z=a.z;
	}

   void set (double x0,double y0,double z0){
      x=x0; y=y0; z=z0;
   }

	void times (double s) {
		x*=s; y*=s; z*=s;
	}

	void negate () {
		x=-x; y=-y; z=-z;
	}

	void plus (fAngle3d p) {
		x+=p.x; y+=p.y; z+=p.z;
	}

	void plus (fAngle3d p,double factor) {
		x+=p.x*factor; y+=p.y*factor; z+=p.z*factor;
	}

	public String toString () {
		return new String("("+x+", "+y+", "+z+")");
	}

   public fAngle3d makeClone(){
      return new fAngle3d(x,y,z);
   }
   
}

