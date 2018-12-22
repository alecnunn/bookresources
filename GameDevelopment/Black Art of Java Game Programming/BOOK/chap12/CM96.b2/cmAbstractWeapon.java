abstract class cmAbstractWeapon extends Object {
   protected cmAbstractWeapon(cmAbstractVehicle host,fPoint3d relPos,double loadingTime0,int ammo0){
      loadingTime=loadingTime0;
      ammo=ammo0;
      theHost=host;
      relOrigin=relPos;
   }

   public void addAmmo(int nbr){
      ammo+=nbr;
   }
   
   public void update(double dt){
      lastFire-=dt;
      if(lastFire<0){
         lastFire=0;
      }
   }
   
   public boolean fire(){
      if(lastFire>0) return false;
      if(ammo<=0) return false;
      ammo--;
      lastFire=loadingTime;
      return true;
   }

   public int getAmmo(){
      return ammo;
   }

   public String getName(){
      return null;
   }

   protected fPoint3d relOrigin;
   protected double loadingTime;
   protected double lastFire;
   protected int ammo;
   protected cmAbstractVehicle theHost;
}


