import  java.applet.*;

class cmBombBay extends cmAbstractWeapon {
   cmBombBay(cmAbstractVehicle host,fPoint3d relPos){
      super(host,relPos,loadingtime,defaultammo);
   }

   public boolean fire(){
      if(super.fire()){
         fPoint3d p=theHost.getPosition();
         p.plus(relOrigin);
         new cmGenericBomb(theHost.getWorld(), theHost.getPosition(),
            theHost.getAngle(), theHost.getdPosition(), 15);
         return true;
      }
      return false;
   }

   public String getName(){
      return name;
   }

  	public static void initiateClass (Applet app) {
     	app.showStatus(" cmBombBay : initiating class");
  		loadingtime=new Double(app.getParameter("cmBombBay_loadingtime")).doubleValue();
		defaultammo=Integer.parseInt(app.getParameter("cmBombBay_defaultammo" ));
      name=app.getParameter("cmBombBay_name" );
		
		cmGenericBomb.initiateClass(app);
  	}


  	protected static double loadingtime;
  	protected static int defaultammo;
 	protected static String name;
}
