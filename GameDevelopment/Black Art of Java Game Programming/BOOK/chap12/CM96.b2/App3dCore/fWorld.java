import java.awt.*;
import java.applet.*;
import java.util.Vector;
import java.util.Stack;
/**
 * Represents the world in which all objects reside.
 */
public abstract class fWorld extends Object{
   /**
    * Constructs the world world.
    */
   protected fWorld(Applet app,double Xmin,double Ymin,double size,int rows){
      //-- make my map.
      myMap=new fMap(Xmin,Ymin,size,rows);
      //-- create the stack with new object.
      newObjects=new Stack();
      //-- create the vector with all my objects
      myObjects=new Vector(500,100);
      myTime=0;
      //-- initiate all classes
      initiateClasses(app);
   }
   /**
    * Reset the simulation.
    */
   public void reset(){
      myObjects.removeAllElements();
      newObjects.removeAllElements();
      myTime=0.0;
   }
   /**
    * Inserts a object into the world. The object will be pushed into the
    * stack and then inserted first thing next round.
    */
	public final void insertObject(fObject obj){
		newObjects.push(obj);
	}
	/**
	 * Update the world by dt seconds.
	 */
	public void update(double dt){
		myTime+=dt;
		//-- insert new objects.
		insertNewObjects();
		//-- let extended worlds make their intervention.
		devineIntervention();
		//-- clear map
		myMap.update(dt);
		//-- update all objects.
		updateObjects(dt);
		//-- detect collisions.
		myMap.detectCollisions(dt);
		//-- let objects handle their events.
		handleEvents();
	}
   /**
    * Here is a chance for extended worlds to affect their objects.
    */
   protected void devineIntervention(){
   }
   /**
    * Updates all objects.
    */
   private void updateObjects(double dt){
      int size=myObjects.size();
      for(int n=0;n<size;n++){
         fObject obj=(fObject)myObjects.elementAt(n);
         if(obj.isAlive()){
            //-- update object if it still alive
            obj.update(dt);
         }else{
            //-- remove it
            myObjects.removeElementAt(n);
            size--; n--;
         }
      }
   }
   /**
    * Instructs all objects to handle their collisions.
    */
   private void handleEvents(){
      int nbr=myObjects.size();
      for(int n=0;n<nbr;n++){
         ((fObject)myObjects.elementAt(n)).handleEvents();
      }
   }
   /**
    * Inserts all new objects from the stack into the world.
    */
   private void insertNewObjects(){
      while(newObjects.empty()==false){
         myObjects.addElement(newObjects.pop());
      }
   }
   
   public final double getTime() {
      return myTime;
   }
   public final fMap getMap() {
      return myMap;
   }

   protected abstract void initiateClasses(Applet app);
   /**
    * Inserts all objects that are within radius into vect.
    */
	public final synchronized void getAllObjectsInRadius(fPoint3d pos,double radius,Vector vect){
	   myMap.getAllObjectsInRadius(pos,radius,vect);
	}
   /**
    * Inserts all objects that are within radius and infront of the plane specified by
    * a point (pos) and a normal (norm).
    */
	public final synchronized void getAllObjectsInRadiusAndInfront(fPoint3d point,fPoint3d norm,double radius,Vector vect){
	   myMap.getAllObjectsInRadiusAndInfront(point,norm,radius,vect);
	}
   /**
    * Returns a random number between b and e.
    */
   public final static double rand(double b,double e){
      return (e-b)*Math.random()+b;
   }
   /**
    * Prints the class name and a string to stout.
    */
   public final static void debug(Object source,String str){
      System.out.println("***"+source.getClass().getName()+" : "+str);
   }
	/**
	 * This world's map.
	 */
	protected fMap myMap;
	/**
	 * Stack of new objects that should enter the world next round.
	 */
	protected Stack newObjects;
	/**
	 * All objects in this world.
	 */
	protected Vector myObjects;
	/**
	 * The time in this world.
	 */
	protected double myTime;
}
