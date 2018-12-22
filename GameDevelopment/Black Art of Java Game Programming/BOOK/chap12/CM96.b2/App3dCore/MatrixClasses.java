/**
 * A generic 3d matrix class that implements the rotation
 * about the principal axis, translation and scaling.
 */
class fGeneric3dMatrix extends Object {
   double xx, xy, xz, xo;
   double yx, yy, yz, yo;
   double zx, zy, zz, zo;

   /**
    * Constructs the identity matrix.
    */
   public fGeneric3dMatrix(){
      makeIdentity();
   }
   /**
    * Resets the matrix.
    */
   public void makeIdentity(){
      xx = 1; xy = 0; xz = 0; xo = 0;
      yx = 0; yy = 1; yz = 0; yo = 0; 
      zx = 0; zy = 0; zz = 1; zo = 0;
   }
   /**
    * "Smart" multiplies a rotation about Z-axis
    */
   public void concatRz(double az){
      double ct = Math.cos(az);
      double st = Math.sin(az);

      double Nyx = (yx * ct + xx * st);
      double Nyy = (yy * ct + xy * st);
      double Nyz = (yz * ct + xz * st);
      double Nyo = (yo * ct + xo * st);

      double Nxx = (xx * ct - yx * st);
      double Nxy = (xy * ct - yy * st);
      double Nxz = (xz * ct - yz * st);
      double Nxo = (xo * ct - yo * st);

      xx = Nxx; xy = Nxy; xz = Nxz; xo = Nxo;
      yx = Nyx; yy = Nyy; yz = Nyz; yo = Nyo;
   }
   /**
    * "Smart" multiplies a rotation about Y-axis
    */
   public void concatRy(double ay){
      double ct = Math.cos(ay);
      double st = Math.sin(ay);

      double Nxx = (xx * ct + zx * st);
      double Nxy = (xy * ct + zy * st);
      double Nxz = (xz * ct + zz * st);
      double Nxo = (xo * ct + zo * st);

      double Nzx = (zx * ct - xx * st);
      double Nzy = (zy * ct - xy * st);
      double Nzz = (zz * ct - xz * st);
      double Nzo = (zo * ct - xo * st);

      xx = Nxx; xy = Nxy; xz = Nxz; xo = Nxo;
      zx = Nzx; zy = Nzy; zz = Nzz; zo = Nzo;
   }
   /**
    * "Smart" multiplies a rotation about X-axis
    */
   public void concatRx(double ax){
      double ct = Math.cos(ax);
      double st = Math.sin(ax);

      double Nyx = (yx * ct + zx * st);
      double Nyy = (yy * ct + zy * st);
      double Nyz = (yz * ct + zz * st);
      double Nyo = (yo * ct + zo * st);

      double Nzx = (zx * ct - yx * st);
      double Nzy = (zy * ct - yy * st);
      double Nzz = (zz * ct - yz * st);
      double Nzo = (zo * ct - yo * st);

      yx = Nyx; yy = Nyy; yz = Nyz; yo = Nyo;
      zx = Nzx; zy = Nzy; zz = Nzz; zo = Nzo;
   }
   /**
    * "Smart" multiplies a translation
    */
   public void concatT(double x,double y,double z){
		xo+=x; yo+=y; zo+=z;
   }
   /**
    * "Smart" multiplies scaling
    */
   public void concatS(double sx,double sy,double sz){
      xx *= sx; xy *= sx; xz *= sx; xo *= sx;
      yx *= sy; yy *= sy; yz *= sy; yo *= sy;
      zx *= sz; zy *= sz; zz *= sz; zo *= sz;
   }
   /**
    * Multiplies the vector "ps" of 3d points and stores the result
    * in "pd".
    */
   public void transform(fArrayOf3dPoints ps,fArrayOf3dPoints pd){
     for (int i=0; i<ps.npoints; i++) {
       double x=ps.x[i]; double y=ps.y[i]; double z=ps.z[i];
       pd.x[i] = x*xx + y*xy + z*xz + xo;
       pd.y[i] = x*yx + y*yy + z*yz + yo;
       pd.z[i] = x*zx + y*zy + z*zz + zo;
     }
   }

   public void transformPoint(fPoint3d ps,fPoint3d pd){
       double x=ps.x; double y=ps.y; double z=ps.z;
         
       pd.x = x*xx + y*xy + z*xz + xo;
       pd.y = x*yx + y*yy + z*yz + yo;
       pd.z = x*zx + y*zy + z*zz + zo;
   }

   public void rotate(fArrayOf3dPoints ps,fArrayOf3dPoints pd){
     for (int i=0; i<ps.npoints; i++) {
       double x=ps.x[i]; double y=ps.y[i]; double z=ps.z[i];
       pd.x[i] = x*xx + y*xy + z*xz;
       pd.y[i] = x*yx + y*yy + z*yz;
       pd.z[i] = x*zx + y*zy + z*zz;
     }
   }
   
   //-- special transform
   void transformWithTranslation(fArrayOf3dPoints ps,fArrayOf3dPoints pd,fPoint3d et){
      double xt=et.x,yt=et.y,zt=et.z;
     for (int i=0; i<ps.npoints; i++) {
       double x=ps.x[i]+xt; double y=ps.y[i]+yt; double z=ps.z[i]+zt;
       pd.x[i] = x*xx + y*xy + z*xz + xo ;
       pd.y[i] = x*yx + y*yy + z*yz + yo;
       pd.z[i] = x*zx + y*zy + z*zz + zo;
     }
   }
}
/**
 * A 3d matrix that hides the making of the different 
 * transforms
 */
class fMatrix3d extends fGeneric3dMatrix {
   /**
    * construct the matrix
    */
   public fMatrix3d(){
      super();
   }
   /**
    * let matrix contain the MCS to WCS transform
    */
   public void makeMCStoWCStransform(fPoint3d pos,fAngle3d agl,fPoint3d scale){
      makeIdentity();
      concatS(scale.x,scale.y,scale.z);
      concatRx(agl.x);
      concatRy(agl.y);
      concatRz(agl.z);
      concatT(pos.x,pos.y,pos.z);
   }
   /**
    * let matrix contain the WCS to MCS transform
    */
   public void makeWCStoVCStransform(fPoint3d pos,fAngle3d agl){
      makeIdentity();
      concatT(-pos.x,-pos.y,-pos.z);
      concatRz(-agl.z);
      concatRy(-agl.y);
      concatRx(-agl.x);
   }

   public void makeLookAtPointTransform(fPoint3d p0,fPoint3d p1){
      fPoint3d vecZaxis=new fPoint3d(p0,p1);
      vecZaxis.normalize(1);
      fPoint3d vecXaxis=new fPoint3d();      
      vecXaxis.vectorProduct(new fPoint3d(0,1,0),p1);
      vecXaxis.normalize(1);
      
      fPoint3d vecYaxis=new fPoint3d();
      vecYaxis.vectorProduct(vecZaxis,vecXaxis);

      xo=-p0.x; yo=-p0.y; zo=-p0.z;
      xx=vecXaxis.x; xy=vecXaxis.y; xz=vecXaxis.z;
      yx=vecYaxis.x; yy=vecYaxis.y; yz=vecYaxis.z;
      zx=vecZaxis.x; zy=vecZaxis.y; zz=vecZaxis.z;
   }

}
