import java.util.Vector;
/**
 * Represents a grid in fMap.
 * @see fMap
 */
class fGrid extends Object {
	protected Vector theMovingObjects;
	protected Vector theStaticObjects;
	/**
	 * Constructs a grid.
	 */
	fGrid() {
		theStaticObjects=new Vector(20,20);
		theMovingObjects=new Vector(20,20);
	}
	/**
	 * Inserts a object into this grid.
	 */
	final void insertObject(fObject obj) {
	   if(obj instanceof fMovingObject){
   	   theMovingObjects.addElement(obj);
	   } else {
   	   theStaticObjects.addElement(obj);
	   }
   }
	/**
	 * Removes a object from this grid.
	 */
	final void removeObject(fObject obj) {
	   if(obj instanceof fMovingObject){
	      theMovingObjects.removeElement(obj);
	   } else {
   	   theStaticObjects.removeElement(obj);
	   }
	}
	/**
	 * Removes all moving object from this grid.
	 */
	final void removeAllMovingObjects() {
	   theMovingObjects.removeAllElements();
	}
	/**
	 * Detects all collisions in this grid.
	 */
	final void detectCollisions(double dt) {
	   //-- get some "short-cuts"
	   int nbrMovingObjects=theMovingObjects.size();
      int nbrStaticObjects=theStaticObjects.size();
      
      //-- check for collision between moving objects if there is more than
      //-- one moving object
	   //-- in this grid
	   if(nbrMovingObjects>1){
	      fObject ObjA,ObjB;
   		for(int n=nbrMovingObjects-1;n>0;n--){
	   		ObjA=(fObject)theMovingObjects.elementAt(n);
		   	for(int m=n-1;m>=0;m--){
			   	ObjB=(fObject)theMovingObjects.elementAt(m);
   				//-- check if object A is interseted in a collision with object B.
   				boolean AinB=ObjA.interestedOfCollisionWith(ObjB);
   				//-- check if object B is interseted in a collision with object A.
   				boolean BinA=ObjB.interestedOfCollisionWith(ObjA);
   				if(AinB || BinA){
   					if(ObjA.checkForCollisionWith(ObjB,dt)){
   						//-- add the collision to the interested parts.
   						if(AinB) {ObjA.collisionWith(ObjB,dt);}
   						if(BinA) {ObjB.collisionWith(ObjA,dt);}
                  }
               }
            }
   		}
	   }

	   //-- check collision between moving objects and static objects
      fObject ObjA,ObjB;
   	for(int n=0;n<nbrMovingObjects;n++){
   	   //-- get a moving object
	      ObjA=(fObject)theMovingObjects.elementAt(n);

	   	//-- check it with all static objects in this grid
		   for(int m=0;m<nbrStaticObjects;m++){
		      //-- get the static object
			  	ObjB=(fObject)theStaticObjects.elementAt(m);
			  	
   			//-- check if A is interseted in a collision with B.
   			boolean AinB=ObjA.interestedOfCollisionWith(ObjB);
   			
   			//-- check if B is interseted in a collision with A
   			boolean BinA=ObjB.interestedOfCollisionWith(ObjA);
   			
   			if(AinB || BinA){
   			   //-- if any of them is interested
   				if(ObjA.checkForCollisionWith(ObjB,dt)){
   					//-- add the collision to the interested parts.
   					if(AinB) ObjA.collisionWith(ObjB,dt);
   					if(BinA) ObjB.collisionWith(ObjA,dt);
               }
            }
         }
      }
	}
	
	final void putAllObjectsInVector(Vector vect){
  	   int nbrMovingObjects=theMovingObjects.size();
      for(int n=0;n<nbrMovingObjects;n++){
         vect.addElement(theMovingObjects.elementAt(n));
      }

      int nbrStaticObjects=theStaticObjects.size();
      for(int n=0;n<nbrStaticObjects;n++){
            vect.addElement(theStaticObjects.elementAt(n));
      }
	}

}
