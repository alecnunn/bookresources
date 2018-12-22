import java.awt.*;
import java.io.*;
/**
 * Describes an abstract indexing polygon.
 */
abstract class fIndexingPolygon extends Object{
   /**
    * construct a polygon with the supplied indices
    */
	protected fIndexingPolygon(int indices[],int n){
	   myIndices=indices;
		nbrIndices=n;
	}
   /**
    * construct a polygon from a stream
    */
	protected fIndexingPolygon(InputStream is) throws IOException {
		fromString(is);
	}
   /**
    * paints a polygon. the 2d list of coordiantes must be supplied
    */
	public abstract void paint(Graphics g,int x[],int y[]);
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
	/**
   * make a clone of this polygon
     */
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
	 * paints the polygon if it is cw
	 */
	public void paint(Graphics g,int x[],int y[]){
	   //-- copy the indexed coordiantes from the 2d list to
	   //-- the scratch-pad
      copyIndexedPoints(x,y);
      render(g);
	}
	/**
    * The actual rendering.
    */
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
	/**
   * Makes a clone of this polygon.
 */
   public fIndexingPolygon makeClone(){
      int i[];
      System.arraycopy(myIndices,0,i=new int[nbrIndices],0,nbrIndices);
      return new fFilledPolygon(i,nbrIndices,myColor.makeClone());
   }
}

