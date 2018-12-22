abstract class cmAbstractMovingStructure extends cmAbstractMovingObject{
   protected cmAbstractMovingStructure (fWorld w, fPoint3d p, fAngle3d a,
                                        fPoint3d dp, fAngle3d da)
   {
      super(w,p,a,dp,da);
      myHealth=Double.MAX_VALUE;
   }
   protected cmAbstractMovingStructure (fWorld w, fPoint3d p, fAngle3d a,
                                        fPoint3d dp, fAngle3d da,double health)
   {
      super(w,p,a,dp,da,health);
   }

   public boolean interestedOfCollisionWith (fObject obj) {
	   if(obj instanceof cmAbstractRound) return true;
	   return super.interestedOfCollisionWith(obj);
	}

   protected boolean handleCollisionWith (fObject obj,double dt) {
      if(obj instanceof cmAbstractRound){
         myHealth-=((cmAbstractRound)obj).getImpactDamage();
         if(myHealth<0) {
            die();
         }
      }
      return super.handleCollisionWith(obj,dt);
   }

}

