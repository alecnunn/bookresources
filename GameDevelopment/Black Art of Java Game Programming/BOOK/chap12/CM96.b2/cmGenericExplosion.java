import  java.net.*;
import  java.io.*;
import  java.applet.*;
// MODULE: cmGenericExplosion header
/** */


class cmGenericExplosion extends cmAbstractMovingScenery{
   public cmGenericExplosion (fWorld w, fPoint3d p, double s0,double t0,
      double s1,double t1,double s2)
   {
      super(w,new fPoint3d(p.x,s0,p.z),new fAngle3d(),
         new fPoint3d(),new fAngle3d(0,3,0));

      //-- set the polyhedron instance
      usePolyhedronInstance(new fPolyhedronInstance(
         ourDefaultPolyhedron,new fPoint3d(s0*2,s0*0.33,s0*2)));

      //-- create some fragments
      for(int n=0;n<(int)(nbrOfFragments*s1);n++){
         new cmGenericFragment(getWorld(),s1*fragmentsSize,p,
            s0*fragmentsSpeed,1,s1*fragmentsSpeed,s0*fragmentsRotation);
      }

      //-- calculate the delta scaling
      strength=s1;
		
      time1=t0;
      dScale1=(s1-s0)/t0;
		
      time2=t1;
      dScale2=(s2-s1)/t1;
   }

   public void update (double dt) {
      super.update(dt);
      double age=getAge();
      
      //-- adjust the scaling of the polyehdron
      fPoint3d scale=getPolyhedronInstance().getScalingFactor();
      if(age>(time1+time2)){
         die();
      }else if(age>time1){
         scale.plus(dScale2*dt);
      } else {
         scale.plus(dScale1*dt);
      }
      getPolyhedronInstance().setScalingFactor(scale);

      //-- adjust the position so that the bottom always touches
      //-- the ground
      fPoint3d p=getPosition();
      p.y=scale.y;
      setPosition(p);
   }

   public double getStrength(){
      return strength;
   }
   protected double dScale1,time1,dScale2,time2;
   protected double strength;

	public static void initiateClass (Applet app) {
     	app.showStatus(" cmGenericExplosion : initiating class");
		 		try{
		 			ourDefaultPolyhedron=new fConvexPolyhedron(new URL(app.getCodeBase(),app.getParameter("cmGenericExplosion_polyhedronFile")).openStream());
		 		} catch(Exception e) {e.printStackTrace();}
 				nbrOfFragments=new Double(app.getParameter("cmGenericExplosion_nbrOfFragments")).doubleValue();
 				fragmentsSpeed=new Double(app.getParameter("cmGenericExplosion_fragmentsSpeed")).doubleValue();
 				fragmentsSize=new Double(app.getParameter("cmGenericExplosion_fragmentsSize")).doubleValue();
 				fragmentsRotation=new Double(app.getParameter("cmGenericExplosion_fragmentsRotation")).doubleValue();

	}

	protected static fPolyhedron ourDefaultPolyhedron;
	protected static double nbrOfFragments,fragmentsSpeed,fragmentsSize,fragmentsRotation;
}

