import java.util.*;

abstract class cmAbstractBrain extends Object {
   protected Vector myEvents;
   protected cmAbstractPlayer theHost;
   protected fWorld theWorld;

   protected cmAbstractBrain(cmAbstractPlayer host){
      theHost=host;
      theWorld=theHost.getWorld();
      myEvents=new Vector(10,10);
      theHost.setBrain(this);
   }

  	public void handleEvents () {
		int n=myEvents.size();
		while((n-->0) && handleEvent((fEvent)myEvents.elementAt(n)));
		//-- all collisions are handled. clear the list.
		myEvents.removeAllElements();
	}

	public void addEvent (fEvent ev) {
		myEvents.addElement(ev);
	}

	protected boolean handleEvent (fEvent event) {
		return true;
	}

	public void update (double dt){
	   handleEvents();
	}

	public void abortJob(){
	}

	protected int getUniqueId(){
	   return idCount++;
	}

	private int idCount;
}

class cmMotionBrain extends cmAbstractBrain {

   cmMotionBrain(cmAbstractPlayer host){
      super(host);
   }
   
	protected boolean handleEvent (fEvent event) {
  	   fWorld.debug(this," handling event");
	   if(event instanceof cmEventMotionCommand){
	      cmEventMotionCommand ev=(cmEventMotionCommand)event;
	      state=ADJUSTING_HEIGHT;
	      destination=ev.dest;
	      maxError=ev.precision;
     		return true;
	   } else return super.handleEvent(event);
	}

	public void update (double dt) {
	   super.update(dt);
	   if(state==ADJUSTING_HEIGHT){
	      double error1=turnTowardsPoint(destination,dt);
	      double error2=climbTowardsPoint(destination,dt);
	      if( (error1<maxError) && (error2<maxError) ){
            state=FLYING;
	      }	         
      } else if(state==FLYING){
         turnTowardsPoint(destination,dt);
         moveTowardsPoint(destination,dt);
      }
	}

   protected double climbTowardsPoint(fPoint3d dest,double dt){
      double error=destination.y-theHost.getPosition().y;
      //-- check if I should climb or decent
      if(error>0){
	      theHost.climb(Math.min(1,Math.abs(error/theHost.climbrate)),dt);
	   }else{
	      theHost.decent(Math.min(1,Math.abs(error/theHost.decentrate)),dt);
	   }
	   return error;
   }

   protected double turnTowardsPoint(fPoint3d dest,double dt){
      double error=getDeterminant(dest);
      if(error<0){
         theHost.turnRight(Math.min(1,Math.abs(error/theHost.turningrate)),dt);
      }else{
         theHost.turnLeft(Math.min(1,Math.abs(error/theHost.turningrate)),dt);
      }
      return error;
   }

   protected double moveTowardsPoint(fPoint3d ref,double dt){
      theHost.increaseVelocity(1,dt);
      fPoint3d v=new fPoint3d(theHost.getPosition(),ref);
      return v.magnitude();
   }

	public void gotoPosition(fPoint3d pos,double error){
	   addEvent(new cmEventMotionCommand(theWorld.getTime(),getUniqueId(),cmEventMotionCommand.GOTO_POSITION,pos,error));
	}
   	
   private double getDeterminant(fPoint3d dest){
      //-- make vector from host position to destination
      fPoint3d v1=new fPoint3d(theHost.getPosition(),dest);
      //-- make vector for host's direction
      fPoint3d v2=new fPoint3d(0,0,-1);
      v2.rotateAboutYaxis(theHost.getAngle().y);
      //-- check if destination is infront of the host
      if(v1.dotProduct(v2)>0){
         //-- destination infront of the host
         v1.normalize(1);
         return v1.x*v2.z-v2.x*v1.z;
      } else {
         //-- destination behind host
         double temp=v1.x*v2.z-v2.x*v1.z;
         if(temp<0){
            return -1;
         }else{
            return 1;
         }
      }
   }

   public void abortJob(){
      super.abortJob();
      this.state=this.STANDBY;
   }
   
   fPoint3d destination;
   double maxError;
	protected int state;
	protected static final int STANDBY=1,ADJUSTING_HEIGHT=2,ADJUSTING_ANGLE=3,HOVERING=4,FLYING=5;
}

class cmAttackBrain extends cmMotionBrain{
   cmAttackBrain(cmAbstractPlayer host){
      super(host);
      state=STANDBY;
   }

   public void selectTarget(fObject target){
      this.target=target;
   }

   public void attackTarget(){
      state=ATTACKING;
   }

   protected double distanceToTarget(){
      return target.distanceToPoint(theHost.getPosition());
   }

	public void update (double dt) {
	   super.update(dt);
	   if(state==ATTACKING){
   	   if( Math.abs(turnTowardsPoint(target.getPosition(),dt))<0.2){
   	      if(distanceToTarget()<30){
      	      theHost.fireSelectedWeapon();
   	      }else{
   	         moveTowardsPoint(target.getPosition(),dt);
   	      }
   	   }
	   }
	}

   public void abortJob(){
      super.abortJob();
      this.state=this.STANDBY;
   }

   protected fObject target;
   protected int state;
   protected static final int STANDBY=1,ATTACKING=2;
}

class cmEventMotionCommand extends fEvent {

   cmEventMotionCommand(double time,int id0,int com,fPoint3d dest0,double precision0){
      super(time);
      id=id0; command=com; dest=dest0; precision=precision0;
   }

   fPoint3d dest;
   double precision;
   int id,command;
   static int GOTO_POSITION=1;
}


