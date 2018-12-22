import  java.net.*;
import  java.io.*;
import  java.applet.*;

class cmGenericWall extends cmAbstractStaticStructure{
	protected double mySize;
	cmGenericWall (fWorld world, double x, double z, fAngle3d agl,
	   double w, double b, double h)
	{
		      super(world,x,z,agl,w,b,h);
		      //-- make a building
			   usePolyhedronInstance(new fPolyhedronInstance(ourDefaultPolyhedron,new fPoint3d(w,h,b)));
	}

	public static void initiateClass (Applet app) {
     	app.showStatus(" cmGenericWall : initiating class");
		try{
			ourDefaultPolyhedron=new fConvexPolyhedron(new URL(app.getCodeBase(),app.getParameter("cmGenericWall_polyhedronFile")).openStream());
		} catch(Exception e) {e.printStackTrace();}
	}

	protected static fPolyhedron ourDefaultPolyhedron;
}

