import  java.awt.*;
import  java.util.*;

public class fTrackerCamera extends fMagicCamera{
	public fTrackerCamera (fWorld w, double viewangle, double viewdistance,double gridSize,double fading, double speed,fObject theObject,fAngle3d relagl,fPoint3d relpos) {
		super(w,viewangle,viewdistance,theObject.getPosition(),theObject.getAngle(),gridSize,fading);
		speedFactor=speed;
		this.theObject=theObject;
		this.relagl=relagl.makeClone();
		this.relpos=relpos.makeClone();
		init();
	}

   private void init(){
		relagl.negate();
   }

	public void update (double dt) {
	   fPoint3d thePointToTrack=theObject.getWorldCoordForRelativePoint(relpos);
		
		fPoint3d v=new fPoint3d(myPosition,thePointToTrack);      		
		v.times(speedFactor*dt);
		v.plus(myPosition);
/*
      fPoint3d v2=new fPoint3d(myPosition,theObject.getPosition());
      double ay=Math.atan2(v2.x,-v2.z);
      double ax=Math.atan2(v2.y,-v2.z);
  */    
      fAngle3d b=theObject.getAngle();
      b.plus(relagl);
		fAngle3d a=new fAngle3d(myAngle,b);
		a.times(speedFactor*dt);
		a.plus(myAngle);

		setOrientation(v,a);
	}

	public void setRelativePosition(fPoint3d p){
	   relpos=p.makeClone();
	}

	public void setRelativeAngle(fAngle3d p){
	   relagl=p.makeClone();
	}

	public fPoint3d getRelativePosition(){
	   return relpos.makeClone();
	}

	public fAngle3d getRelativeAngle(){
	   return relagl.makeClone();
	}

	protected double speedFactor;
	protected fObject theObject;
	protected fAngle3d relagl;
	protected fPoint3d relpos;
}

