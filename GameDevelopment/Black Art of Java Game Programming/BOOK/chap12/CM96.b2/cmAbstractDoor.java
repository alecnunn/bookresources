abstract class cmAbstractDoor extends cmAbstractMovingStructure{
   cmAbstractDoor (fWorld w, fPoint3d p, fAngle3d a, double speedUp0,
      double speedDown0, double waitTimeUp0, double height0)
   {
      super(w,p,a,new fPoint3d(0,0,0),new fAngle3d(0,0,0));
      state = 0;
      originalPos = p.y;
      speedUp = speedUp0;
      speedDown = speedDown0;
      height = height0;
      waitTimeUp = waitTimeUp0;
   }
	
   public boolean interestedOfCollisionWith(fObject obj){
      if(obj instanceof cmAbstractVehicle) return true;
      else return super.interestedOfCollisionWith(obj);
   }
  	
   protected boolean handleCollisionWith (fObject obj,double dt) {
      if (state == 0){
         // Start move up
         fWorld.debug(this,"open");
         state = 1;
      }
      return false;
   }
	
   public void update (double dt) {
      super.update(dt);
	
      if (state == 1){
         // Start moving up
         fPoint3d p=getdPosition();
         p.y = speedUp*dt;
         setdPosition(p);
         state = 2;
      } else if (state == 2){
         // Moving up
         fPoint3d p=getPosition();
         if (p.y-height > originalPos){
            p=getdPosition();
            p.y = 0;
            setdPosition(p);
            state = 3;
            counter = 0;
         }
      } else if (state == 3){
         // Wait
         counter = counter+dt;
         if (counter > waitTimeUp){
            fPoint3d p=getdPosition();
            p.y = -speedDown*dt;
            setdPosition(p);
            state = 4;
         }
      } else if (state == 4){
         // Moving down
         fPoint3d p=getPosition();
         if (p.y < originalPos){
            p=getdPosition();
            p.y=0;
            setdPosition(p);
            state = 0;
         }
      }
	}

	private double originalPos;
	private int state;
	private double speedUp;
   private double speedDown;
   private double waitTimeUp;
   private double height;
   private double counter;
}

