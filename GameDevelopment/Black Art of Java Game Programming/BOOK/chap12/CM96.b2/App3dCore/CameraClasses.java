import java.awt.*;
import java.util.*;
/**
 * A generic camera.
 */
class fGenericCamera extends Object {
   //-- a temporary buffer used for projection
	protected static fProjectedPoints our2dBuffer=new fProjectedPoints(100);
   
   //-- a temporary buffer used for WCS to VCS transform
   protected static fArrayOf3dPoints our3dBuffer=new fArrayOf3dPoints(100);
   
	//-- the screen distance
	protected double screendist;
	
	//-- screen origo, width and height
	protected int x0,y0;
	protected int myWidth,myHeight;
	
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
	public fGenericCamera(double viewAngle){
	   myWidth=100;
	   myHeight=100;
		myViewAngle=viewAngle;
      calculateParameters();
		//-- construct the matrix
		myWCStoVCSmatrix=new fMatrix3d();
		//--
		myPosition=new fPoint3d();
		myAngle=new fAngle3d();
		matrixIsDirty=true;
	}
   public void setScreenSize(int w,int h){
      myWidth=w; myHeight=h;
      calculateParameters();
   }

   private void calculateParameters(){
	   //-- calculate the screen origo
      x0=myWidth>>1; y0=myHeight>>1;
      //-- calculate the screen distance
      screendist=-(double)x0/(Math.tan(myViewAngle/2));
      Zclip=-3;
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
   public void update(double dt){
   }
   /**
    * projects an array of 3d points to the temporary 2d buffer
    */
	public fArrayOf2dPoints project(fArrayOf3dPoints p3d){
	   doTransform(p3d);
  		doProjection();
		return our2dBuffer;
	}

   protected void doTransform(fArrayOf3dPoints p3d){
	   updateMatrix();
  		myWCStoVCSmatrix.transform(p3d,our3dBuffer);
  		our3dBuffer.npoints=p3d.npoints;
   }
   
	protected void doProjection(){
  		//-- project the VCS coordiantes to SCS storing the results
  		//-- in a buffer
		for(int n=0;n<our3dBuffer.npoints;n++){
		   double z=our3dBuffer.z[n];
			our2dBuffer.x[n]= (int)(screendist*our3dBuffer.x[n]/z)+x0;
			our2dBuffer.y[n]=-(int)(screendist*our3dBuffer.y[n]/z)+y0;
		}
		our2dBuffer.npoints=our3dBuffer.npoints;
		//-- lend the buffer to the caller.
	}

	protected void doTheChecks(){
	   //-- inititate the AND and OR operation
	   our2dBuffer.clipAndOp=Integer.MAX_VALUE;
	   our2dBuffer.clipOrOp=0;
	   for(int n=0;n<our2dBuffer.npoints;n++){
         our2dBuffer.clipFlags[n]=0;
	      if(our2dBuffer.x[n]>myWidth){
	         our2dBuffer.clipFlags[n]|=CLIP_RIGHT;
	      } else if(our2dBuffer.x[n]<0){
	         our2dBuffer.clipFlags[n]|=CLIP_LEFT;
	      }
	      if(our2dBuffer.y[n]>myHeight){
	         our2dBuffer.clipFlags[n]|=CLIP_BOTTOM;
	      } else if(our2dBuffer.y[n]<0){
	         our2dBuffer.clipFlags[n]|=CLIP_TOP;
	      }
	      
	      our2dBuffer.z[n]=our3dBuffer.z[n];
	      if(our2dBuffer.z[n]>Zclip){
	         our2dBuffer.clipFlags[n]|=CLIP_Z;
	      }
         our2dBuffer.clipOrOp  |= our2dBuffer.clipFlags[n];	      
         our2dBuffer.clipAndOp &= our2dBuffer.clipFlags[n];	      
	   }
	}
	/**
	 * Transforms and projects the points to the screen. It also
	 * stores the z coordinate and clipping info in case the polygons
	 * need clipping.
	 */
	public fProjectedPoints projectWithCheck(fArrayOf3dPoints p3d){
	   doTransform(p3d);
	   doProjection();
	   doTheChecks();
	   return our2dBuffer;
	}
	/**
	 * updates the matrix
	 */
	protected void updateMatrix(){
	   if(matrixIsDirty==true){
	      //-- only remake the matrix if it is "dirty"
   	   myWCStoVCSmatrix.makeWCStoVCStransform(myPosition,myAngle);
   	   matrixIsDirty=false;
	   }
	}

   public void clipZandStore(int xs1,int ys1,double zs1,int xs2,int ys2,double zs2,int xstore[],int ystore[],int p){
		double zscaled,t;
		//-- transform points from SCS to VCS
		zscaled=zs1/screendist;
		double xv1=zscaled*(xs1-x0);
		double yv1=zscaled*(ys1-y0);

		zscaled=zs2/screendist;
		double xv2=zscaled*(xs2-x0);
		double yv2=zscaled*(ys2-y0);

		t=(Zclip-zs1)/(zs2-zs1);
		xv1=xv1+t*(xv2-xv1);
		yv1=yv1+t*(yv2-yv1);
		//-- project the points		
		xstore[p]=(int)(screendist*xv1/Zclip)+x0;
		ystore[p]=(int)(screendist*yv1/Zclip)+y0;
	}

   public double Zclip;
	public static int CLIP_TOP=1,CLIP_BOTTOM=2,CLIP_RIGHT=4,CLIP_LEFT=8,CLIP_Z=16;
} 

class fCamera extends fGenericCamera {
   fPoint3d light;
   
  	public fCamera(fWorld w,double viewAngle,double viewdistance,fPoint3d pos,fAngle3d agl){
  	   super(viewAngle);
  	   theWorld=w;
  	   myViewDistance=viewdistance;
  	   theVisibleObjects=new Vector(100,100);
  	   theDistToObjects=new double[200];
      setOrientation(pos,agl);
      //-- faking light
      light=new fPoint3d(-1,0,0);
      light.rotateAboutXaxis(Math.PI/5);
      light.rotateAboutYaxis(Math.PI/3);
      light.normalize(1);
  	}

   public void paint(Graphics g){
      theVisibleObjects.removeAllElements();
      theWorld.getAllObjectsInRadius(myPosition,myViewDistance,theVisibleObjects);
      
      removeUnvisibleObjects(theVisibleObjects);

      sortObjects(theVisibleObjects);
      //--
      int nbr=theVisibleObjects.size();
      for(int n=0;n<nbr;n++){
         ((fObject)(theVisibleObjects.elementAt(n))).paintWithShading(g,this,light);
      }
   }
   /**
    * Sets the ambient light.
    */
   public void setAmbientLight(fPoint3d vect){
      light=vect.makeClone();
   }

   public void update(double dt){
   }

   private void sortObjects(Vector list){
      int nbr=list.size();
      for(int n=0;n<nbr;n++){
         fObject obj=(fObject)list.elementAt(n);
         theDistToObjects[n]=obj.distanceToPoint(myPosition);
      }

      for(int i=0;i<nbr-1;i++){
         double maxZ=-10000;
         Object maxZobj=null;
         int maxIndex=0;
         for(int j=i;j<nbr;j++){
            if(theDistToObjects[j]>maxZ){
               maxZ=theDistToObjects[j];
               maxZobj=list.elementAt(j);
               maxIndex=j;
            }
         }
         theDistToObjects[maxIndex]=theDistToObjects[i];
         theDistToObjects[i]=maxZ;
         list.setElementAt(list.elementAt(i),maxIndex);
         list.setElementAt(maxZobj,i);
      }
   }
   
   private void removeUnvisibleObjects(Vector vec){
      int nbr=vec.size();
      for(int n=0;n<nbr;n++){
         fPolyhedronInstance inst=((fObject)vec.elementAt(n)).getPolyhedronInstance();
         if(sphereIsVisible(inst.getPosition(),inst.getBoundingRadius())==false){
            vec.removeElementAt(n);
            //-- decrease "n" and "nbr" since the array will shrink
            n--; nbr--;
         }
      }
   }

   protected boolean sphereIsVisible(fPoint3d pos,double radius){
      fPoint3d p=new fPoint3d();
	   updateMatrix();
      myWCStoVCSmatrix.transformPoint(pos,p);
      if((p.z-radius)>Zclip){
         return false;
      }else{
         return true;
      }
   }
   
   protected fWorld theWorld;
  	protected double myViewDistance;
  	protected Vector theVisibleObjects;
  	protected double theDistToObjects[];
}

class fMagicCamera extends fCamera {
   fMagicCamera(fWorld w,double viewAngle,double viewdistance,fPoint3d pos,fAngle3d agl,double gridSize0,double fadingFactor0){
      super(w,viewAngle,viewdistance,pos,agl);
      gridSize=gridSize0;
      fadingFactor=fadingFactor0;

      //-- make the default ground
      int pts=(int)(2*viewdistance/gridSize)+1;
      nbrPointsInGround=pts*pts;
      Gwcs=new fArrayOf3dPoints(nbrPointsInGround);

      int n=0;
      for(double x=-myViewDistance;x<=myViewDistance; x+=gridSize){
         for(double z=-myViewDistance;z<=myViewDistance; z+=gridSize){
            Gwcs.x[n]=x; Gwcs.y[n]=0; Gwcs.z[n]=z;
            n++;
         }
      }

      if(our3dBuffer.npoints<nbrPointsInGround){
         our3dBuffer=new fArrayOf3dPoints(nbrPointsInGround);
         our2dBuffer=new fProjectedPoints(nbrPointsInGround);
      }
         
      mySkyColor=new Color(50,50,200);
      myGroundColor=new Color(50,130,50);
   }

   public void paint(Graphics g){
      fakeHorizon(g);
      fakeGround(g);
      super.paint(g);
   }

   public void setSkyColor(Color col){
      mySkyColor=col;
   }

   public void setGroundColor(Color col){
      myGroundColor=col;
   }

   protected void fakeHorizon(Graphics g){
      fPoint3d p=new fPoint3d(0,myPosition.y,-myViewDistance*5);
      p.rotateAboutXaxis(-myAngle.x);
      int scry=(int)(screendist*p.y/p.z)+y0;

      if(scry<0){
         if(p.z<0){
            g.setColor(myGroundColor);
         }else{
            g.setColor(mySkyColor);
         }
         g.fillRect(0,0,x0<<1,y0<<1);
      } else if(scry>2*y0){
         if(p.z>0){
            g.setColor(myGroundColor);
         }else{
            g.setColor(mySkyColor);
         }
         g.fillRect(0,0,x0<<1,y0<<1);
      } else {
         if(p.z<0){
            g.setColor(mySkyColor);
            g.fillRect(0,0,x0<<1,scry);
            g.setColor(myGroundColor);
            g.fillRect(0,scry,x0<<1,y0<<1);
         }else{
            g.setColor(myGroundColor);
            g.fillRect(0,0,x0<<1,scry);
            g.setColor(mySkyColor);
            g.fillRect(0,scry,x0<<1,y0<<1);
         }
      }
   }
   
   protected void fakeGround(Graphics g){
      int Xcen=(int)(myPosition.x/gridSize);
      int Zcen=(int)(myPosition.z/gridSize);

	   updateMatrix();
 		myWCStoVCSmatrix.transformWithTranslation(Gwcs,our3dBuffer,new fPoint3d(Xcen*gridSize,0,Zcen*gridSize));
  		our3dBuffer.npoints=Gwcs.npoints;
  		
	   doProjection();
	   doTheChecks();
      
      int nn=0;
      g.setColor(new Color(0,0,0));
      for(int i=0;i<our2dBuffer.npoints;i++){
         if(our2dBuffer.clipFlags[i]==0){
            g.drawLine(our2dBuffer.x[i],our2dBuffer.y[i],our2dBuffer.x[i],our2dBuffer.y[i]);
            nn++;
         }
      }
   }

   fArrayOf3dPoints Gwcs;
   protected double gridSize,fadingFactor;
   protected int nbrPointsInGround;
   protected Color mySkyColor,myGroundColor;
}
