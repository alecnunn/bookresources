import  java.net.*;
import  java.io.*;
import  java.applet.*;
import java.util.*;

class cmGenericBomb extends cmAbstractMovingScenery{
	cmGenericBomb (fWorld w, fPoint3d p, fAngle3d a,
	   fPoint3d dp, double strength0 )
	{
      super(w,p, a, new fPoint3d(0,0,0),
         new fAngle3d(
            fWorld.rand(-randRotation,randRotation),
   		   fWorld.rand(-randRotation,randRotation),
   		   fWorld.rand(-randRotation,randRotation))
      );
      //--
      usePolyhedronInstance(new fPolyhedronInstance(ourDefaultPolyhedron,ourScale));
      //--
      strength=strength0;
   }

   public void update (double dt) {
      super.update(dt);
		   
      fPoint3d v=getdPosition();
      v.y+=cmWorld.gravity;
      setdPosition(v);
		
      fPoint3d p=getPosition();
      if(p.y<0){
         die();
      }
	}

   protected void die () {
      super.die();
      //-- create an explosion that is proportional to the strength
      //-- of the bomb
      new cmGenericExplosion(getWorld(),getPosition(),
         strength*0.25,0.5,strength,0.6,strength*0.1);

      //-- create an explosion round since
      cmAbstractRound wep=new cmExplosion(getWorld(),10*strength);
  		
      //-- get all objects within a radius and check feed them with
      //-- the impact of the bomb
      vect.removeAllElements();
      getWorld().getAllObjectsInRadius(getPosition(),30,vect);
      int nbr=vect.size();
      for(int n=0;n<nbr;n++){
         fObject obj=(fObject)vect.elementAt(n);
         if(obj.interestedOfCollisionWith(wep)){
            obj.collisionWith(wep,1);
         }
      }
      wep.die();
   }
	protected double strength;

	public static void initiateClass (Applet app) {
     	app.showStatus(" GenericBomb : initiating class");
		try{
			ourDefaultPolyhedron=new fConvexPolyhedron(new URL(app.getCodeBase(),app.getParameter("cmGenericBomb_polyhedronFile")).openStream());
		} catch(Exception e) {e.printStackTrace();}
		randStartAngle=new Double(app.getParameter("cmGenericBomb_randStartAngle")).doubleValue();
		randRotation=new Double(app.getParameter("cmGenericBomb_randRotation"  )).doubleValue();
		double scalex=new Double(app.getParameter("cmGenericBomb_scaleX"        )).doubleValue();
		double scaley=new Double(app.getParameter("cmGenericBomb_scaleY"        )).doubleValue();
		double scalez=new Double(app.getParameter("cmGenericBomb_scaleZ"        )).doubleValue();
		ourScale=new fPoint3d(scalex,scaley,scalez);
		vect=new Vector(50,50);
	}

	//-- class constants
	protected static fPolyhedron ourDefaultPolyhedron;
	protected static fPoint3d ourScale;
	protected static double randStartAngle,randRotation;
	protected static Vector vect;
}

class cmExplosion extends cmAbstractRound {
   cmExplosion(fWorld world, double strength){
      super(world,null,new fPoint3d(),new fVelocityVector(0,0,0),
         0,0,0,0,0,0,0,1,strength);
   }
}

