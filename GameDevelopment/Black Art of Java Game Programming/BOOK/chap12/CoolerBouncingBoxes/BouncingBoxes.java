import java.net.*;
import java.io.*;
import java.applet.*;
import java.util.*;
import java.awt.*;


class StaticBox extends fObject{
   StaticBox(fWorld w, fPoint3d p ) {
      //-- construct the base class
      super(w,p,new fAngle3d(0,0,0));
      //--
      //-- every non-abstract object MUST have a polyhedron instance.
      //-- this line of code MUST be somewhere in the constructor
      //--
		usePolyhedronInstance(new fPolyhedronInstance(ourDefaultPolyhedron,ourScale));
	}
	
   public void update (double dt) {
      super.update(dt);
      if(getAge()>ourLifeTime){
         die();
      }
   }

   public static void initiateClass (Applet app) {
      //--
      //-- get the static "constants" that all bouncing boxes have
      //--
      try{
         //-- load the default polyhedron for all bouncing boxes
         //-- this MUST be done in all non-abstract classes
         String polyfile=app.getParameter("StaticBox_ourDefaultPolyhedron");
         URL url=new URL(app.getCodeBase(),polyfile);
         InputStream is=url.openStream();
         ourDefaultPolyhedron=new fConvexPolyhedron(is);
      } catch(Exception e) {
         e.printStackTrace();
      }
      //-- the scaling of the polyhedron
      double xscale=new Double(app.getParameter("StaticBox_scalex")).doubleValue();
      double yscale=new Double(app.getParameter("StaticBox_scaley")).doubleValue();
      double zscale=new Double(app.getParameter("StaticBox_scalez")).doubleValue();
      ourScale=new fPoint3d(xscale,yscale,zscale);
      ourLifeTime=new Double(app.getParameter("StaticBox_ourLifeTime")).doubleValue();
   }
   
   //-- class constants
   protected static fPolyhedron ourDefaultPolyhedron;
   protected static fPoint3d ourScale;
   protected static double ourLifeTime;
}
   
	

class BouncingBox extends fMovingObject{
   BouncingBox(fWorld w, fPoint3d p ) {
      //-- construct the base class
      super(w,p,new fAngle3d(0,0,0),new fPoint3d(0,0,0),new fAngle3d(0,Math.PI,0));
      //--
      //-- every non-abstract object MUST have a polyhedron instance.
      //-- this line of code MUST be somewhere in the constructor
      //--
		usePolyhedronInstance(new fPolyhedronInstance(ourDefaultPolyhedron,ourScale));
	}

   public void update (double dt) {
      //-- always let the base class do the default action for
      //-- an overridden method. Never forget this!
      super.update(dt);

      //-- retrieve the position and velocity of the box
      fPoint3d dp=getdPosition();
      fPoint3d p=getPosition();
		
      //-- check if the box has hit the ground
      if(p.y<ourScale.y){
         //-- the bottom of the box has hit the ground
         //-- switch the sign of the velocity and
         //-- decrease it a bit
         dp.y=-dp.y*ourBounciness;
         //-- make sure the box is not below ground level
         p.y=ourScale.y;
         //-- set the new position
         setPosition(p);
      }
      //-- make sure gravity does it's share
      dp.y-=BouncingBoxWorld.gravity*dt;
      
      //-- set the new velocity
      setdPosition(dp);
	}

	public boolean interestedOfCollisionWith (fObject obj){
	   //-- I'm interested in all collisions
	   if(obj instanceof StaticBox) return true;
	   
	   //-- none of the objects I know, let the base class decide
	   return super.interestedOfCollisionWith (obj);
	}

	protected boolean handleCollisionWith (fObject obj,double dt){
	   //-- if it is one of my interest objects
	   if(obj instanceof StaticBox){
         //-- retrieve the position and velocity of the box
         fPoint3d dp=getdPosition();
         
         dp.y=-dp.y;
         //-- make sure gravity does it's share
         dp.y-=BouncingBoxWorld.gravity*dt;
      
         //-- set the new velocity
         setdPosition(dp);

         //-- event has been handled, get more events
         return true;
	   }
	   //-- if I don't know what this object is then it
	   //-- means that a super class is intersted about
	   //-- a collision with it. let it take care of it
	   return super.handleCollisionWith(obj,dt);
	}

   public static void initiateClass (Applet app) {
      //--
      //-- get the static "constants" that all bouncing boxes have
      //--
      try{
         //-- load the default polyhedron for all bouncing boxes
         //-- this MUST be done in all non-abstract classes
         String polyfile=app.getParameter("BouncingBox_ourDefaultPolyhedron");
         URL url=new URL(app.getCodeBase(),polyfile);
         InputStream is=url.openStream();
         ourDefaultPolyhedron=new fConvexPolyhedron(is);
      } catch(Exception e) {
         e.printStackTrace();
      }
      //-- the scaling of the polyhedron
      double xscale=new Double(app.getParameter("BouncingBox_scalex")).doubleValue();
      double yscale=new Double(app.getParameter("BouncingBox_scaley")).doubleValue();
      double zscale=new Double(app.getParameter("BouncingBox_scalez")).doubleValue();
      ourScale=new fPoint3d(xscale,yscale,zscale);
      //-- the bounciness of this box                         
		ourBounciness=new Double(app.getParameter("BouncingBox_ourBounciness")).doubleValue();
	}
   //-- class constants
   protected static fPolyhedron ourDefaultPolyhedron;
   protected static double ourBounciness;
   protected static fPoint3d ourScale;
}

class BouncingBoxWorld extends fWorld {
   BouncingBoxWorld(Applet app){
      //-- make a world which is 200x200 meters with -100,-100
      //-- as minimum divided into 5x5 grids
      super(app,-100,-100,200,5);

      //-- insert a couple of boxes at random height
      for(double x=-10;x<=10;x+=10){
         for(double z=-10;z<=10;z+=10){
            new BouncingBox(this,new fPoint3d(x,15+Math.random()*30,z));
         }
      }
      //-- insert a static box
      new StaticBox(this,new fPoint3d(0,10,0));
   }

   //-- bouncing box class must be inititated.
   //-- overrides the base class method.
  	protected void initiateClasses (Applet app) {
		BouncingBox.initiateClass(app);
      StaticBox.initiateClass(app);
		gravity=new Double(app.getParameter("BouncingBoxWorld_gravity")).doubleValue();
		
	}

	public static double gravity;
}

public class BouncingBoxes extends NoFlickerApplet implements Runnable {
   //-- the world and the camera   
   BouncingBoxWorld world;
   fCamera camera;

   //-- standard-fare applet stuff
	Thread myThread;
	boolean alive;

   public void init () {
      //-- construct the world
      world=new BouncingBoxWorld(this);
      //-- construct the camera
      double viewangle=Math.PI/3;
      double viewdist=100;
      fPoint3d campos=new fPoint3d(30,15,30);
      fAngle3d camagl=new fAngle3d(0,Math.PI/4,0);
      double gridsize=10;
      double fading=0;
      camera=new fMagicCamera(world,viewangle,viewdist,campos,camagl,gridsize,fading);
      
	   //-- applet stuff
      myThread=new Thread(this);
      myThread.start();
      alive=true;
   }

   protected long lastUpdate;
   
   public void run () {
      lastUpdate=System.currentTimeMillis();
      while(alive){
         long currentTime=System.currentTimeMillis();
         long dtmillis=currentTime-lastUpdate;
   	   double dt=(double)dtmillis/1000;
     	   lastUpdate=currentTime;

     	   //-- make sure the update doesn't take to large "steps"
   	   if(dt>0.2) dt=0.2;
         world.update(dt);

         //-- to speed up the repaint we tell the Applet to update
         //-- directly instead of calling repaint
         if(getPeer()==null) return;
         Graphics g=getGraphics();
         if(g==null) return;
         Dimension dim=size();
         if(dim.width==0 || dim.height==0) return;
         update(g);
      }
   }

   public void start () {
      if(myThread==null){
         myThread=new Thread(this);
         myThread.start();
         alive=true;
      }
   }

   public void paint(Graphics g){
      //-- update the screen size for the camera
      camera.setScreenSize(size().width,size().height);
      //-- erase the screen
      g.clearRect(0,0,size().width,size().height);
      //-- paint the world
      camera.paint(g);
   }

   public void stop () {
      if(myThread!=null){
         myThread.stop();
         myThread=null;
         alive=false;
      }
   }
}
