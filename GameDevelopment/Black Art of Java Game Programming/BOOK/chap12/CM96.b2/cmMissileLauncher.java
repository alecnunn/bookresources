import  java.applet.*;

class cmMissileLauncher extends cmAbstractWeapon {
   cmMissileLauncher(cmAbstractVehicle host,fPoint3d relPos){
      super(host,relPos,loadingtime,defaultammo);
   }

   public boolean fire(){
      if(super.fire()){
         //-- create a new missile
         fPoint3d p=theHost.getPosition();
         p.plus(relOrigin);
         new cmGenericMissile(theHost.getWorld(), theHost,
            p, theHost.getAngle());
         return true;
      }
      return false;
   }

   public String getName(){
      return name;
   }

  	public static void initiateClass (Applet app) {
     	app.showStatus(" cmMissileLauncher : initiating class");
  		loadingtime=new Double(app.getParameter("cmMissileLauncher_loadingtime")).doubleValue();
		defaultammo=Integer.parseInt(app.getParameter("cmMissileLauncher_defaultammo" ));
      name=app.getParameter("cmMissileLauncher_name" );
		
		cmGenericMissile.initiateClass(app);
  	}

  	protected static double loadingtime;
  	protected static int defaultammo;
 	protected static String name;
}
