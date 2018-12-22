import  java.applet.*;

class cmMinicannon extends cmAbstractWeapon {
   cmMinicannon(cmAbstractVehicle host,fPoint3d relPos){
      super(host,relPos,loadingtime,defaultammo);
   }

   public boolean fire(){
      if(super.fire()){
         fPoint3d p=theHost.getPosition();
         p.plus(relOrigin);
         new cmMinicannonRound(theHost.getWorld(),theHost,p,theHost.getAngle());
         return true;
      }
      return false;
   }

   public String getName(){
      return name;
   }

  	public static void initiateClass (Applet app) {
     	app.showStatus(" cmMinicannon : initiating class");
  		loadingtime=new Double(app.getParameter("cmMinicannon_loadingtime")).doubleValue();
		defaultammo=Integer.parseInt(app.getParameter("cmMinicannon_defaultammo" ));
      name=app.getParameter("cmMinicannon_name" );
		
		cmMinicannonRound.initiateClass(app);
  	}

  	protected static double loadingtime;
  	protected static int defaultammo;
 	protected static String name;
}
