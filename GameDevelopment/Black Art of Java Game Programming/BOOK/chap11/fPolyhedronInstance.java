import java.awt.*;
/**
 * Class that represents an instance of a polyhedron.
 */
public class fPolyhedronInstance extends Object {
   //-- the transformed vertices
	protected fArrayOf3dPoints transformedVertices;
	//-- the matrix used for transformations
	protected fMatrix3d myTransformMatrix;
	//-- the polyhedron
	protected fPolyhedron thePolyhedron;
	//-- position in WCS
	protected fPoint3d myPosition;
	//-- the angle in WCS
	protected fAngle3d myAngle;
	//-- 
	protected boolean positionIsDirty,angleIsDirty;

   /**
    * construct an instance of the supplied polyhedron.
    */
	public fPolyhedronInstance(fPolyhedron poly){
		//-- the polyhedron that this instance is using
		thePolyhedron=poly;
		//-- create the vertices to be used for storing transformations
		try{
			transformedVertices=(fArrayOf3dPoints)thePolyhedron.getVertices().makeClone();
		} catch(Exception e){e.printStackTrace();}

		myPosition=new fPoint3d();
		myAngle=new fAngle3d();
		myTransformMatrix=new fMatrix3d();
	}

   /**
    * set the position and angle for this polyhedron instance.
    */
	public void setOrientation(fPoint3d pos,fAngle3d agl){
		if(myPosition.equals(pos)==false){
		   //-- if position has changed then mark the matrix
		   //-- as "dirty" meaning that the transformed points
		   //-- need to be updated.
			myPosition.set(pos);
			positionIsDirty=true;
		}
		if(myAngle.equals(agl)==false){
			myAngle.set(agl);
			angleIsDirty=true;
		}
	}

	/**
	 * paint the polyhedron instance.
	 */
	public void paint(Graphics g,fGenericCamera camera){
		if(positionIsDirty || angleIsDirty){
		   //-- position or angle has changed and the transformed
		   //-- vertices need to be updated.
			myTransformMatrix.makeMCStoWCStransform(myPosition,myAngle);
			//-- transform the polyhedron model coordinates to world coords.
			myTransformMatrix.transform(thePolyhedron.getVertices(),transformedVertices);
			//-- 
			positionIsDirty=angleIsDirty=false;
		}
		//-- project the WCS to the screen with the supplied camera
		//-- and then call the paint method of the polyhedron with
		//-- the returned 2d array
		thePolyhedron.paint(g,camera.project(transformedVertices));
	}
}	
