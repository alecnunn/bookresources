import java.io.*;

class fArrayOf2dPoints extends Object {
	int x[],y[];
	int npoints;

	fArrayOf2dPoints(int x0[],int y0[],int n){
		x=x0; y=y0; npoints=n;
	}
}

class fProjectedPoints extends fArrayOf2dPoints {
   double z[];
   int clipFlags[];
   int clipOrOp,clipAndOp;

   fProjectedPoints(int nbr){
      super(new int[nbr],new int[nbr],nbr);
      z=new double[nbr];
      clipFlags=new int[nbr];
   }
   
   fProjectedPoints(int x[],int y[],double z0[],int clipFlags0[],int n){
      super(x,y,n);
      z=z0;
      clipFlags=clipFlags0;
   }
}


/**
 * A class that encapulates and array of 3d points.
 */
class fArrayOf3dPoints extends Object {
   double x[],y[],z[];
   int npoints;
   /**
    * Constructs an array of 3d points with the supplied vectors.
    */
   fArrayOf3dPoints(double x0[],double y0[],double z0[],int n){
      x=x0; y=y0; z=z0; npoints=n;
   }
   /**
    * Constructs an empty array of 3d points with size "n"
    */
   fArrayOf3dPoints(int n){
      npoints=n;
      x=new double[n]; y=new double[n]; z=new double[n];
   }
   /**
    * construct an array of 3d points from a stream
    */
   fArrayOf3dPoints(InputStream is) throws IOException{
      fromString(is);
   }
   /**
    * ovrrides the Object method
    */
   public String toString(){
      String str=new String();
      //-- the number of vertices
      str=" "+npoints+"\n";
      //-- concat the coordinates to the string
      for(int n=0;n<npoints;n++){
         str=str+x[n]+" "+y[n]+" "+z[n]+"\n";
      }
      return str;
   }
   /**
    * Returns a clone.
    */
   fArrayOf3dPoints makeClone(){
      double xnew[],ynew[],znew[];
      System.arraycopy(x,0,xnew=new double[npoints],0,npoints);
      System.arraycopy(y,0,ynew=new double[npoints],0,npoints);
      System.arraycopy(z,0,znew=new double[npoints],0,npoints);
      return new fArrayOf3dPoints(xnew,ynew,znew,npoints);
   }
   /**
    * Reads an array from a stream
    */
   void fromString(InputStream is) throws IOException {
      //-- make a stream tokenizer
      StreamTokenizer stream = new StreamTokenizer (is);
		stream.commentChar('#');

      //-- get the # points
      stream.nextToken(); npoints=(int)stream.nval;

      //-- create the vectors
      x=new double[npoints];
      y=new double[npoints];
      z=new double[npoints];

      //-- read the coordiantes
      for(int n=0;n<npoints;n++){
          stream.nextToken(); x[n]=(double)stream.nval;
          stream.nextToken(); y[n]=(double)stream.nval;
          stream.nextToken(); z[n]=(double)stream.nval;
      }
  }
}

