import java.awt.*;
import java.io.*;
/**
 * Describes an abstract indexing polygon.
 */
abstract class fIndexingPolygon extends Object{
   /**
    * construct a polygon with the supplied indices
    */
	public fIndexingPolygon(int indices[],int n){
	   myIndices=indices;
		nbrIndices=n;
	}

   /**
    * construct a polygon from a stream
    */
	public fIndexingPolygon(InputStream is) throws IOException {
		fromString(is);
	}

   /**
    * paints a polygon. the 2d list of coordiantes must be supplied
    */
	public abstract void paint(Graphics g,int x[],int y[]);

   public void paintWithShading(Graphics g,int x[],int y[],double intensity){
      paint(g,x,y);
   }
   public void clipAndPaint(Graphics g,fProjectedPoints p,fGenericCamera cam){
      paint(g,p.x,p.y);
   }
   public void clipAndPaintWithShading(Graphics g,fProjectedPoints p,fGenericCamera camera,double intensity){
      paint(g,p.x,p.y);
   }
   /**
    * read a polygon from a stream
    */
	public void fromString(InputStream is) throws IOException {
		//-- make a stream tokenizer
   	StreamTokenizer stream = new StreamTokenizer (is);
		stream.commentChar('#');

		//-- get the # of indicies in this polygon
		stream.nextToken(); nbrIndices=(int)stream.nval;
		//-- allocate the vector
		myIndices=new int[nbrIndices];
		//-- read all indices
		for(int i=0;i<nbrIndices;i++){
			stream.nextToken(); myIndices[i]=(int)stream.nval;
		}
	}

   /**
    * make a string representation of a polygon
    */
	public String toString(){
		String str=new String();
		str=str+nbrIndices;
		for(int n=0;n<nbrIndices;n++){
			str=str+" "+myIndices[n];
		}
		return str;
	}
   /**
    * returns the normal of this polygon.
    */
   public fPoint3d getNormal(fArrayOf3dPoints a){
      int i0=myIndices[0];
      int i1=myIndices[1];
      int i2=myIndices[nbrIndices-1];

      fPoint3d p0=new fPoint3d(a.x[i0],a.y[i0],a.z[i0]);
      fPoint3d v1=new fPoint3d(p0,new fPoint3d(a.x[i1],a.y[i1],a.z[i1]));
      fPoint3d v2=new fPoint3d(p0,new fPoint3d(a.x[i2],a.y[i2],a.z[i2]));
      fPoint3d norm=new fPoint3d();
      norm.vectorProduct(v1,v2);
      norm.normalize(1);
      
      return norm;
   }
	
   /**
    * pokes out the 2d coordiantes and stores them into the
    * scratch polygon.
    */
   protected void copyIndexedPoints(int x[],int y[]){
		for(int n=0;n<nbrIndices;n++){
			int i=myIndices[n];
			ourScratchPoly.xpoints[n]=x[i];
			ourScratchPoly.ypoints[n]=y[i];
		}
		ourScratchPoly.npoints=nbrIndices;
	 }

   /**
    * determine the orientation of the scratch polygon.
    * if the result is positive then it is CW.
    */
	protected static int orientation() {
	   int p1x=ourScratchPoly.xpoints[1],p1y=ourScratchPoly.ypoints[1];
	   //-- vector from vertex #1 to vertex #2
	   int v1x=ourScratchPoly.xpoints[2]-p1x;
	   int v1y=ourScratchPoly.ypoints[2]-p1y;
	   //-- vector from vertex #1 to vertex #0
	   int v2x=ourScratchPoly.xpoints[0]-p1x;
	   int v2y=ourScratchPoly.ypoints[0]-p1y;
      //-- return the determinant of the vectors
	   return v1x*v2y-v2x*v1y;
	}

	public abstract fIndexingPolygon makeClone();
	/**
	 * the "scratch" polygon that is used for painting
	 */
	protected static Polygon ourScratchPoly=new Polygon(new int[50],new int[50],50);
	/**
	 * the indices that define this polygon
	 */
	protected int myIndices[];
	/**
	 * number of indices in this polygon.
	 */
	protected int nbrIndices;
	
}

/**
 * A solid color polygon.
 */
class fFilledPolygon extends fIndexingPolygon {
   /**
    * The color of this polygon.
    */
	protected fColor myColor;

	/**
	 * Create a polygon with the supplied data.
	 */
	public fFilledPolygon(int indices[],int n,fColor color){
		super(indices,n);
		myColor=color;
	}

	/**
	 * Create a polygon from a stream.
	 */
	public fFilledPolygon(InputStream is) throws IOException {
		super(is);
	}

	/**
	 * Paint the polygon if it is CCW
	 */
	public void paint(Graphics g,int x[],int y[]){
	   //-- copy the indexed coordiantes from the 2d list to
	   //-- the scratch-pad
      copyIndexedPoints(x,y);
      render(g);
	}

	protected void render(Graphics g){
		//-- check orientation
		if(orientation()>0){
			g.setColor(myColor.getColor());
			g.fillPolygon(ourScratchPoly);
		}
	}
      
	/**
	 * overrides fIndexingPolygon.toString()
	 * the color must also be written to the string.
	 */
	public String toString(){
	   //-- make the string for fIndexingPolygon
		String str=super.toString();
		//-- add the color and line break
		str=str+" "+myColor.toString()+"\n";
		return str;
	}

	/**
	 * overrides fIndexingPolygon.toString()
	 * the color must also be read from the stream.
	 */
	public void fromString(InputStream is) throws IOException {
		super.fromString(is);
		//-- read the color
		myColor=new fColor(is);
	}

   public fIndexingPolygon makeClone(){
      int i[];
      System.arraycopy(myIndices,0,i=new int[nbrIndices],0,nbrIndices);
      return new fFilledPolygon(i,nbrIndices,myColor.makeClone());
   }
}

class fClippableFilledPolygon extends fFilledPolygon {

   fClippableFilledPolygon(int indices[],int n,fColor col){
      super(indices,n,col);
   }

	fClippableFilledPolygon(InputStream is) throws IOException {
		super(is);
	}
   
   public void clipAndPaint(Graphics g,fProjectedPoints p,fGenericCamera cam){
  		int clipFlagsAndOp,clipFlagsOrOp,temp;

  		//-- gather information about the clipping needed for
		clipFlagsAndOp=clipFlagsOrOp=p.clipFlags[myIndices[0]];
		for(int n=1;n<nbrIndices;n++){
			temp=p.clipFlags[myIndices[n]];
			clipFlagsOrOp  |=temp;
			clipFlagsAndOp &=temp;
		}

		//-- check if all points of the polygon are
		//-- out of view.
		if(clipFlagsAndOp != 0){
		   //-- all points are on one side of the
		   //-- clipping volume meaning that the
		   //-- entire polygon is out of view.
		   return;
		}
		
      //-- check if any clipping is needed
		if(clipFlagsOrOp==0){
		   //-- no clipping needed, do the standard fill
		   paint(g,p.x,p.y);
   	   return;
		}
		
      //-- clipping is needed
	   if( (clipFlagsOrOp & cam.CLIP_Z)!=0 ){
	      //-- polygon points are behind the camera
	      //-- z-clipping needed.
	      int pleft;
         if( (pleft=clipZ(p,xt,yt,cam))==0){
            //-- no points left after clipping.
            return;
         }else{
            //-- polygon succsefully clipped
            copyPoints(xt,yt,pleft);
            super.render(g);
         }
      } else {
         paint(g,p.x,p.y);
      }
   }

   protected void copyPoints(int x[],int y[],int npts){
  		for(int n=0;n<npts;n++){
			ourScratchPoly.xpoints[n]=x[n];
			ourScratchPoly.ypoints[n]=y[n];
		}
		ourScratchPoly.npoints=npts;
   }

   protected int clipZ(fProjectedPoints p,int xt[],int yt[],fGenericCamera cam){
      int p0=0,pts=0;
      int i0=myIndices[p0];
      int p1,i1;
      
      boolean inside=p.z[i0]<cam.Zclip;
      if(inside){
         xt[0]=p.x[i0]; yt[0]=p.y[i0]; pts++;
      }
      
      for(int n=1;n<=nbrIndices;n++){
         if(n<nbrIndices) p1=n; else p1=0;
         i1=myIndices[p1];
         
         if(p.z[i1]<cam.Zclip){
            //-- point infront of the camera
            if(inside==false){
               //-- last point was "outside"
               cam.clipZandStore(p.x[i0],p.y[i0],p.z[i0],p.x[i1],p.y[i1],p.z[i1],xt,yt,pts);
               pts++;
            }
            //-- point inside, store it in the array
            inside=true;
            xt[pts]=p.x[i1]; yt[pts]=p.y[i1];
            pts++;
         } else {
            //-- point behind camera
            if(inside==true){
               //-- the last point was "inside" view volume
               cam.clipZandStore(p.x[i0],p.y[i0],p.z[i0],p.x[i1],p.y[i1],p.z[i1],xt,yt,pts);
               pts++;
            }
            inside=false;
         }
         i0=i1;
      }
      return pts;
   }

   public fIndexingPolygon makeClone(){
      int i[];
      System.arraycopy(myIndices,0,i=new int[nbrIndices],0,nbrIndices);
      return new fClippableFilledPolygon(i,nbrIndices,myColor.makeClone());
   }

   static int xt[]=new int[100];
   static int yt[]=new int[100];
}

class fShadedPolygon extends fClippableFilledPolygon{
   fShadedPolygon(int indices[],int n,fColor col){
      super(indices,n,col);
   }

	fShadedPolygon(InputStream is) throws IOException {
		super(is);
	}
   
   public void paintWithShading(Graphics g,int x[],int y[],double intensity){
      myColor.setIntensity(intensity);
      super.paint(g,x,y);
   }

   public void clipAndPaintWithShading(Graphics g,fProjectedPoints p,fGenericCamera camera,double intensity){
      myColor.setIntensity(intensity);
      super.clipAndPaint(g,p,camera);
   }
   
   public void calculateIntensityForLight(fPoint3d light){
      myColor.setIntensity(light.dotProduct(myNormal));
   }

   public fIndexingPolygon makeClone(){
      int i[];
      System.arraycopy(myIndices,0,i=new int[nbrIndices],0,nbrIndices);
      return new fShadedPolygon(i,nbrIndices,myColor.makeClone());
   }

	protected fPoint3d myNormal;
}
