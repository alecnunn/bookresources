import java.awt.*;
import java.io.*;
/**
 * A polyhedron class that is made out of a list of vertices
 * and a list of indexing polygons.
 */
abstract class fPolyhedron extends Object {
   //-- the 3d coordiantes for the model
	protected fArrayOf3dPoints myVertices;
	protected fArrayOf3dPoints myPolygonNormals;
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
		makePolygonNormals();
	}
   /**
    * construct a polyhedron with the supplied data.
    */
   protected fPolyhedron(fArrayOf3dPoints points,fIndexingPolygon polys[],int npolys,fArrayOf3dPoints normals){
		myVertices=points;
		myPolygons=polys;
		nbrPolygons=npolys;
      myPolygonNormals=normals;
	}
   public int getNbrOfPolygons(){
      return nbrPolygons;
   }
   
   protected void makePolygonNormals(){
      myPolygonNormals=new fArrayOf3dPoints(nbrPolygons);
      for(int n=0;n<nbrPolygons;n++){
         fPoint3d norm=myPolygons[n].getNormal(myVertices);
         myPolygonNormals.x[n]=norm.x;
         myPolygonNormals.y[n]=norm.y;
         myPolygonNormals.z[n]=norm.z;
      }
   }
	/**
	 * construct a polyhedron from a stream.
	 */
	public fPolyhedron(InputStream is) throws IOException {
		fromString(is);
		makePolygonNormals();
	}
	/**
	 * paint the polyhedron using the supplied 2d coordiantes.
	 */
	public abstract void paint(Graphics g,fArrayOf2dPoints point2d);
   public abstract void clipAndPaint(Graphics g,fProjectedPoints p,fGenericCamera camera);
   public abstract void paintWithShading(Graphics g,fArrayOf2dPoints point2d,double intensities[]);
   public abstract void clipAndPaintWithShading(Graphics g,fProjectedPoints p,fGenericCamera camera,double intensities[]);
   
   public fArrayOf3dPoints getPolygonNormals(){
      return myPolygonNormals;
   }

  	public void calculateIntensities(fPoint3d light,double i[]){
  	   fPoint3d ptemp=new fPoint3d();
  	   for(int n=0;n<nbrPolygons;n++){
  	      ptemp.set(myPolygonNormals.x[n],myPolygonNormals.y[n],myPolygonNormals.z[n]);
  	      i[n]=ptemp.dotProduct(light);
  	   }
	}

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
			myPolygons[n]=new fShadedPolygon(is);
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
	public fConvexPolyhedron(fArrayOf3dPoints points,fIndexingPolygon polys[],int npolys,fArrayOf3dPoints polygonNormals){
	   super(points,polys,npolys,polygonNormals);
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
	
   public void clipAndPaint(Graphics g,fProjectedPoints p,fGenericCamera camera){
		for(int n=0;n<nbrPolygons;n++){
		   myPolygons[n].clipAndPaint(g,p,camera);
		}
   }
   public void paintWithShading(Graphics g,fArrayOf2dPoints point2d,double intensities[]){
		for(int n=0;n<nbrPolygons;n++){
			myPolygons[n].paintWithShading(g,point2d.x,point2d.y,intensities[n]);
		}
   }
   public void clipAndPaintWithShading(Graphics g,fProjectedPoints p,fGenericCamera camera,double intensities[]){
		for(int n=0;n<nbrPolygons;n++){
		   myPolygons[n].clipAndPaintWithShading(g,p,camera,intensities[n]);
		}
   }
   
	public fPolyhedron makeClone(){
      fIndexingPolygon polys[];
      polys=new fIndexingPolygon[nbrPolygons];
      for(int n=0;n<nbrPolygons;n++){
         polys[n]=myPolygons[n].makeClone();
      }
      return new fConvexPolyhedron(myVertices.makeClone(),polys,nbrPolygons,myPolygonNormals);
   }

}

