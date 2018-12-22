abstract class cmAbstractMovingObject extends fMovingObject{
   protected cmAbstractMovingObject (fWorld w, fPoint3d pos,
                                     fAngle3d agl, fPoint3d dpos,
                                     fAngle3d dagl)
   {
      super(w,pos,agl,dpos,dagl);
      myHealth=Double.MAX_VALUE;
   }
   protected cmAbstractMovingObject (fWorld w, fPoint3d pos,
                                     fAngle3d agl, fPoint3d dpos,
                                     fAngle3d dagl,double health)
   {
	   super(w,pos,agl,dpos,dagl);
      myHealth=health;
   }
   public double getHealth(){
      return myHealth;
   }
   public void setHealth(double h){
      myHealth=h;
   }
   protected double myHealth;
}

