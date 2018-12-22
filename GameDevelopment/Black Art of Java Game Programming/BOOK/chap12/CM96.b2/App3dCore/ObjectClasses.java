import  java.awt.Graphics;
import  java.awt.Color;
import  java.util.Vector;

/**
 * Abstract class the represents a virtual object.
 */
abstract class fObject extends Object{
   /** 
    * Constructs a object.
    * world = the world in which the object will be inserted
    * pos   = the initial position
    * angle = the angle of the object
    */ 	 
	public fObject (fWorld world, fPoint3d pos, fAngle3d angle) {
		theWorld=world;
		Pos=pos.makeClone();
		Agl=angle.makeClone();
		
		myTime=0.0;
		alive=true;
		theOccupiedGrids=new Vector(4,4);
		myEvents=new Vector(20,10);
		//-- insert object into the world
		world.insertObject(this);
	}
   /**
    * Checks collision with another object. Only implmeneted by checking the boudning circles.
    */
	boolean checkForCollisionWith (fObject obj,double dt) {
      return myPolyhedronInstance.checkForCollisionWith(obj.myPolyhedronInstance);
	}
   /**
    * Updates this object by dt seconds.
    */
   public void update (double dt) {
      //-- not much in the base class except for aging.
      myTime+=dt;
   }
	/**
    * Calls handleEvent for every event in the list.
    * @see #handleEvent
    */
   final void handleEvents () {
      int n=myEvents.size();
      while((n-->0) && handleEvent((fEvent)myEvents.elementAt(n)));
      //-- all collisions are handled. clear the list.
      myEvents.removeAllElements();
	}
	/**
	 * The core will ask this object if it is interested of
	 * collision with some other object. Return true if the object
	 * is interested.
	 */
	public boolean interestedOfCollisionWith (fObject obj) {
		return false;
	}
   /**
    * Creates a collision event which is added to the event list.
    */
	public final void collisionWith (fObject obj,double dt) {
		myEvents.addElement(new fEventCollision(myTime,obj,dt));
	}
	/**
	 * Add a raw event to this object.
	 */
	public final void addEvent (fEvent ev) {
		myEvents.addElement(ev);
	}
	/**
    * Paints this object on camera.
    */
	public void paint (Graphics g, fCamera cam) {
		myPolyhedronInstance.clipAndPaint(g,cam);
	}

	public void paintWithShading (Graphics g, fCamera cam, fPoint3d light) {
		myPolyhedronInstance.clipAndPaintWithShading(g,cam,light);
	}
   /**
    * Returns the distance of this object to some other point.
    */
	public final double distanceToPoint (fPoint3d toPoint) {
	   return Math.sqrt(Pos.distanceToPoint2(toPoint));
	}
   /**
    * Returns the polyhedron instance that is used by
    * this object.
    */
	public final fPolyhedronInstance getPolyhedronInstance () {
		return myPolyhedronInstance;
	}
   /**
    * Returns a clone of the position.
    */
	public final fPoint3d getPosition () {
		return Pos.makeClone();
	}
   /**
    * Returns a clone of the angle.
    */
	public final fAngle3d getAngle () {
		return Agl.makeClone();
	}
   /**
    * Returns true if the object is alive.
    */
	public final boolean isAlive () {
		return alive;
	}
   /**
    * Returns the age of this object.
    */
	public final double getAge () {
		return myTime;
	}
	/**
	 * Returns the world coordinate for a position relative this
	 * object.
	 */
   public fPoint3d getWorldCoordForRelativePoint(fPoint3d relpos){
      fPoint3d Pwcs=new fPoint3d();
      myPolyhedronInstance.transformPoint(relpos,Pwcs);
      return Pwcs;
   }
	/**
    * Updates the list of occupied grids. Moving objects will do this
    * every frame while static objects do it once.
    */
	protected final void updateTheOccupiedGrids () {
      //-- clear the list.
      theOccupiedGrids.removeAllElements();
      //-- get the new grids.
      theWorld.getMap().getGridsForSphere(Pos,myPolyhedronInstance.getBoundingRadius(),theOccupiedGrids);
      //-- insert this object in all occupied grids.
      for(int n=0;n<theOccupiedGrids.size();n++){
         ((fGrid)theOccupiedGrids.elementAt(n)).insertObject(this);
      }
	}
	/**
    * Handles an event. If it is a known event it will be cracked and the "handle" method
    * for this event will be called.
    * Returns false if the object doesn't want any more events. I.e. the object has died.
    * @see #handleEvents
    */
   protected boolean handleEvent (fEvent event) {
      if(event instanceof fEventCollision) {
         fEventCollision ev=(fEventCollision)event;
         return handleCollisionWith(ev.theObject,ev.dt);
      }
      return true;
	}
   /**
    * Handles a collision with a object. Returns false if there is no point in checking more collisions.
    * I.e. the object is dead.
    */
   protected boolean handleCollisionWith (fObject obj,double dt) {
      return false;
   }
	/**
	 * Instructs the object to use this fPolyhedronInstance.
	 */
	protected final void usePolyhedronInstance (fPolyhedronInstance poly) {
		      myPolyhedronInstance=poly;
		  		myPolyhedronInstance.setOrientation(Pos,Agl);
				updateTheOccupiedGrids();
	}
   /**
    * Kills this object.
    */
   protected void die () {
      alive=false;
      //-- remove the dead object from the map.
      int nbr=theOccupiedGrids.size();
      for(int n=0;n<nbr;n++){
         fGrid g=(fGrid)theOccupiedGrids.elementAt(n);
         g.removeObject(this);
      }
   }
   /**
    * Returns the world in which this object lives in.
    */
	protected final fWorld getWorld () {
		return theWorld;
	}

	protected boolean alive;
	protected fWorld theWorld;
	protected Vector myEvents;
	protected Vector theOccupiedGrids;
	protected fPolyhedronInstance myPolyhedronInstance;
	protected double myTime;
	protected fPoint3d Pos;
	protected fAngle3d Agl;
	int flags;
}


/**
 * Abstract class that represents a moving object
 */
abstract class fMovingObject extends fObject{
   protected fPoint3d dPos;
   protected fAngle3d dAgl;

   /**
    * Constructs a moving object.
    */
	public fMovingObject (fWorld W, fPoint3d pos, fAngle3d agl, fPoint3d dpos, fAngle3d dagl) {
		super(W,pos,agl);
		dPos=dpos.makeClone(); 
		dAgl=dagl.makeClone();
	}

	public void paintWithShading (Graphics g, fCamera cam, fPoint3d light) {
		myPolyhedronInstance.setOrientation(Pos,Agl);
		super.paintWithShading(g,cam,light);
	}

	public void paint (Graphics g, fCamera cam) {
		myPolyhedronInstance.setOrientation(Pos,Agl);
		super.paint(g,cam);
	}
	/**
    * Overrides. Updates the moving object. It extends the regular static object.
    */
	public void update (double dt) {
		super.update(dt);
		//-- since this is a moving object it's physics must be updated.
		updatePhysics(dt);
		//-- must be updated each round since this object might have left the old grids and entered new ones.
		updateTheOccupiedGrids();
	}
   /**
    * Overrides.
    */
	public boolean checkForCollisionWith (fObject obj,double dt) {
	   //-- update the position and angle of the polyhedron
		myPolyhedronInstance.setOrientation(Pos,Agl);
      //-- if the other object is a moving object. updates it too.
		if(obj instanceof fMovingObject){
		   obj.myPolyhedronInstance.setOrientation(obj.Pos,obj.Agl);
		}
		//-- let the overriden method check the collision
      return super.checkForCollisionWith(obj,dt);
	}
   /**
    * Updates the physical state of this object.
    */
   protected void updatePhysics (double dt) {
      Pos.plus(dPos,dt); Agl.plus(dAgl,dt);
   }

   public final fAngle3d getdAngle () {
      return dAgl.makeClone();
   }

   public final fPoint3d getdPosition () {
      return dPos.makeClone();
   }

   public final void setdPosition (fPoint3d p) {
   	dPos.set(p);
   }

   public final void setPosition (fPoint3d p) {
      Pos.set(p);
      updateTheOccupiedGrids();
   }

   public final void setdAngle (fAngle3d a) {
      dAgl.set(a);
   }

   public final void setAngle (fAngle3d a) {
      Agl.set(a);
   }
}

class fEventCollision extends fEvent {
   protected fObject theObject;
   protected double dt;
	
   fEventCollision(double time,fObject theObject,double dt){
      super(time);
      this.theObject=theObject;
      this.dt=dt;
   }
}
