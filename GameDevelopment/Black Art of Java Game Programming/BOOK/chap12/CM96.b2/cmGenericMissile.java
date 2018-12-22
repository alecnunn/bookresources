import java.net.*;
import java.io.*;
import java.applet.*;
import java.util.*;

class cmGenericMissile extends cmAbstractRound{
   cmGenericMissile (fWorld w, fObject shooter,
      fPoint3d pos, fAngle3d agl)
   {
      super(w,shooter,pos,new fVelocityVector(agl.y,agl.x,0),0,0,
         acceleration,0,maxVelocity,0,0,1,impactDamage);
         
      usePolyhedronInstance(new fPolyhedronInstance(ourDefaultPolyhedron,ourScale));
	}

	public void update (double dt) {
		super.update(dt);
		increaseVelocity(1,dt);
		
		fPoint3d p=getPosition();
		if((getAge()>4) || (p.y<0)){
		   p.y=0;
		   setPosition(p);
		   die();
		}
	}

   protected void die () {
      super.die();
      for(int n=0;n<fragmentsWhenDead;n++){
      new cmGenericFragment(getWorld(),fragmentSize,getPosition(),
         fragmentSpread,fragmentGenerations,fragmentSpeed,3);
		}
	}

	public static void initiateClass (Applet app) {
     	app.showStatus(" cmGenericMissile : initiating class");
		try{
			ourDefaultPolyhedron=new fConvexPolyhedron(new URL(app.getCodeBase(),app.getParameter("cmGenericMissile_polyhedronFile")).openStream());
		} catch(Exception e) {e.printStackTrace();}
		double scalex =new Double(app.getParameter("cmGenericMissile_scalex")).doubleValue();
		double scaley =new Double(app.getParameter("cmGenericMissile_scaley")).doubleValue();
		double scalez =new Double(app.getParameter("cmGenericMissile_scalez")).doubleValue();
		ourScale=new fPoint3d(scalex,scaley,scalez);
		
		acceleration =new Double(app.getParameter("cmGenericMissile_acceleration")).doubleValue();
		maxVelocity  =new Double(app.getParameter("cmGenericMissile_maxVelocity")).doubleValue();
		impactDamage =new Double(app.getParameter("cmGenericMissile_impactDamage")).doubleValue();

		fragmentsWhenDead  =Integer.parseInt(app.getParameter("cmGenericMissile_fragmentsWhenDead"));
		fragmentSpeed      =new Double(app.getParameter("cmGenericMissile_fragmentSpeed")).doubleValue();
		fragmentSpread     =new Double(app.getParameter("cmGenericMissile_fragmentSpread")).doubleValue();
		fragmentSize       =new Double(app.getParameter("cmGenericMissile_fragmentSize")).doubleValue();
		fragmentGenerations=Integer.parseInt(app.getParameter("cmGenericMissile_fragmentGenerations"));
	}

	protected static fPolyhedron ourDefaultPolyhedron;
	protected static fPoint3d ourScale;
	protected static double turningRate;
	protected static double pitchRate;
	protected static double acceleration;
	protected static double brakeRate;
	protected static double maxVelocity;
	protected static double impactDamage;
	protected static double fragmentSize;
	protected static double fragmentSpeed;
	protected static double fragmentSpread;
	protected static int fragmentsWhenDead;
	protected static int fragmentGenerations;
}

