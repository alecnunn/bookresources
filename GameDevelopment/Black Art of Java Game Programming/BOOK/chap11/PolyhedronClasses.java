import java.awt.*;
import java.io.*;
/**
 * A polyhedron class that is made out of a list of vertices
 * and a list of indexing polygons.
 */
abstract class fPolyhedron extends Object {
   //-- the 3d coordiantes for the model
	protected fArrayOf3dPoints myVertices;
	//-- the polygons
	protected fIndexingPolygon myPolygons[];
	protected int nbrPolygons;
   /**
    * construct a polyhedron with the supplied data.
    */
   protected fPolyhedron(fArrayOf3dPoints points,fIndexingPolygon polys[],int npolys){
		myVertices=points;
		myPolygons=polys;
		nbrPolygons=npolys;
	}
	/**
	 * construct a polyhedron from a stream.
	 */
	public fPolyhedron(InputStream is) throws IOException {
		fromString(is);
	}
	/**
	 * paint the polyhedron using the supplied 2d coordiantes.
	 */
	public abstract void paint(Graphics g,fArrayOf2dPoints point2d);
   /**
    * make a string representation of this polyhedron
    */
	public String toString(){
		String str=new String();
		//-- make the array of 3d points into a stream
		str=myVertices.toString();
		//-- write to stream how many polygons there are
		str=str+nbrPolygons+"\n";
		//-- write all polygons to the stream
		for(int n=0;n<nbrPolygons;n++){
			str=str+myPolygons[n].toString();
		}
		return str;
	}
	/**
	 * read the polyhedron from a stream
	 */
	public void fromString(InputStream is) throws IOException {
		//-- make a stream tokenizer
		StreamTokenizer stream = new StreamTokenizer (is);
		stream.commentChar('#');

		//-- get the points
		myVertices=new fArrayOf3dPoints(is);
		myVertices.toString();
		//-- get the # polygons
		stream.nextToken(); nbrPolygons=(int)stream.nval;
		//-- create the vector
		myPolygons=new fIndexingPolygon[nbrPolygons];
		//-- read each polygon
		for(int n=0;n<nbrPolygons;n++){
			myPolygons[n]=new fFilledPolygon(is);
		}
	}

	public fArrayOf3dPoints getVertices(){
		return myVertices;
	}

   public abstract fPolyhedron makeClone();

   public void scalePoints(double fx,double fy,double fz){
      for(int n=0;n<myVertices.npoints;n++){
         myVertices.x[n]*=fx; myVertices.y[n]*=fy; myVertices.z[n]*=fz;
      }
   }
}

class fConvexPolyhedron extends fPolyhedron {
   /**
    * construct a polyhedron with the supplied data.
    */
	public fConvexPolyhedron(fArrayOf3dPoints points,fIndexingPolygon polys[],int npolys){
	   super(points,polys,npolys);
	}
	/**
	 * construct a polyhedron from a stream.
	 */
	public fConvexPolyhedron(InputStream is) throws IOException {
	   super(is);
	}
   /**
    * overrides fPolyhedron.paint(..)
    * the polygons don't need to be sorted.
    */
	public void paint(Graphics g,fArrayOf2dPoints point2d){
	   //-- the polygons don't have to be sorted
		for(int n=0;n<nbrPolygons;n++){
			myPolygons[n].paint(g,point2d.x,point2d.y);
		}
	}
   /**
    * Makes a clone of this polyhedron.
    */
	public fPolyhedron makeClone(){
      fIndexingPolygon polys[];
      polys=new fIndexingPolygon[nbrPolygons];
      for(int n=0;n<nbrPolygons;n++){
         polys[n]=myPolygons[n].makeClone();
      }
      return new fConvexPolyhedron(myVertices.makeClone(),polys,nbrPolygons);
   }
}

