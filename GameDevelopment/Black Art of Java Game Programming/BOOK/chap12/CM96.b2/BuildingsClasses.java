import  java.net.*;
import  java.io.*;
import  java.applet.*;

class cmGenericBuildingRuin extends cmAbstractStaticStructure {
   cmGenericBuildingRuin (fWorld world, double x, double z,
      fAngle3d agl, double w, double b, double h)
   {
      super(world,x,z,agl,w,b,h);
      //-- make a building
      usePolyhedronInstance(new fPolyhedronInstance(ourDefaultPolyhedron,new fPoint3d(w,h,b)));
   }
   
   public static void initiateClass (Applet app) {
      app.showStatus(" cmGenericBuildingRuin : inititating class.");
      try{
         ourDefaultPolyhedron=new fConvexPolyhedron(new URL(app.getCodeBase(),app.getParameter("cmGenericBuildingRuin_polyhedronFile")).openStream());
      } catch(Exception e) {e.printStackTrace();}
   }

   protected static fPolyhedron ourDefaultPolyhedron;
}

class cmGenericBuilding extends cmAbstractStaticStructure{
   /**
    * Generic building with an angle.
    */
   cmGenericBuilding (fWorld world, double x, double z, fAngle3d agl,
      double w, double b, double h)
	{
      super(world,x,z,agl,w,b,h,ourHealth);
		//-- make a building
		myWidth=w; myBredth=b; myHeight=h;
		usePolyhedronInstance(new fPolyhedronInstance(ourDefaultPolyhedron,new fPoint3d(w,h,b)));
	}
   /**
    * Generic building with the default 0,0,0 angle.
    */
	cmGenericBuilding (fWorld world, double x, double z, double w,
	   double b, double h)
	{
      super(world,x,z,new fAngle3d(),w,b,h,ourHealth);
		//-- make a building
		myWidth=w; myBredth=b; myHeight=h;
		usePolyhedronInstance(new fPolyhedronInstance(ourDefaultPolyhedron,new fPoint3d(w,h,b)));
	}

   protected void die () {
      super.die();
      fPoint3d pos=getPosition();
      new cmGenericBuildingRuin(getWorld(),pos.x,pos.z,getAngle(),
         myWidth,myBredth,myHeight*.2);
      int nbr=(int)(relFragsWhenDead*myHeight);
      for(int n=0;n<nbr;n++){
         new cmGenericFragment(getWorld(),relFragSize*myHeight,
            getPosition(),relFragSpread*myHeight,
            (int)(myHeight*relFragGens),myHeight*relFragSpeed,
            myHeight*relFragRot);
      }
   }

	public static void initiateClass (Applet app) {
      app.showStatus(" cmGenericBuilding : initiating class");
      try{
         ourDefaultPolyhedron=new fConvexPolyhedron(new URL(app.getCodeBase(),app.getParameter("cmGenericBuilding_polyhedronFile")).openStream());
      } catch(Exception e) {e.printStackTrace();}
		relFragsWhenDead=new Double(app.getParameter("cmGenericBuilding_relFragsWhenDead")).doubleValue();
      relFragSpeed    =new Double(app.getParameter("cmGenericBuilding_relFragSpeed"    )).doubleValue();
      relFragSpread   =new Double(app.getParameter("cmGenericBuilding_relFragSpread"   )).doubleValue();
      relFragSize     =new Double(app.getParameter("cmGenericBuilding_relFragSize"     )).doubleValue();
      relFragGens     =new Double(app.getParameter("cmGenericBuilding_relFragGens"     )).doubleValue();
      relFragRot      =new Double(app.getParameter("cmGenericBuilding_relFragRot"      )).doubleValue();
      ourHealth       =new Double(app.getParameter("cmGenericBuilding_ourHealth"       )).doubleValue();
      cmGenericBuildingRuin.initiateClass(app);
	}
	protected static fPolyhedron ourDefaultPolyhedron;
	protected static double relFragSize;
	protected static double relFragSpeed;
	protected static double relFragSpread;
	protected static double relFragsWhenDead;
	protected static double relFragGens;
	protected static double relFragRot;
	protected static double ourHealth;

	protected double myWidth,myBredth,myHeight;
}
class cmGenericTowerRuin extends cmAbstractStaticStructure {
   cmGenericTowerRuin (fWorld world, double x, double z, fAngle3d agl, double w, double b, double h) {
      super(world,x,z,agl,w,b,h);
      //-- make a building
      usePolyhedronInstance(new fPolyhedronInstance(ourDefaultPolyhedron,new fPoint3d(w,h,b)));
   }
   
   public static void initiateClass (Applet app) {
      app.showStatus(" cmGenericTowerRuin : inititating class.");
      try{
         ourDefaultPolyhedron=new fConvexPolyhedron(new URL(app.getCodeBase(),app.getParameter("cmGenericTowerRuin_polyhedronFile")).openStream());
      } catch(Exception e) {e.printStackTrace();}
   }

   protected static fPolyhedron ourDefaultPolyhedron;
}

class cmGenericTower extends cmAbstractStaticStructure{
	cmGenericTower (fWorld world, double x, double z, fAngle3d agl,double w, double b, double h) {
      super(world,x,z,new fAngle3d(),w,b,h,ourHealth);
		//-- make a building
		myWidth=w; myBredth=b; myHeight=h;
		usePolyhedronInstance(new fPolyhedronInstance(ourDefaultPolyhedron,new fPoint3d(w,h,b)));
	}

	protected void die () {
      super.die();
      fPoint3d pos=getPosition();
      new cmGenericTowerRuin(getWorld(),pos.x,pos.z,getAngle(),myWidth,myBredth,myHeight*.2);
      int nbr=(int)(relFragsWhenDead*myHeight);
      for(int n=0;n<nbr;n++){
         new cmGenericFragment(getWorld(),relFragSize*myHeight,getPosition(),relFragSpread*myHeight,
      	                      (int)(myHeight*relFragGens),myHeight*relFragSpeed,myHeight*relFragRot);
      }
   }

	public static void initiateClass (Applet app) {
      app.showStatus(" cmGenericTower : initiating class");
      try{
         ourDefaultPolyhedron=new fConvexPolyhedron(new URL(app.getCodeBase(),app.getParameter("cmGenericTower_polyhedronFile")).openStream());
      } catch(Exception e) {e.printStackTrace();}
		relFragsWhenDead=new Double(app.getParameter("cmGenericTower_relFragsWhenDead")).doubleValue();
      relFragSpeed    =new Double(app.getParameter("cmGenericTower_relFragSpeed"    )).doubleValue();
      relFragSpread   =new Double(app.getParameter("cmGenericTower_relFragSpread"   )).doubleValue();
      relFragSize     =new Double(app.getParameter("cmGenericTower_relFragSize"     )).doubleValue();
      relFragGens     =new Double(app.getParameter("cmGenericTower_relFragGens"     )).doubleValue();
      relFragRot      =new Double(app.getParameter("cmGenericTower_relFragRot"      )).doubleValue();
      ourHealth       =new Double(app.getParameter("cmGenericTower_ourHealth"       )).doubleValue();
      cmGenericTowerRuin.initiateClass(app);
	}
	protected static fPolyhedron ourDefaultPolyhedron;
	protected static double relFragSize;
	protected static double relFragSpeed;
	protected static double relFragSpread;
	protected static double relFragsWhenDead;
	protected static double relFragGens;
	protected static double relFragRot;
	protected static double ourHealth;

	protected double myWidth,myBredth,myHeight;
}

