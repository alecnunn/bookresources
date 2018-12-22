import  java.net.*;
import  java.io.*;
import  java.applet.*;

class cmAntiTankCannon extends cmAbstractPlayer{
	cmAntiTankCannon (fWorld w, double x, double z,double a) {
      super(w,new fPoint3d(x,scale.y,z),new fVelocityVector(a,0,0),
         turningRate,0,0,0,ourHealth);
	   usePolyhedronInstance(new fPolyhedronInstance(ourDefaultPolyhedron,scale));
	   addWeapon(new cmMinicannon(this,new fPoint3d(0,0,0)));
	   selectWeapon(0);
	}

	public boolean interestedOfCollisionWith (fObject obj) {
	   if(obj instanceof cmAbstractRound) return true;
	   if(obj instanceof cmAbstractVehicle) return true;
	   return super.interestedOfCollisionWith(obj);
	}

	protected boolean handleEvent (fEvent event) {
		if(event instanceof cmEventWeaponCommand) {
		   cmEventWeaponCommand com = (cmEventWeaponCommand)event;
		   if (com.command == cmEventWeaponCommand.FIRE) {
		      ((cmAbstractWeapon)myWeapons.elementAt(selectedWeapon)).fire();
		      return true;
		   }
		}
		return super.handleEvent(event);
	}
	
	protected boolean handleCollisionWith (fObject obj,double dt) {
		   if(obj instanceof cmAbstractRound) {
		      cmAbstractRound round = (cmAbstractRound)obj;
		      if (round.getShooter()==this){
		         return true;
		      }
		      die();
		      return false;
		   }
		   return super.handleCollisionWith(obj,dt);
	}

	protected void die () {
		super.die();
		for(int n=0;n<fragmentsWhenDead;n++){
		   new cmGenericFragment(getWorld(),fragmentSize,getPosition(),fragmentSpread,fragmentGenerations,fragmentSpeed,3);
		}
	}

	public static void initiateClass (Applet app) {
    	app.showStatus(" cmAntiTankCannon : initiating class");
		try{
			ourDefaultPolyhedron=new fConvexPolyhedron(new URL(app.getCodeBase(),app.getParameter("cmAntiTankCannon_polyhedronFile")).openStream());
		} catch(Exception e) {e.printStackTrace();}
		
		turningRate=new Double(app.getParameter("cmAntiTankCannon_turningRate")).doubleValue();
		scale=new fPoint3d(new Double(app.getParameter("cmAntiTankCannon_sizeX")).doubleValue(),
		                   new Double(app.getParameter("cmAntiTankCannon_sizeY")).doubleValue(),
                         new Double(app.getParameter("cmAntiTankCannon_sizeZ")).doubleValue());
		ourHealth=new Double(app.getParameter("cmAntiTankCannon_ourHealth")).doubleValue();
		fragmentsWhenDead=Integer.parseInt(app.getParameter("cmAntiTankCannon_fragmentsWhenDead"));
		fragmentSpeed=new Double(app.getParameter("cmAntiTankCannon_fragmentSpeed")).doubleValue();
		fragmentSpread=new Double(app.getParameter("cmAntiTankCannon_fragmentSpread")).doubleValue();
		fragmentSize=new Double(app.getParameter("cmAntiTankCannon_fragmentSize")).doubleValue();
		fragmentGenerations=Integer.parseInt(app.getParameter("cmAntiTankCannon_fragmentGenerations"));
	}

	protected static fPolyhedron ourDefaultPolyhedron;
	protected static double ourHealth;
	protected static double turningRate;
	protected static fPoint3d scale;
	protected static double fragmentSize;
	protected static double fragmentSpeed;
	protected static double fragmentSpread;
	protected static int fragmentsWhenDead;
	protected static int fragmentGenerations;
}

