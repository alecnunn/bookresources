import  java.net.*;
import  java.io.*;
import  java.applet.*;

class cmFesseTankRemains extends cmAbstractMovingScenery {
   cmFesseTankRemains(fWorld w, cmFesseTank deadTank){
      super(w,deadTank.getPosition(),deadTank.getAngle(),deadTank.getdPosition(),deadTank.getdAngle());
      usePolyhedronInstance(new fPolyhedronInstance(ourDefaultPolyhedron,ourScale));
      
      //-- throw the remaining tank up in the air
      fPoint3d dp=getdPosition();
      dp.y=fWorld.rand(0,ourSpeedUp);
      setdPosition(dp);
      //-- set a random rotation on the remaining tank
      setdAngle(new fAngle3d(fWorld.rand(-ourRandRot,ourRandRot),
         fWorld.rand(-ourRandRot,ourRandRot),
         fWorld.rand(-ourRandRot,ourRandRot)));
   }
   

   public void update(double dt){
      super.update(dt);
      fPoint3d p=getPosition();
      //-- check if hit the ground
      if(p.y<ourScale.y){
         fPoint3d dp=getdPosition();
         p.y=ourScale.y;
         dp.x=dp.y=dp.z=0;
         setPosition(p);
         fAngle3d a=getAngle();
         a.x=a.z=0;
         setAngle(a);
         setdAngle(new fAngle3d(0,0,0));
         setdPosition(dp);
      } else if(p.y>ourScale.y) {
         fPoint3d dp=getdPosition();
         dp.y+=((cmWorld)getWorld()).gravity*dt;
         setdPosition(dp);
      }
   }

   public static void initiateClass (Applet app) {
     	app.showStatus(" cmFesseTankRemains : initiating class");
		try{
			ourDefaultPolyhedron=new fConvexPolyhedron(new URL(app.getCodeBase(),app.getParameter("cmFesseTankRemains_ourDefaultPolyehdron")).openStream());
		} catch(Exception e) {e.printStackTrace();}
		double scalex=new Double(app.getParameter("cmFesseTankRemains_scalex")).doubleValue();
		double scaley=new Double(app.getParameter("cmFesseTankRemains_scaley")).doubleValue();
		double scalez=new Double(app.getParameter("cmFesseTankRemains_scalez")).doubleValue();
		ourScale=new fPoint3d(scalex,scaley,scalez);

		ourSpeedUp=new Double(app.getParameter("cmFesseTankRemains_ourSpeedUp")).doubleValue();
		ourRandRot=new Double(app.getParameter("cmFesseTankRemains_ourRandRot")).doubleValue();
   }

	protected static fPolyhedron ourDefaultPolyhedron;
	protected static fPoint3d ourScale;

	protected static double ourSpeedUp;
	protected static double ourRandRot;
   
}

class cmFesseTank extends cmAbstractPlayer{
	cmFesseTank (fWorld w, double x, double z, double a) {
	   
      super(w,new fPoint3d(x,ourScale.y,z),
         new fVelocityVector(a,0,0),turningRate,acceleration,
         brakeRate,maxVelocity,ourHealth);

      usePolyhedronInstance(new fPolyhedronInstance(ourDefaultPolyhedron,ourScale));
      addWeapon(new cmMinicannon(this,new fPoint3d(0,ourScale.y,0)));
      addWeapon(new cmMissileLauncher(this,new fPoint3d(0,ourScale.y,0)));
      selectWeapon(0);
	}

   protected void die () {
      super.die();
      for(int n=0;n<fragmentsWhenDead;n++){
      new cmGenericFragment(getWorld(),fragmentSize,getPosition(),
         fragmentSpread,fragmentGenerations,fragmentSpeed,3);
		}
		new cmFesseTankRemains(getWorld(),this);
	}

   public static void initiateClass (Applet app) {
     	app.showStatus(" cmFesseTank : initiating class");
		try{
			ourDefaultPolyhedron=new fConvexPolyhedron(new URL(app.getCodeBase(),app.getParameter("cmFesseTank_polyhedronFile")).openStream());
		} catch(Exception e) {e.printStackTrace();}
		double scaleX=new Double(app.getParameter("cmFesseTank_sizeX")).doubleValue();
		double scaleY=new Double(app.getParameter("cmFesseTank_sizeY")).doubleValue();
		double scaleZ=new Double(app.getParameter("cmFesseTank_sizeZ")).doubleValue();
		ourScale=new fPoint3d(scaleX,scaleY,scaleZ);
		
		turningRate=new Double(app.getParameter("cmFesseTank_turningRate")).doubleValue();
		acceleration=new Double(app.getParameter("cmFesseTank_acceleration")).doubleValue();
		brakeRate=new Double(app.getParameter("cmFesseTank_brakeRate")).doubleValue();
		maxVelocity=new Double(app.getParameter("cmFesseTank_maxVelocity")).doubleValue();
		fragmentsWhenDead=Integer.parseInt(app.getParameter("cmFesseTank_fragmentsWhenDead"));
		fragmentSpeed=new Double(app.getParameter("cmFesseTank_fragmentSpeed")).doubleValue();
		fragmentSpread=new Double(app.getParameter("cmFesseTank_fragmentSpread")).doubleValue();
		fragmentSize=new Double(app.getParameter("cmFesseTank_fragmentSize")).doubleValue();
		fragmentGenerations=Integer.parseInt(app.getParameter("cmFesseTank_fragmentGenerations"));
		ourHealth=new Double(app.getParameter("cmFesseTank_ourHealth")).doubleValue();
		
		cmFesseTankRemains.initiateClass(app);
	}

	protected static fPolyhedron ourDefaultPolyhedron;
	protected static fPoint3d ourScale;
	protected static double ourHealth;
	protected static double turningRate;
	protected static double acceleration;
	protected static double brakeRate;
	protected static double maxVelocity;
	protected static double fragmentSize;
	protected static double fragmentSpeed;
	protected static double fragmentSpread;
	protected static int fragmentsWhenDead;
	protected static int fragmentGenerations;
}

