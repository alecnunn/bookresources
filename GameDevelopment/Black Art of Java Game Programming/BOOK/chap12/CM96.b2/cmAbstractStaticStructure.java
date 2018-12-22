abstract class cmAbstractStaticStructure extends cmAbstractStaticObject{
   protected cmAbstractStaticStructure (fWorld world, double x, double z,fAngle3d agl,
                                        double w, double b, double h)
   {
      super(world,new fPoint3d(x,h,z),agl);
      myHealth=Double.MAX_VALUE;
   }

   protected cmAbstractStaticStructure (fWorld world, double x, double z,fAngle3d agl,
                                        double w, double b, double h,
                                        double health)
   {
      super(world,new fPoint3d(x,h,z),agl,health);
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

