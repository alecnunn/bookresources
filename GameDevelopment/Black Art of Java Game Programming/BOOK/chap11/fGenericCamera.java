/**
 * A generic camera.
 */
public class fGenericCamera extends Object {
   //-- a temporary buffer used for projection
	protected static fArrayOf2dPoints our2dBuffer=
	          new fArrayOf2dPoints(new int[100],new int[100],100);
   //-- a temporary buffer used for WCS to VCS transform
   protected static fArrayOf3dPoints our3dBuffer=new fArrayOf3dPoints(100);
	//-- the screen distance
	protected double screendist;
	//-- screen origo
	protected int x0,y0;
	//-- the viewangle
	protected double myViewAngle;
	//-- the matrix used for the WCS to VCS tranform
	fMatrix3d myWCStoVCSmatrix;
	//-- mark if the matrix is dirty
	boolean matrixIsDirty;
	//-- the position and angle of the camera in WCS
	fPoint3d myPosition;
	fAngle3d myAngle;

   /**
    * constructs a camera by specifing the widht, height and viewangle
    */
	public fGenericCamera(int width,int height,double viewAngle){
		myViewAngle=viewAngle;
		//-- calculate the screen origo
		x0=width>>1; y0=height>>1;
		//-- calculate the screen distance
		screendist=(double)x0/(Math.tan(viewAngle/2));
		//-- construct the matrix
		myWCStoVCSmatrix=new fMatrix3d();
		//--
		myPosition=new fPoint3d();
		myAngle=new fAngle3d();
		matrixIsDirty=true;
	}
	/**
	 * sets the position and angle of the camera.
	 */
	public void setOrientation(fPoint3d pos,fAngle3d agl){
		if(myPosition.equals(pos)==false){
			myPosition.set(pos);
			matrixIsDirty=true;
		}
		if(myAngle.equals(agl)==false){
			myAngle.set(agl);
			matrixIsDirty=true;
		}
	}
   /**
    * projects an array of 3d points to the temporary 2d buffer
    */
	public fArrayOf2dPoints project(fArrayOf3dPoints p3d){
	   //-- updates the matrix if it needed
	   updateMatrix();
	   //-- transform the WCS vertices to VCS storing the results
	   //-- in a buffer
  		myWCStoVCSmatrix.transform(p3d,our3dBuffer);
  		//-- project the VCS coordiantes to SCS storing the results
  		//-- in a buffer
		for(int n=0;n<p3d.npoints;n++){
		   double z=our3dBuffer.z[n];
			our2dBuffer.x[n]=-(int)(screendist*our3dBuffer.x[n]/z)+x0;
			our2dBuffer.y[n]= (int)(screendist*our3dBuffer.y[n]/z)+y0;
		}
		//-- lend the buffer to the caller.
		return our2dBuffer;
	}
	/**
	 * updates the matrix
	 */
	private void updateMatrix(){
	   if(matrixIsDirty==true){
	      //-- only remake the matrix if it is "dirty"
   	   myWCStoVCSmatrix.makeWCStoVCStransform(myPosition,myAngle);
   	   matrixIsDirty=false;
	   }
	}
}
