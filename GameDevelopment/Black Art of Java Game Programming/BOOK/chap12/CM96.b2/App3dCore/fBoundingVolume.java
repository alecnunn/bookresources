class fBoundingVolume{
   protected static fArrayOf3dPoints ourBox,ourNormals;
   
   protected double myRadius;
   protected fPoint3d myScale;
   protected fArrayOf3dPoints myBox,myNormals;
   protected fPolyhedronInstance theHostPolyInst;

   public fBoundingVolume(fPolyhedronInstance host,fPoint3d scale){
      //-- remove
      if(ourBox==null) initClass();
      //-- remove
      myScale=scale;
      myRadius=scale.magnitude();
      theHostPolyInst=host;
      myBox=new fArrayOf3dPoints(8);
      myNormals=new fArrayOf3dPoints(6);
   }
   
   public boolean checkForCollisionWith(fBoundingVolume vol){
  	   fPoint3d pp=theHostPolyInst.getPosition().makeClone();
	   pp.negate();
	   pp.plus(vol.theHostPolyInst.getPosition());
	   
	   //-- check if the bounding spheres collide
	   if(pp.magnitude()>(myRadius+vol.myRadius)){
	      //-- outside collision radius. no need for further checking
	      return false;
	   }

//      System.out.println("checking boxes");
      //-- update the world coordinates of the box
	   updateBox();
	   vol.updateBox();

      //-- rewrite --
      boolean a,b;
	   a=pointInMyVolume(vol.myBox);
	   b=vol.pointInMyVolume(myBox);

	   if( (a==true) || (b==true)){
         return true;
      }else{
         return false;
      }
   }
   public fBoundingVolume makeClone(){
      return new fBoundingVolume(theHostPolyInst,myScale);
   }

   private boolean pointInMyVolume(fArrayOf3dPoints otherBox){
	   //-- start checking if any point is withing my volume
      fPoint3d normal=new fPoint3d();
      fPoint3d point=new fPoint3d();
      fPoint3d vec=new fPoint3d();
      
	   for(int p=0;p<8;p++){
	      point.set(otherBox.x[p],otherBox.y[p],otherBox.z[p]);
	      boolean outside=false;
	      for(int n=0;(n<6) && (outside==false);n++){
	         //-- make the normal
	         normal.set(myNormals.x[n],myNormals.y[n],myNormals.z[n]);

	         //-- make the vector from my normal to other point
	         vec.set(myBox.x[n],myBox.y[n],myBox.z[n]);
	         vec.makeVectorTo(point);
	         
	         //-- check the dot product
	         if(normal.dotProduct(vec)>0){
	            //-- "infront" of one of the planes. no collision
//	            System.out.println("point "+p+"  is infront plane "+n);
	            outside=true;
	         } else {
//	            System.out.println("point "+p+"  is behind plane "+n);
	         }
	      }
	      if(outside==false){
	         //-- point behind all planes => inside volume
	         return true;
	      }
	   }
      return false;
   }
   
   public double getBoundingRadius(){
      return myRadius;
   }

   public static void initClass(){
      //-- the vertices of our box
      //--          0  1  2  3  4  5  6  7
      double px[]={ 1,-1,-1, 1,-1, 1, 1,-1};
      double py[]={ 1,-1, 1,-1, 1,-1, 1,-1};
      double pz[]={ 1,-1, 1,-1,-1, 1,-1, 1};
      ourBox=new fArrayOf3dPoints(px,py,pz,8);

      //-- the normals
      //--          0  1  2  3  4  5
      double nx[]={ 0, 0,-1, 1, 0, 0};
      double ny[]={ 1,-1, 0, 0, 0, 0};
      double nz[]={ 0, 0, 0, 0,-1, 1};
      ourNormals=new fArrayOf3dPoints(nx,ny,nz,6);
   }

   private void updateBox(){
      theHostPolyInst.transformPoints(ourBox,myBox);
      theHostPolyInst.rotateNormals(ourNormals,myNormals);
   }
}
