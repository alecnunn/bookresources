import java.util.*;

abstract class cmAbstractVehicle extends cmAbstractMovingObject{
	protected cmAbstractVehicle (fWorld w, fPoint3d pos, fVelocityVector v,
	                             double turningrate0, double pitchrate0,
	                             double acceleration0, double brakingrate0,
	                             double maxVelocity0, double climbrate0,
	                             double decentrate0, double pitchClimbrateRelation0,
	                             double health)
	{
      super(w,pos,v.getAngle(),v,new fAngle3d(0,0,0),health);
		
      turningrate=turningrate0;
      pitchrate=pitchrate0;
      acceleration=acceleration0;
      brakingrate=brakingrate0;
      maxVelocity=maxVelocity0;
      climbrate=climbrate0;
      decentrate=decentrate0;

      selectedWeapon=-1;
      myWeapons=new Vector(5,1);
      
      pitchClimbrateRelation=pitchClimbrateRelation0;
	}

	protected cmAbstractVehicle (fWorld w, fPoint3d pos, fVelocityVector v,
	                             double turningrate0, double acceleration0,
	                             double brakingrate0, double maxVelocity0,
	                             double health)
	{
      super(w,pos,v.getAngle(),v,new fAngle3d(0,0,0),health);

      turningrate=turningrate0;
      acceleration=acceleration0;
      brakingrate=brakingrate0;
      maxVelocity=maxVelocity0;

      selectedWeapon=-1;
      myWeapons=new Vector(5,1);
   }

   public boolean interestedOfCollisionWith (fObject obj) {
	   if(obj instanceof cmAbstractStaticStructure) return true;
	   if(obj instanceof cmAbstractMovingStructure) return true;
	   if(this instanceof cmAbstractRound){
	      //-- if this a round then it shouldn't be interested about
	      //-- collisions with other rounds
   	   if(obj instanceof cmAbstractRound) return false;
	   }
	   if(obj instanceof cmAbstractVehicle) return true;
	   return super.interestedOfCollisionWith(obj);
	}

   protected boolean handleCollisionWith (fObject obj,double dt) {
      if( (obj instanceof cmAbstractMovingStructure)||(obj instanceof cmAbstractStaticStructure) ){
         oldStates();
         return true;
      }
      if(obj instanceof cmAbstractRound){
         //-- check if the round comes from this vehicle
	      cmAbstractRound round = (cmAbstractRound)obj;
	      if (round.getShooter()==this){
	         return true;
	      }
         myHealth-=((cmAbstractRound)obj).getImpactDamage();
         if(myHealth<0) {
            die();
            return false;
         }
      }
      //-- check if a vehicle, must be done after abstract round
      //-- since an abstract round is also a vehicle
      if(obj instanceof cmAbstractVehicle){
         oldStates();
         return true;
      }
      return super.handleCollisionWith(obj,dt);
   }

   private void oldStates(){
      setPosition(myLastPos);
      setAngle(myLastAgl);
      ((fVelocityVector)dPos).setVelocity(0);
   }
	
   public void addWeapon(cmAbstractWeapon wep){
      myWeapons.addElement(wep);
   }

   public void removeWeapon(cmAbstractWeapon wep){
      myWeapons.addElement(wep);
   }

   public boolean selectWeapon(int wep){
      if(wep>-1 && wep<myWeapons.size()){
         selectedWeapon=wep;
         return true;
      }
      return false;
   }
         

   fPoint3d myLastPos;
   fAngle3d myLastAgl;

	public void update (double dt) {
	   myLastPos=getPosition();
	   myLastAgl=getAngle();
	   
		super.update(dt);

		//-- synchronize this object's angle with the
		//-- angle in the velocity vector
		fVelocityVector v=(fVelocityVector)getdPosition();
		fAngle3d a=getAngle();
		v.synchronizeAngle(a);
		setAngle(a);

		//-- update the weapons
		int nbr=myWeapons.size();
		for(int n=0;n<nbr;n++){
		   ((cmAbstractWeapon)myWeapons.elementAt(n)).update(dt);
		}
	}

   public void fireSelectedWeapon(){
      addEvent(new cmEventWeaponCommand(getWorld().getTime(),cmEventWeaponCommand.FIRE,0));
   }

	public void turnLeft (double factor, double dt) {
		addEvent(new cmEventSteeringCommand(getWorld().getTime(),cmEventSteeringCommand.TURN_LEFT,factor,dt));
	}

	public void turnRight (double factor, double dt) {
		addEvent(new cmEventSteeringCommand(getWorld().getTime(),cmEventSteeringCommand.TURN_RIGHT,factor,dt));
	}

	public void increaseVelocity (double factor, double dt) {
		addEvent(new cmEventSteeringCommand(getWorld().getTime(),cmEventSteeringCommand.INCREASE_VELOCITY,factor,dt));
	}

	public void decreaseVelocity (double factor, double dt) {
		addEvent(new cmEventSteeringCommand(getWorld().getTime(),cmEventSteeringCommand.DECREASE_VELOCITY,factor,dt));
	}

	public void brake (double factor, double dt) {
		addEvent(new cmEventSteeringCommand(getWorld().getTime(),cmEventSteeringCommand.BRAKE,factor,dt));
	}

	public void climb (double factor, double dt) {
		addEvent(new cmEventSteeringCommand(getWorld().getTime(),cmEventSteeringCommand.CLIMB,factor,dt));
	}

	public void decent (double factor, double dt) {
		addEvent(new cmEventSteeringCommand(getWorld().getTime(),cmEventSteeringCommand.DECENT,factor,dt));
	}

	public void pitchUp (double factor, double dt) {
		addEvent(new cmEventSteeringCommand(getWorld().getTime(),cmEventSteeringCommand.PITCH_UP,factor,dt));
	}

	public void pitchDown (double factor, double dt) {
		addEvent(new cmEventSteeringCommand(getWorld().getTime(),cmEventSteeringCommand.PITCH_DOWN,-factor,dt));
	}

	protected boolean handleEvent (fEvent event) {
		      if(event instanceof cmEventSteeringCommand){
				   cmEventSteeringCommand com=(cmEventSteeringCommand)event;
		         if(com.command==cmEventSteeringCommand.TURN_LEFT){
				      handleTurnLeft(com.factor,com.dt);
				      return true;
		         }else if(com.command==cmEventSteeringCommand.TURN_RIGHT){
		            handleTurnRight(com.factor,com.dt);
		   		   return true;
		         }else if(com.command==cmEventSteeringCommand.INCREASE_VELOCITY){
		            handleIncreaseVelocity(com.factor,com.dt);
		   		   return true;
		         }else if(com.command==cmEventSteeringCommand.DECREASE_VELOCITY){
		            handleIncreaseVelocity(com.factor,com.dt);
		   		   return true;
		         }else if(com.command==cmEventSteeringCommand.BRAKE){
				      handleBrake(com.factor,com.dt);
		   		   return true;
				   }else if(com.command==cmEventSteeringCommand.CLIMB){
				      handleClimb(com.factor,com.dt);
		   		   return true;
				   }else if(com.command==cmEventSteeringCommand.DECENT){
				      handleDecent(com.factor,com.dt);
		   		   return true;
		   	   }else if(com.command==cmEventSteeringCommand.PITCH_DOWN){
				      handlePitch(com.factor,com.dt);
		   		   return true;
				   }else if(com.command==cmEventSteeringCommand.PITCH_UP){
				      handlePitch(com.factor,com.dt);
		   		   return true;
		   	   }
		      } else
		      if(event instanceof cmEventWeaponCommand){
		         cmEventWeaponCommand com=(cmEventWeaponCommand)event;
		         if(com.command==cmEventWeaponCommand.SELECT){
		            selectWeapon(com.arg-20);
		            return true;
		         } else if (com.command == cmEventWeaponCommand.FIRE) {
                  if(selectedWeapon!=-1){
                     ((cmAbstractWeapon)myWeapons.elementAt(selectedWeapon)).fire();
                  }
                  return true;
      		   }
		      }
		      return super.handleEvent(event);
	}

	protected void handleTurnLeft (double factor, double dt) {
		fVelocityVector v=(fVelocityVector)getdPosition();
		v.increaseAngleAboutYaxis( turningrate*factor*dt);
		setdPosition(v);
	}

	protected void handleTurnRight (double factor, double dt) {
		fVelocityVector v=(fVelocityVector)getdPosition();
		v.increaseAngleAboutYaxis(-turningrate*factor*dt);
		setdPosition(v);
	}

	protected void handleIncreaseVelocity (double factor, double dt) {
		fVelocityVector v=(fVelocityVector)getdPosition();
		if(v.getVelocity()<maxVelocity){
		   v.increaseVelocity(acceleration*factor*dt);
		   setdPosition(v);
		}
	}

	protected void handleDecreaseVelocity (double factor, double dt) {
		fVelocityVector v=(fVelocityVector)getdPosition();
		if(v.getVelocity()>-maxVelocity){
		   v.increaseVelocity(-acceleration*factor*dt);
		   setdPosition(v);
		}
	}

	protected void handleBrake (double factor, double dt) {
		fVelocityVector v=(fVelocityVector)getdPosition();
		if(v.getVelocity()>0){
		   v.increaseVelocity(-brakingrate*factor*dt);
		} else if(v.getVelocity()<0){
		   v.setVelocity(0);
		}
		setdPosition(v);
	}

	protected void handlePitch (double factor, double dt) {
		fVelocityVector v=(fVelocityVector)getdPosition();
		v.increasePitch(-pitchrate*factor*dt);
		setdPosition(v);
	}

	protected void handleClimb (double factor, double dt) {
	   fPoint3d p=getPosition();
	   p.y+=climbrate*dt*factor;
	   setPosition(p);
	}

	protected void handleDecent (double factor, double dt) {
	   fPoint3d p=getPosition();
	   p.y-=climbrate*dt*factor;
	   setPosition(p);
	}

   Vector myWeapons;
   int selectedWeapon;

	protected double turningrate;
	protected double pitchrate;
	protected double pitchClimbrateRelation;
	protected double acceleration;
	protected double brakingrate;
	protected double maxVelocity;
	protected double climbrate;
	protected double decentrate;
}

class cmEventSteeringCommand extends fEvent{
	cmEventSteeringCommand (double time, int com, double factor0, double dt0) {
		super(time);
		command=com;
		factor=factor0;
		dt=dt0;
	}

	int command;
	double factor;
	double dt;
	static final int TURN_LEFT = 1,TURN_RIGHT=2,INCREASE_VELOCITY=3,BRAKE=4,
              CLIMB=5,DECENT=6,PITCH_UP=7,PITCH_DOWN=8,DECREASE_VELOCITY=9;
}


class cmEventWeaponCommand extends fEvent{
	cmEventWeaponCommand (double time, int command,int arg) {
		super(time);
		this.command=command;
		this.arg=arg;
	}

	int command,arg;
	static final int FIRE = 19,SELECT=18,MINICANNON=20,MISSILE=21,BOMB=22;
}
/*
abstract class cmAbstractVehicle extends cmAbstractMovingObject{
   //-- the constructor for flying vehicles
	protected cmAbstractVehicle (fWorld w, fPoint3d pos, fVelocityVector v,
	                             double turningrate, double pitchrate,
	                             double acceleration, double brakingrate,
	                             double maxVelocity, double climbrate,
	                             double decentrate, double pitchClimbrateRelation)

   //-- the constructor for ground vehicles
	protected cmAbstractVehicle (fWorld w, fPoint3d pos, fVelocityVector v,
	                             double turningrate0, double acceleration0,
	                             double brakingrate0, double maxVelocity0)

   public void addWeapon(cmAbstractWeapon wep)

   public void removeWeapon(cmAbstractWeapon wep)

   public boolean selectWeapon(int wep)

   public void update (double dt)

   //--
   //-- event creating methods
   //--
   public void fireSelectedWeapon()

   public void turnLeft (double factor, double dt)

   public void turnRight (double factor, double dt)

   public void increaseVelocity (double factor, double dt) 

   public void decreaseVelocity (double factor, double dt) 

   public void brake (double factor, double dt) 

   public void climb (double factor, double dt) 

   public void decent (double factor, double dt) 

   public void pitchUp (double factor, double dt) 

   public void pitchDown (double factor, double dt)
	
   //--
   //-- event handling methods
   //--
   protected void handleTurnLeft (double factor, double dt)

   protected void handleTurnRight (double factor, double dt)

   protected void handleIncreaseVelocity (double factor, double dt)
	
   protected void handleDecreaseVelocity (double factor, double dt)

   protected void handleBrake (double factor, double dt)

   protected void handleClimb (double factor, double dt)

   protected void handleDecent (double factor, double dt)
}

*/
