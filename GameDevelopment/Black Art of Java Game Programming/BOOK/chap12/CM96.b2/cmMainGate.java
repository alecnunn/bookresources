import  java.net.*;
import  java.io.*;
import  java.applet.*;

class cmMainGate extends cmAbstractDoor {
   cmMainGate(fWorld w, fPoint3d p, fAngle3d a){
      super(w,new fPoint3d(p.x,p.y+sizeY,p.z),a,speedUp,speedDown,waitTimeUp,maxHeight);
	   usePolyhedronInstance(new fPolyhedronInstance(ourDefaultPolyhedron,new fPoint3d(sizeX,sizeY,sizeZ)));
   }
   
	public static void initiateClass (Applet app) {
     	app.showStatus(" cmMainGate : initiating class");
		try{
			ourDefaultPolyhedron=new fConvexPolyhedron(new URL(app.getCodeBase(),app.getParameter("cmMainGate_polyhedronFile")).openStream());
		} catch(Exception e) {e.printStackTrace();}
		speedUp=new Double(app.getParameter("cmMainGate_SpeedUp")).doubleValue();
		speedDown=new Double(app.getParameter("cmMainGate_SpeedDown")).doubleValue();
		waitTimeUp=new Double(app.getParameter("cmMainGate_WaitTimeUp")).doubleValue();
		maxHeight=new Double(app.getParameter("cmMainGate_maxHeight")).doubleValue();
		sizeX=new Double(app.getParameter("cmMainGate_sizeX")).doubleValue();
		sizeY=new Double(app.getParameter("cmMainGate_sizeY")).doubleValue();
		sizeZ=new Double(app.getParameter("cmMainGate_sizeZ")).doubleValue();
	}

	//-- class constants
	protected static fPolyhedron ourDefaultPolyhedron;
   protected static double speedUp;
   protected static double speedDown;
   protected static double waitTimeUp;
   protected static double maxHeight;
   protected static double sizeX;
   protected static double sizeY;
   protected static double sizeZ;
}

