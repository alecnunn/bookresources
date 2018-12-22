import  java.net.*;
import  java.io.*;
import  java.applet.*;

class cmMinicannonShell extends cmAbstractShell{
	cmMinicannonShell (fWorld w, fPoint3d origin, fAngle3d agl, fPoint3d vel) {
		super(w,origin,agl, new fPoint3d(vel.x+speed*Math.sin(agl.y+Math.PI/2),
		   vel.y+speed, vel.z+speed*Math.cos(agl.y+Math.PI/2)),
		   new fAngle3d(angleX,angleY,angleZ),
		   randomSpread, randomRotation,lifeTime);

		usePolyhedronInstance(new fPolyhedronInstance(ourDefaultPolyhedron,ourScale));
	}

	public static void initiateClass (Applet app) {
      app.showStatus(" cmMinicannonShell : initiating class");
 		try{
 			ourDefaultPolyhedron=new fConvexPolyhedron(new URL(app.getCodeBase(),app.getParameter("cmMinicannonShell_polyhedronFile")).openStream());
 		} catch(Exception e) {e.printStackTrace();}
 		speed=new Double(app.getParameter("cmMinicannonShell_Speed")).doubleValue();
 		angleX=new Double(app.getParameter("cmMinicannonShell_angleX")).doubleValue();
 		angleY=new Double(app.getParameter("cmMinicannonShell_angleY")).doubleValue();
 		angleZ=new Double(app.getParameter("cmMinicannonShell_angleZ")).doubleValue();
 		randomSpread=new Double(app.getParameter("cmMinicannonShell_Spread")).doubleValue();
 		randomRotation=new Double(app.getParameter("cmMinicannonShell_Rotation")).doubleValue();
		double scalex=new Double(app.getParameter("cmMinicannonShell_scalex")).doubleValue();
 		double scaley=new Double(app.getParameter("cmMinicannonShell_scaley")).doubleValue();
 		double scalez=new Double(app.getParameter("cmMinicannonShell_scalez")).doubleValue();
 		ourScale=new fPoint3d(scalex,scaley,scalez);
 		lifeTime=new Double(app.getParameter("cmMinicannonShell_lifeTime")).doubleValue();
	}

	protected static fPolyhedron ourDefaultPolyhedron;
	protected static fPoint3d ourScale;
	protected static double angleX;
	protected static double angleY;
	protected static double angleZ;
	protected static double speed;
	protected static double randomSpread;
	protected static double randomRotation;
	protected static double lifeTime;
}

