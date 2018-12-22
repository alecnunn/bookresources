import java.util.Vector;
/**
 * Represents a virtual map of the world divided into grids which are populated by objects. The map is only used to optimize collision detection.
 * @see fGrid
 */
class fMap extends Object {
	/**
	 * Constructs a map
	 *
	 * Xmin,Ymin ----+----
	 *               |
	 *           ----+----
	 *               |
	 *           ----+----Xmin+size,Ymin+size
	 */
	fMap(double Xmin0,double Ymin0,double size0,int rows0){
		rows=rows0; Xmin=Xmin0; Ymin=Ymin0;
		nbrGrids=rows*rows;
		myGrids=new fGrid[nbrGrids];
		for(int n=0;n<nbrGrids;n++){
			myGrids[n]=new fGrid();
		}
		gridSize=size0/rows;
		gridSizeInv=(double)rows/size0;
	}
   /**
    * Inserts the grids that are within the sphere into the supplied vector.
    */
	final void getGridsForSphere(fPoint3d p,double radius,Vector grids){
	   double x=p.x,y=p.z;
	   
	   //-- calculate the number of grids needed to cover the radius
      int xstart=getGridX(x-radius);
      int xend  =getGridX(x+radius);
   
	   //-- calculate the starting y position in the map
      int ystart=getGridY(y-radius);
      int yend  =getGridY(y+radius);

      //-- insert all those grids into the array provided
      for(int yy=ystart;yy<=yend;yy++){
         for(int xx=xstart;xx<=xend;xx++){
            grids.addElement(myGrids[yy*rows+xx]);
         }
      }
	}
   /**
    * Inserts all the objects within the specified radius into the supplied vector.
    */
	final synchronized void getAllObjectsInRadius(fPoint3d p,double radius,Vector vect){
	   double x=p.x,y=p.z;
	   
	   //-- calculate the number of grids needed to cover the radius
      int xstart=getGridX(x-radius);
      int xend  =getGridX(x+radius);
   
	   //-- calculate the starting y position in the map
      int ystart=getGridY(y-radius);
      int yend  =getGridY(y+radius);

      //-- insert all the objects in the grids into the vector
      for(int yy=ystart;yy<=yend;yy++){
         for(int xx=xstart;xx<=xend;xx++){
            myGrids[yy*rows+xx].putAllObjectsInVector(vect);
         }
      }
      
      //-- remove marked objects and objects that are out of the radius
      double radius2=radius*radius;
      int nbr=vect.size();
      for(int n=0;n<nbr;n++){
         fObject obj=(fObject)vect.elementAt(n);
         if(obj.flags==1){
            vect.removeElementAt(n);
            nbr--; n--;
         } else if(obj.getPosition().distanceToPoint2(p)>radius2){
            vect.removeElementAt(n);
            nbr--; n--;
         } else obj.flags=1;
      }
      //-- remove objects that are not truly within the radius
      nbr=vect.size();
      for(int n=0;n<nbr;n++){
         fObject obj=(fObject)vect.elementAt(n);
         obj.flags=0;
      }
	}
	final synchronized void getAllObjectsInRadiusAndInfront(fPoint3d pos,fPoint3d dir,double radius,Vector vect){
	   getAllObjectsInRadius(pos,radius,vect);
      //-- remove objects that are behind the plane
      int nbr=vect.size();
      for(int n=0;n<nbr;n++){
         fObject obj=(fObject)vect.elementAt(n);
         fPoint3d v=new fPoint3d(pos,obj.getPosition());
         if(v.dotProduct(dir)<0){
            vect.removeElementAt(n);
            nbr--; n--;
         }
      }
	}

	/**
	 * Detects all collisions between objects in this map.
	 */
	final void detectCollisions(double dt){
		for(int n=0;n<nbrGrids;n++){
			myGrids[n].detectCollisions(dt);
		}
	}
   /**
    * Update the map by "dt" seconds.
    */
	final void update(double dt){
		for(int n=0;n<nbrGrids;n++){
			myGrids[n].removeAllMovingObjects();
		}
	}

   private int getGridX(double x){
      int xx=(int)((x-Xmin)/gridSize);
      if(xx<0) {
         return 0;
      }else if(xx>=rows){
         return rows-1;
      }else{
         return xx;
      }
   }

   private int getGridY(double y){
      int yy=(int)((y-Ymin)/gridSize);
      if(yy<0) {
         return 0;
      }else if(yy>=rows){
         return rows-1;
      }else{
         return yy;
      }
   }
   
	/**
	 * My grids.
	 */
	protected fGrid myGrids[];
	/**
	 * Number of rows and columns in this map. Rows = columns => map should be a square.
	 */
	protected int rows;
	/**
	 * The width of a grid in world coordinates.
	 */
	protected double gridSize;
	/**
	 * The width of a grid inverted. Precalculated.
	 */
	protected double gridSizeInv;
	/**
	 * My left-,topmost point.
	 */
	protected double Xmin,Ymin;
	/**
	 * Number of grids in this map.
	 */
	 protected int nbrGrids;
}
