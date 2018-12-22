import  java.net.*;
import  java.io.*;
import  java.applet.*;

class cmMinicannonRound extends cmAbstractRound{
	cmMinicannonRound (fWorld w, fObject shooter,
	   fPoint3d pos, fAngle3d agl)
	{
      super(w,shooter,pos,new fVelocityVector(agl.y,agl.x,maxVelocity),
		   0,0,0,0,maxVelocity,0,0,1,impactDamage);

		//-- the polyhedron instance
      usePolyhedronInstance(new fPolyhedronInstance(ourDefaultPolyhedron,ourScale));
      //-- create an empty shell
      new cmMinicannonShell(getWorld(),pos,agl,
         ((cmAbstractMovingObject)shooter).getdPosition());
	}

	public void update (double dt) {
      super.update(dt);
      fPoint3d p=getPosition();
      //-- check if it is time to die
      if(dieNextUpdate==true){
         die();
         return;
      }
		//-- if bullet hits the ground then die
		//-- next round so that a collision detection
		//-- can be done
      if(p.y<0){
         p.y=0;
         setPosition(p);
         dieNextUpdate=true;
      }
		//-- if life is out
      if(getAge()>lifeTime){
         deadOfAge=true;
         die();
      }
   }

   protected void die () {
      super.die();
      //-- if this round has died by hitting something
      if(deadOfAge==false){
         for(int n=0;n<fragmentsWhenDead;n++){
            new cmGenericFragment(getWorld(), fragmentSize, getPosition(),
               fragmentSpread, fragmentGenerations, fragmentSpeed,
               fragmentRotation);
         }
      }
   }
   protected boolean dieNextUpdate;
   protected boolean deadOfAge;

	public static void initiateClass (Applet app) {
    	app.showStatus(" cmMinicannonRound : initiating class");
		try{
			ourDefaultPolyhedron=new fConvexPolyhedron(new URL(app.getCodeBase(),app.getParameter("cmMinicannonRound_polyhedronFile")).openStream());
		} catch(Exception e) {e.printStackTrace();}
		
		maxVelocity=new Double(app.getParameter("cmMinicannonRound_maxVelocity")).doubleValue();
		double scalex=new Double(app.getParameter("cmMinicannonRound_scalex")).doubleValue();
		double scaley=new Double(app.getParameter("cmMinicannonRound_scaley")).doubleValue();
		double scalez=new Double(app.getParameter("cmMinicannonRound_scalez")).doubleValue();
		ourScale=new fPoint3d(scalex,scaley,scalez);
		fragmentsWhenDead=Integer.parseInt(app.getParameter("cmMinicannonRound_fragmentsWhenDead"));
		fragmentSpeed=new Double(app.getParameter("cmMinicannonRound_fragmentSpeed")).doubleValue();
		fragmentSpread=new Double(app.getParameter("cmMinicannonRound_fragmentSpread")).doubleValue();
		fragmentSize=new Double(app.getParameter("cmMinicannonRound_fragmentSize")).doubleValue();
		fragmentGenerations=Integer.parseInt(app.getParameter("cmMinicannonRound_fragmentGenerations"));
		fragmentRotation=new Double(app.getParameter("cmMinicannonRound_fragmentRotation")).doubleValue();
		lifeTime=new Double(app.getParameter("cmMinicannonRound_lifeTime")).doubleValue();
		impactDamage=new Double(app.getParameter("cmMinicannonRound_impactDamage")).doubleValue();

      cmMinicannonShell.initiateClass(app);
	}

	protected static fPolyhedron ourDefaultPolyhedron;
	protected static fPoint3d ourScale;
	protected static double maxVelocity;
	protected static double fragmentSize;
	protected static double fragmentSpeed;
	protected static double fragmentSpread;
	protected static int fragmentsWhenDead;
	protected static int fragmentGenerations;
	protected static double fragmentRotation;
	protected static double lifeTime;
   protected static double impactDamage;
}

