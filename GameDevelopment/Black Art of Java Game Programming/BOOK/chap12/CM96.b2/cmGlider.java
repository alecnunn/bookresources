import  java.net.*;
import  java.io.*;
import  java.applet.*;

class cmGliderRemains extends cmAbstractMovingScenery {
   cmGliderRemains(fWorld theWorld,cmGlider g){
      super(theWorld,g.getPosition(),g.getAngle(),
         ((fPoint3d)g.getdPosition()),g.getdAngle());

      //-- use the default polyhedron instance
      usePolyhedronInstance(new fPolyhedronInstance(ourDefaultPolyhedron,ourScale));

      //-- set a random rotation on the remaining glider
      setdAngle(new fAngle3d(fWorld.rand(-ourRandRot,ourRandRot),
         fWorld.rand(-ourRandRot,ourRandRot),
         fWorld.rand(-ourRandRot,ourRandRot)));
   }

   public void update(double dt){
      super.update(dt);
      fPoint3d p=getPosition();
      
      //-- check if collision with ground
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
         //-- gravity
         fPoint3d dp=getdPosition();
         dp.y+=((cmWorld)getWorld()).gravity*dt;
         setdPosition(dp);
      }
   }

   public static void initiateClass (Applet app) {
     	app.showStatus(" cmGliderRemains : initiating class");
		try{
			ourDefaultPolyhedron=new fConvexPolyhedron(new URL(app.getCodeBase(),app.getParameter("cmGliderRemains_ourDefaultPolyehdron")).openStream());
		} catch(Exception e) {e.printStackTrace();}
		double scalex=new Double(app.getParameter("cmGliderRemains_scalex")).doubleValue();
		double scaley=new Double(app.getParameter("cmGliderRemains_scaley")).doubleValue();
		double scalez=new Double(app.getParameter("cmGliderRemains_scalez")).doubleValue();
      ourScale=new fPoint3d(scalex,scaley,scalez);

		ourRandRot=new Double(app.getParameter("cmGliderRemains_ourRandRot")).doubleValue();
   }

	protected static fPolyhedron ourDefaultPolyhedron;
	protected static fPoint3d ourScale;

	protected static double ourRandRot;
   
}



class cmGlider extends cmAbstractPlayer{
   cmGlider (fWorld w, double x, double z, double y,
      double turn, double pitch)
   {
      super(w,new fPoint3d(x,ourScale.y+y,z),
         new fVelocityVector(turn,pitch,0),
         turningRate,pitchRate,acceleration,
         brakeRate,maxVelocity,climbRate,decentRate,1,ourHealth);
         
      //-- use the default polyhedron instance
      usePolyhedronInstance(new fPolyhedronInstance(ourDefaultPolyhedron,ourScale));
      
      //-- add the weapons
      addWeapon(new cmMinicannon(this,new fPoint3d(0,ourScale.y,0)));
      addWeapon(new cmMissileLauncher(this,
         new fPoint3d(0,ourScale.y,0)));
      addWeapon(new cmBombBay(this,new fPoint3d(0,ourScale.y,0)));
      selectWeapon(0);
	}

   public void update (double dt) {
      super.update(dt);

      //-- check collision with ground
      fPoint3d p=getPosition();
      if(p.y<ourScale.y){
	      p.y=ourScale.y;
         setPosition(p);
         fVelocityVector dp=(fVelocityVector)getdPosition();
         dp.setAngleAboutXaxis(0);
	      dp.setVelocity(0);
	      setdPosition(dp);
	      //-- some damage depending on the speed
	      double vel=((fVelocityVector)getdPosition()).getVelocity();
	      setHealth(getHealth()-vel);
	   }
	}

   protected void die(){
      super.die();
      for(int n=0;n<fragmentsWhenDead;n++){
         new cmGenericFragment(getWorld(),fragmentSize,getPosition(),
         fragmentSpread, fragmentGenerations, fragmentSpeed,3);
      }
      new cmGliderRemains(getWorld(),this);
   }

   public static void initiateClass (Applet app) {
    	app.showStatus(" cmGlider : initiating class");
		try{
			ourDefaultPolyhedron=new fConvexPolyhedron(new URL(app.getCodeBase(),app.getParameter("cmGlider_polyhedronFile")).openStream());
		} catch(Exception e) {e.printStackTrace();}
		double sizeX=new Double(app.getParameter("cmGlider_sizeX")).doubleValue();
		double sizeY=new Double(app.getParameter("cmGlider_sizeY")).doubleValue();
		double sizeZ=new Double(app.getParameter("cmGlider_sizeZ")).doubleValue();
      ourScale=new fPoint3d(sizeX,sizeY,sizeZ);
      ourHealth=new Double(app.getParameter("cmGlider_ourHealth")).doubleValue();
      
		turningRate=new Double(app.getParameter("cmGlider_turningRate")).doubleValue();
		pitchRate=new Double(app.getParameter("cmGlider_pitchRate")).doubleValue();
		acceleration=new Double(app.getParameter("cmGlider_acceleration")).doubleValue();
		brakeRate=new Double(app.getParameter("cmGlider_brakeRate")).doubleValue();
		maxVelocity=new Double(app.getParameter("cmGlider_maxVelocity")).doubleValue();  		
		climbRate=new Double(app.getParameter("cmGlider_climbRate")).doubleValue();
		decentRate=new Double(app.getParameter("cmGlider_decentRate")).doubleValue();
		
		fragmentsWhenDead=Integer.parseInt(app.getParameter("cmGlider_fragmentsWhenDead"));
		fragmentSpeed=new Double(app.getParameter("cmGlider_fragmentSpeed")).doubleValue();
		fragmentSpread=new Double(app.getParameter("cmGlider_fragmentSpread")).doubleValue();
		fragmentSize=new Double(app.getParameter("cmGlider_fragmentSize")).doubleValue();
		fragmentGenerations=Integer.parseInt(app.getParameter("cmGlider_fragmentGenerations"));

		cmGliderRemains.initiateClass(app);
   }

	protected static fPolyhedron ourDefaultPolyhedron;
   protected static fPoint3d ourScale;
   protected static double ourHealth;

	protected static double turningRate;
	protected static double acceleration;
	protected static double brakeRate;
	protected static double maxVelocity;
	protected static double climbRate;
	protected static double decentRate;
	protected static double pitchRate;
	protected static double fragmentSize;
	protected static double fragmentSpeed;
	protected static double fragmentSpread;
	protected static int fragmentsWhenDead;
	protected static int fragmentGenerations;
}

