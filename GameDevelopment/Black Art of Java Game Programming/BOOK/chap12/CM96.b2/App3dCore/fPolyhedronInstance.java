import java.awt.*;
/**
 * Class that represents an instance of a polyhedron.
 */
class fPolyhedronInstance extends Object {
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
	protected boolean verticesAreDirty,matrixIsDirty,intensitiesAreDirty,normalsAreDirty;
   //--
	fBoundingVolume myBoundingVolume;
   fPoint3d myScale;
   //--
   protected fPoint3d myLastKnownLight;
   protected double myPolygonIntensities[];
   /**
    * construct an instance of the supplied polyhedron.
    */
	public fPolyhedronInstance(fPolyhedron poly,fPoint3d scale){
		//-- the polyhedron that this instance is using
		thePolyhedron=poly;
		//-- create the vertices to be used for storing transformations
   	transformedVertices=new fArrayOf3dPoints(thePolyhedron.getVertices().npoints);
      myPolygonIntensities=new double[thePolyhedron.getNbrOfPolygons()];

		myPosition=new fPoint3d();
		myAngle=new fAngle3d();
		myTransformMatrix=new fMatrix3d();

		myScale=scale;

		myBoundingVolume=new fBoundingVolume(this,myScale);

		myLastKnownLight=new fPoint3d();

	   matrixIsDirty=verticesAreDirty=normalsAreDirty=intensitiesAreDirty=true;
	}
   /**
    * set the position and angle for this polyhedron instance.
    */
	public void setOrientation(fPoint3d pos,fAngle3d agl){
		if((myPosition.equals(pos)==false)||(myAngle.equals(agl)==false)){
   	   matrixIsDirty=verticesAreDirty=normalsAreDirty=intensitiesAreDirty=true;
			myPosition.set(pos);
			myAngle.set(agl);
		}
	}
   public boolean checkForCollisionWith(fPolyhedronInstance polyinst){
      return myBoundingVolume.checkForCollisionWith(polyinst.myBoundingVolume);
   }
   
   public void clipAndPaint(Graphics g,fGenericCamera camera){
      updateVertices();
      fProjectedPoints theirBuffer=camera.projectWithCheck(transformedVertices);
      if(theirBuffer.clipAndOp!=0){
         //-- all vertices outside the view
         return;
      }
      if(theirBuffer.clipOrOp!=0){
         //-- some vertices outside
   		thePolyhedron.clipAndPaintWithShading(g,theirBuffer,camera,myPolygonIntensities);
//   		thePolyhedron.clipAndPaint(g,theirBuffer,camera);
      }else{
         thePolyhedron.paintWithShading(g,theirBuffer,myPolygonIntensities);
//         thePolyhedron.paint(g,theirBuffer);
      }
   }
      
	/**
	 * paint the polyhedron instance.
	 */
	public void paint(Graphics g,fGenericCamera camera){
	   updateVertices();
		thePolyhedron.paint(g,camera.project(transformedVertices));
	}

	public void paintWithShading(Graphics g,fGenericCamera camera,fPoint3d light){
	   updateVertices();
	   paint(g,camera);
	}

   public void clipAndPaintWithShading(Graphics g,fGenericCamera camera,fPoint3d light){
      updateIntensities(light);
      clipAndPaint(g,camera);
   }

   protected void updateIntensities(fPoint3d light){
	   if(intensitiesAreDirty || !myLastKnownLight.equals(light)){
	      //-- polyhedron has changed orientation or light has moved
	      //-- or both of the above => polygon intensities must be updated
	      myLastKnownLight.set(light);
         thePolyhedron.calculateIntensities(light,myPolygonIntensities);
         intensitiesAreDirty=false;
	   }
   }
   
	private void updateVertices(){
	   if(verticesAreDirty==true){
	      updateMatrix();
			myTransformMatrix.transform(thePolyhedron.getVertices(),transformedVertices);
			verticesAreDirty=false;
		}
	}

	private void updateMatrix(){
		if(matrixIsDirty==true){
		   //-- position or angle has changed and the transformed
		   //-- vertices need to be updated.
   		myTransformMatrix.makeMCStoWCStransform(myPosition,myAngle,myScale);
   		matrixIsDirty=false;
		}
	}

	public void transformPoint(fPoint3d ps,fPoint3d pd){
      updateMatrix();	   
   	//-- transform the polyhedron model coordinates to world coords.
		myTransformMatrix.transformPoint(ps,pd);
	}

	void transformPoints(fArrayOf3dPoints source,fArrayOf3dPoints dest){
	   updateMatrix();
   	myTransformMatrix.transform(source,dest);
	}

	void rotateNormals(fArrayOf3dPoints source,fArrayOf3dPoints dest){
	   updateMatrix();
	   myTransformMatrix.rotate(source,dest);
	}

	public void setScalingFactor(fPoint3d scale){
	   myScale.set(scale);
	   matrixIsDirty=verticesAreDirty=normalsAreDirty=intensitiesAreDirty=true;
	}

   public fPoint3d getScalingFactor() {
      return myScale.makeClone();
   }
   
	public double getBoundingRadius(){
	   return myBoundingVolume.getBoundingRadius();
	}

	public fPoint3d getPosition() {return myPosition;}

}


