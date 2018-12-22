abstract class cmAbstractStaticObject extends fObject{
   protected cmAbstractStaticObject (fWorld w, fPoint3d pos, fAngle3d ang) {
      super(w,pos,ang);
      myHealth=Double.MAX_VALUE;
   }

   protected cmAbstractStaticObject (fWorld w, fPoint3d pos, fAngle3d ang,double health) {
	   super(w,pos,ang);
      myHealth=health;
   }

   public double getHealth(){
      return myHealth;
   }

   public void setHealth(double e){
      myHealth=e;
   }
   
   protected double myHealth;
}
