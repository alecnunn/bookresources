import  java.net.*;
import  java.io.*;
import  java.applet.*;

class cmGenericFragment extends cmAbstractMovingScenery{
	cmGenericFragment (fWorld w, double size0, fPoint3d origin, double spread, int generation0, double speed0, double rotation) {
		super(w,new fPoint3d(origin.x+spread*(Math.random()-0.5),
		                     origin.y+spread*(Math.random()-0.5),
		                     origin.z+spread*(Math.random()-0.5)),
		        new fAngle3d(Math.random()*3,Math.random()*3,Math.random()*3),
		        new fPoint3d(),
		        new fAngle3d(Math.random()*3,Math.random()*3,Math.random()*3));
		//--
		generation=generation0-1;
		speed=speed0;
		size=size0;
		//--
		usePolyhedronInstance(new fPolyhedronInstance(ourDefaultPolyhedron,new fPoint3d(fWorld.rand(0,size),fWorld.rand(0,size),fWorld.rand(0,size))));
		//--
		fPoint3d v=getdPosition();
		v.x=cmWorld.rand(-speed,speed);
		v.y=cmWorld.rand(0,speed*velYfactor);
		v.z=cmWorld.rand(-speed,speed);
		setdPosition(v);
	}

	public void update (double dt) {
		   super.update(dt);
		   
		   fPoint3d v=getdPosition();
		   v.y-=10*dt;
		   setdPosition(v);
		
		   fPoint3d p=getPosition();
		   if(p.y<0){
		      p.y=size;
		      setPosition(p);
		      die();
		   }
	}

	protected void die () {
			   super.die();
		      if(generation>0){
		  	      for(int n=0;n<5;n++){
		      	   new cmGenericFragment(getWorld(),size/2,getPosition(),size,generation-1,getdPosition().y*0.5,3);	            
		  	      }
		      }
	}

	public static void initiateClass (Applet app) {
     	app.showStatus(" cmGenericFragment : initiating class");
		try{
			ourDefaultPolyhedron=new fConvexPolyhedron(new URL(app.getCodeBase(),app.getParameter("cmGenericFragment_polyhedronFile")).openStream());
		} catch(Exception e) {e.printStackTrace();}
		velYfactor=new Double(app.getParameter("cmGenericFragment_velYfactor")).doubleValue();
	}

	protected double speed;
	protected double size;
	protected int generation;
	//-- class constants
	protected static fPolyhedron ourDefaultPolyhedron;
	protected static double velYfactor;
}

