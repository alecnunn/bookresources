
public class fPoint3d{
	public double x;
	public double y;
	public double z;
	public fPoint3d (double x0, double y0, double z0) {
		x=x0; y=y0; z=z0;
	}

	public fPoint3d () {
		x=y=z=0;
	}

	public fPoint3d (fPoint3d p1, fPoint3d p2) {
		x=p2.x-p1.x; y=p2.y-p1.y; z=p2.z-p1.z;
	}

	public fPoint3d (fPoint3d p) {
		x=p.x; y=p.y; z=p.z;
	}

	void vectorProduct (fPoint3d p1, fPoint3d p2) {
		x=p1.y*p2.z-p1.z*p2.y;
		y=p1.z*p2.x-p1.x*p2.z;
		z=p1.x*p2.y-p1.y*p2.x;
	}

	void normalize (double length) {
		double t=length/Math.sqrt(x*x+y*y+z*z);
		x=t*x; y=t*y; z=t*z;
	}

	void plus (fPoint3d p) {
		x+=p.x; y+=p.y; z+=p.z;
	}

	double dotProduct (fPoint3d p) {
		return p.x*x+p.y*y+p.z*z;
	}

	boolean equals (fPoint3d p) {
		return (p.x==x)&&(p.y==y)&&(p.z==z);
	}

	void set (fPoint3d p) {
		x=p.x; y=p.y; z=p.z;
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

