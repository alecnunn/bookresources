
public class fVelocityVector extends fPoint3d{
	public fVelocityVector (double Ay0, double Ax0, double velocity0) {
		Ay=Ay0; Ax=Ax0; velocity=velocity0;
		calculateComponents();
	}

   protected fVelocityVector(double Ay0,double Ax0, double velocity0, double x0, double y0, double z0){
		Ay=Ay0; Ax=Ax0; velocity=velocity0;
      x=x0; y=y0; z=z0;
   }

	public void setVelocity (double nv) {
		velocity=nv;
		calculateComponents();
	}

	public void synchronizeAngle (fAngle3d a) {
		a.set(Ax,Ay,0);
	}

	public double getVelocity () {
		return velocity;
	}

	public double getAngleAboutYaxis () {
		return Ay;
	}

	public void setAngleAboutYaxis (double a) {
		Ay=a;
		calculateComponents();
	}

	public void setAngleAboutXaxis (double a) {
		Ax=a;
		calculateComponents();
	}

	public void increaseAngleAboutYaxis (double da) {
		Ay+=da;
		calculateComponents();
	}

	public void increaseVelocity (double dv) {
		velocity+=dv;
		calculateComponents();
	}

	public void increasePitch (double da) {
		Ax+=da;
		calculateComponents();
	}


	public fPoint3d makeClone () {
		return new fVelocityVector(Ay,Ax,velocity,x,y,z);
	}

	public fAngle3d getAngle () {
		return new fAngle3d(Ax,Ay,0);
	}

	public void set (fPoint3d p) {
		super.set(p);
		if(p instanceof fVelocityVector){
		   fVelocityVector pp=(fVelocityVector)p;
		   Ay=pp.Ay;
		   Ax=pp.Ax;
		   velocity=pp.velocity;
		}
	}

	public void negate(){
	   velocity=-velocity;
	   super.negate();
	}

	public String toString () {
		String str=super.toString();
		return str+" "+Ay+" "+Ax+" "+velocity+" ";
	}

	//----------
	//-- vill ha polär.
	//------------
	private void calculateComponents () {
	   z=-velocity; x=y=0;
	   rotateAboutXaxis(-Ax);
	   rotateAboutYaxis(Ay);
	}

	protected double Ay;
	protected double Ax;
	protected double velocity;
}

