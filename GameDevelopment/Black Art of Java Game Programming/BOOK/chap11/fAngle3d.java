
public class fAngle3d{
	double x;
	double y;
	double z;
	fAngle3d (double x0, double y0, double z0) {
		x=x0; y=y0; z=z0;
	}

	fAngle3d () {
		x=y=z=0;
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

	void times (double s) {
		x*=s; y*=s; z*=s;
	}

	void negate () {
		x=-x; y=-y; z=-z;
	}

	public String toString () {
		return new String("("+x+", "+y+", "+z+")");
	}

}

