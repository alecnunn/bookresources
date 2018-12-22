abstract class cmAbstractPlayer extends cmAbstractVehicle{
   protected cmAbstractPlayer (fWorld w, fPoint3d pos, fVelocityVector v,
                               double turningrate, double pitchrate,
                               double acceleration, double brakingrate,
                               double maxVelocity, double climbrate,
                               double decentrate, double pitchClimbrateFactor,
                               double health)
   {
      super(w,pos,v,turningrate,pitchrate,acceleration,brakingrate,
		      maxVelocity,climbrate,decentrate,pitchClimbrateFactor,
		      health);
   }

   protected cmAbstractPlayer(fWorld w, fPoint3d pos, fVelocityVector v,
	                           double turningrate, double acceleration,
	                           double brakingrate, double maxVelocity,
	                           double health)
   {
      super(w,pos,v,turningrate,acceleration,brakingrate,maxVelocity,health);
   }

	public void update (double dt) {
  		super.update(dt);
		if(myBrain!=null){
		   myBrain.update(dt);
		}
	}

   public void setBrain(cmAbstractBrain brain){
      myBrain=brain;
   }

   public cmAbstractBrain getBrain(){
      return myBrain;
   }
      
   protected cmAbstractBrain myBrain;
}

