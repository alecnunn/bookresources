abstract class cmAbstractRound extends cmAbstractVehicle{
   protected cmAbstractRound (fWorld w, fObject shooter, fPoint3d pos,
                              fVelocityVector v, double turningrate0,
                              double pitchrate0, double acceleration0,
                              double brakingrate0, double maxVelocity0,
                              double climbrate0, double decentrate0,
                              double pitchClimbrateRelation0,
                              double impactDamage)
   {
	   super(w,pos,v,turningrate0,pitchrate0,acceleration0,brakingrate0,
	         maxVelocity0,climbrate0,decentrate0,pitchClimbrateRelation0,
	         Double.MAX_VALUE);
      theShooter = shooter;
      this.impactDamage=impactDamage;
	}

   public fObject getShooter () {
      return theShooter;
   }

   protected boolean handleCollisionWith (fObject obj,double dt) {
      if(obj!=theShooter){
         die();
         return false;
      }
      return true;
   }

   public double getImpactDamage(){
      return impactDamage;
   }

	protected fObject theShooter;
	protected double impactDamage;
}

