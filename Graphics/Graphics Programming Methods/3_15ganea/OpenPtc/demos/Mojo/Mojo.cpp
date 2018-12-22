//
// Mojo demo for OpenPTC 1.0 C++ API
// Coded by Alex Evans and adapted to PTC by Glenn Fiedler
//

// nasty code by alex "statix" evans for ptc. (c) copyright alex evans 1998
// time... 02.00 am on 13/1/98.
// have fun
// it's my take on some classic light mask effect
// it's raytracing through properly modelled fog with occlusion, multiple
// shadow rays cast back to the light for each pixel ray, and erm, its
// s l o w... but it looks nice don't it?

// oh and fresnel fall off... or something

// UNTESTED! ok?

// define inv for interesting fx (not)

#include "ptc.h"
#include <math.h>
#include <stdio.h>
#include <fstream.h>

#define SC 12
//#define INV

#ifndef PI
#define PI 3.14159265493
#endif

#define SQR(x) ((x)*(x))
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#define MINSEGSIZE 2.5f
#define NSEG 5

char8 *MaskMap;

class FVector
{
public:
    union {float X;float R;};
    union {float Y;float G;};
    union {float Z;float B;};

    FVector() {};
    FVector(float x, float y, float z) {X=x;Y=y;Z=z;};
    
    float Magnitude() const {return (float)sqrt(SQR(X)+SQR(Y)+SQR(Z));};
    float MagnitudeSq() const {return (float)(SQR(X)+SQR(Y)+SQR(Z));};
    void Normalise() {float l=1.f/Magnitude();X*=l;Y*=l;Z*=l;};
    float operator*(const FVector &a) const  {return X*a.X+Y*a.Y+Z*a.Z;};
    FVector operator*(const float a) const {return FVector(X*a,Y*a,Z*a);};
    FVector operator+(const FVector &a) const  {return FVector(X+a.X,Y+a.Y,Z+a.Z);};
    FVector operator-(const FVector &a) const {return FVector(X-a.X,Y-a.Y,Z-a.Z);};
    FVector operator^(const FVector &a) const {return FVector(Y*a.Z-Z*a.Y,Z*a.X-X*a.Z,X*a.Y-Y*a.X);};
    void operator*=(const float a) {X*=a;Y*=a;Z*=a;};
    void operator+=(const FVector &a) {X+=a.X;Y+=a.Y;Z+=a.Z;};
    void operator-=(const FVector &a) {X-=a.X;Y-=a.Y;Z-=a.Z;};

};

class FMatrix
{
public:
    FVector Row[3];
    FMatrix(FVector a,FVector b, FVector c) {Row[0]=a;Row[1]=b;Row[2]=c;};
    FMatrix() {};

    FVector Column0() const {return FVector(Row[0].X,Row[1].X,Row[2].X);};
    FVector Column1() const {return FVector(Row[0].Y,Row[1].Y,Row[2].Y);};
    FVector Column2() const {return FVector(Row[0].Z,Row[1].Z,Row[2].Z);};
#define MAKEROT(II,JJ,KK,II2,JJ2,KK2) \
        float c=(float)cos(theta);    \
        float s=(float)sin(theta);    \
        Row[II].II2=c;Row[II].JJ2=s;Row[II].KK2=0;\
        Row[JJ].II2=-s;Row[JJ].JJ2=c;Row[JJ].KK2=0;\
        Row[KK].II2=0;Row[KK].JJ2=0;Row[KK].KK2=1;
    void MakeXRot(float theta) {MAKEROT(1,2,0,Y,Z,X);};
    void MakeYRot(float theta) {MAKEROT(2,0,1,Z,X,Y);};
    void MakeZRot(float theta) {MAKEROT(0,1,2,X,Y,Z);};
    void MakeID() {Row[0]=FVector(1,0,0);Row[2]=FVector(0,0,1);Row[1]=FVector(0,1,0);};
    FMatrix Transpose() const {return FMatrix(Column0(),Column1(),Column2());};
    void TransposeInPlace() {FMatrix m=Transpose();*this=m;};
    void Normalise()
    {
        Row[2].Normalise();
        Row[0]=Row[1]^Row[2];
        Row[0].Normalise();
        Row[1]=Row[2]^Row[0];
        Row[1].Normalise();
    };
    FMatrix operator*(const float a) const {return FMatrix(Row[0]*a,Row[1]*a,Row[2]*a);};
    FMatrix operator*(const FMatrix &a) const {return FMatrix(
        FVector(Row[0].X*a.Row[0].X+Row[0].Y*a.Row[1].X+Row[0].Z*a.Row[2].X,
                Row[0].X*a.Row[0].Y+Row[0].Y*a.Row[1].Y+Row[0].Z*a.Row[2].Y,
                Row[0].X*a.Row[0].Z+Row[0].Y*a.Row[1].Z+Row[0].Z*a.Row[2].Z),

        FVector(Row[1].X*a.Row[0].X+Row[1].Y*a.Row[1].X+Row[1].Z*a.Row[2].X,
                Row[1].X*a.Row[0].Y+Row[1].Y*a.Row[1].Y+Row[1].Z*a.Row[2].Y,
                Row[1].X*a.Row[0].Z+Row[1].Y*a.Row[1].Z+Row[1].Z*a.Row[2].Z),

        FVector(Row[2].X*a.Row[0].X+Row[2].Y*a.Row[1].X+Row[2].Z*a.Row[2].X,
                Row[2].X*a.Row[0].Y+Row[2].Y*a.Row[1].Y+Row[2].Z*a.Row[2].Y,
                Row[2].X*a.Row[0].Z+Row[2].Y*a.Row[1].Z+Row[2].Z*a.Row[2].Z));};

    FVector operator*(const FVector &a) const {return FVector(a*Row[0],a*Row[1],a*Row[2]);};
    FMatrix operator+(const FMatrix &a) const {return FMatrix(Row[0]+a.Row[0],Row[1]+a.Row[1],Row[2]+a.Row[2]);};
    FMatrix operator-(const FMatrix &a) const {return FMatrix(Row[0]+a.Row[0],Row[1]+a.Row[1],Row[2]+a.Row[2]);};

};

class Ray
{
public:
    FVector mPosn;
    FVector mDir;
    Ray(const FVector &p,const FVector &d) {mPosn=p;mDir=d;mDir.Normalise();};
};

class VLight
{
public:
    float mAng;
    FVector mPosn;
    FVector mTarget;
    FMatrix mAxis;
    FVector mCol;

    FVector p,p2,d;    // temp space

    VLight(const FVector &col);
    void Move(const FVector &p) {mPosn=p;Update();};
    void MoveT(const FVector &p) {mTarget=p;Update();};
    void Update() {mAxis.Row[2]=(mTarget-mPosn);mAxis.Normalise();};
    FVector Light(const Ray *ray);
    float CalcLight(float t);

};



VLight::VLight(const FVector &col)
{
    mCol=col*0.9f;
    mAng=2.8f;
    mPosn=FVector(0.0f,0.0f,20.0f);
    mTarget=FVector(0.0f,0.0f,0.1f);
    mAxis.MakeID();
    Update();
}

float VLight::CalcLight(float t)
{
    // trace line to bitmap from mPosn to p2
    if (!((mPosn.Z>0)^(p2.Z>0)))
    {
 // fresnel fall off...
 return p.Z/p.MagnitudeSq();
    }
    float f=-(mPosn.Z)/(p2.Z-mPosn.Z);
    int x=160+int(SC*((p2.X-mPosn.X)*f+mPosn.X));
#ifndef INV
    if (x<0 || x>319) return p.Z/p.MagnitudeSq();;
    int y=100+int(SC*((p2.Y-mPosn.Y)*f+mPosn.Y));
    if (y<0 || y>199) return p.Z/p.MagnitudeSq();;
    int c=MaskMap[y*320+x];
#else
    if (x<0 || x>319) return 0;
    int y=100+int(SC*((p2.Y-mPosn.Y)*f+mPosn.Y));
    if (y<0 || y>199) return 0;
    int c=255-MaskMap[y*320+x];
#endif
    if (c==0) return 0;
    return (c*(1/255.0f))*p.Z/p.MagnitudeSq();

}



FVector VLight::Light(const Ray *ray)
{
    float f=0;

    p2=ray->mPosn;
    p=mAxis*(ray->mPosn-mPosn);
    d=mAxis*ray->mDir;
    float A=(d.X*d.X+d.Y*d.Y);
    float B=2.f*(d.X*p.X+d.Y*p.Y)-mAng*(d.Z);
    float C=(p.X*p.X+p.Y*p.Y)-mAng*(p.Z);
    float D=B*B-4*A*C;
    if (D<=0) return FVector(0,0,0);
    D=float(sqrt(D));
    A*=2;
    float t1=(-B-D)/A;
    float t2=(-B+D)/A;
    int frc=255;
    float t3=-ray->mPosn.Z/ray->mDir.Z;
    if (t2<=0) return FVector(0,0,0);
    if (t1<0) t1=0;
    if (t3>0)
    {
 // clip to bitmap plane
 FVector pp=ray->mPosn+ray->mDir*t3;
 int x=160+int(SC*pp.X);
#ifndef INV
 if (x>=0 && x<=319)
 {
     int y=100+int(SC*pp.Y);
     if (y>=0 && y<=199)
     {
  //return FVector(0,0,1);
  frc=MaskMap[y*320+x];
  if (frc<1)
  {
      if (t1>t3) t1=t3;
      if (t2>t3) t2=t3;
  }
     }
     else t3=t2;
 } else t3=t2;
#else
 if (x>=0 && x<=319)
 {
     int y=100+int(SC*pp.Y);
     if (y>=0 && y<=199 && MaskMap[y*320+x]<128)
     {
  t3=t2;
     }
 }
 if (t1>t3) t1=t3;
 if (t2>t3) t2=t3;
#endif
    }
    if (t1>=t2) return FVector(0,0,0);
    float fr=frc/255.0f;
    float l1=CalcLight(t1);if (t1>t3) l1*=fr;
    int q=NSEG;
    float t=t1;
    float h=(t2-t1)/NSEG;
    if (h<MINSEGSIZE) h=MINSEGSIZE;
    while (t<t3 && q>0 && t<t2)
    {
 t+=h;
 if (t>t2) {h-=t2-t;t=t2;q=0;} else q--;
 float h=(t-t1);
 p+=d*h;
 p2+=ray->mDir*h;
 float l2=CalcLight(t);
 f+=(l1+l2)*h;
 l1=l2;
 t1=t;
    }
    while (q>0 && t<t2)
    {
 t+=h;
 if (t>t2) {h-=t2-t;t=t2;q=0;} else q--;
 p+=d*h;
 p2+=ray->mDir*h;
 float l2=CalcLight(t);if (t>t3) l2*=fr;
 f+=(l1+l2)*h;
 l1=l2;
 t1=t;
    }
    return mCol*f;
}

inline int CLIPC(float f)
{
    int a=int(f*255.0);
    return (a<0)?0:(a>255)?255:a;
}

short16 frandtab[65536];

void initfrand()
{

    memset(frandtab,0,sizeof(frandtab));
    int s=1;
    for (int c1=1;c1<65536;c1++)
    {
 frandtab[c1]=s;
 s=(((s>>4)^(s>>13)^(s>>15))&1)+(s<<1);
    }
}

int inline frand()
{
    static short16 seed=54;
    return frandtab[seed++];
}


void VLightPart(Console &console, Surface &surface)
{
    initfrand();
    VLight *vl=new VLight(FVector(0.1f,0.4f,1.0f));
    VLight *vl2=new VLight(FVector(1.0f,0.5f,0.2f));
    vl->Move(FVector(0,0,20));
    vl2->Move(FVector(0,6,30));

    FVector camposn(7,0.5,-10);
    FMatrix camaxis;
    camaxis.MakeID();
    camaxis.Row[2]=FVector(0,0,0)-camposn;camaxis.Normalise();
    float camf=100.f;

    MaskMap=new char8[320*200];
    memset(MaskMap,0,320*200);

 /////////////////
 // load Mojo.raw

    ifstream file("Mojo.raw",ios::binary);
    file.read((char*)MaskMap,320*200);
    file.close();

    // build the order of the squares
    int c1=0;
    int order[10*19][2];
    for (c1=0;c1<10*19;c1++)
    {
 order[c1][0]=(c1%19);
 order[c1][1]=(c1/19)+1;
    }
    // swap them around
    for (c1=0;c1<10000;c1++)
    {
 int t;
 int c2=rand()%190;
 int c3=(rand()+1+c2)%190;
 t=order[c2][0];order[c2][0]=order[c3][0];order[c3][0]=t;
 t=order[c2][1];order[c2][1]=order[c3][1];order[c3][1]=t;
    }

    // time settings
    float time  = 0;
    float delta = 0.01f;            // this controls the speed of the effect ...

 ///////////////////
 // main loop

    while (!console.key())
    {

    // get surface data
    int width  = surface.width();
    int height = surface.height();
    int pitch  = surface.pitch();

    // light time (makes the effect loop)
    float vlightt = 320 * (float) fabs((float)sin(time/5));

 float t=13.0f-0.1822f*vlightt;
 float cz=1.0f-0.01f*vlightt;
 //vl->Move(FVector(sin(t)*5,cos(t*-0.675+4543)*5,15));
 //vl->MoveT(FVector(0,0,-15));
 vl->Move(FVector(t,0,22));
 vl2->Move(FVector(-t,-7,28));

 camposn=FVector(cz*4.0f+9.0f,cz,-t/7-13);
 camaxis.Row[2]=FVector(0,0,0)-camposn;camaxis.Normalise();


 char8 idx[200/16][320/16];
 memset(idx,25,sizeof(idx));

 int32 oc,c;
 // swap them around
 for (c1=0;c1<100;c1++)
 {
     int t;
     int c2=rand()%190;
     int c3=(rand()+1+c2)%190;
     t=order[c2][0];order[c2][0]=order[c3][0];order[c3][0]=t;
     t=order[c2][1];order[c2][1]=order[c3][1];order[c3][1]=t;
 }
 for (int zz=0;zz<190;zz++)
 {
     int xx=order[zz][0];
     int yy=order[zz][1];
     int i=0;

        // lock surface
        char8 *screenbuf=(char8*)surface.lock();

     int c2=idx[yy][xx]>>1;
     for (c1=0;c1<c2;c1++)
     {
  int a=frand()&255;
  int x=xx*16+(a&15)+6+4;
  int y=yy*16+(a>>4)+6;

  FVector col(0,0,0);
  Ray ray(camposn,camaxis.Row[2]*camf+camaxis.Row[0]*float(x-160)+camaxis.Row[1]*float(y-100));
  col+=vl->Light(&ray);
  col+=vl2->Light(&ray);

        c=(CLIPC(col.R)<<16)+(CLIPC(col.G)<<8)+(CLIPC(col.B));
        char8 *d=screenbuf+x*4+y*pitch;
  i+=abs((c&255)-(d[321]&255))+abs((c>>16)-(d[321]>>16));
  if (c1) i+=abs((c&255)-(oc&255))+abs((c>>16)-(oc>>16));
  oc=c;

        #define DARKEN(x) ((x>>1)&0x7f7f7f)

  int32 c2=DARKEN(c);
  ((int32*)d)[1]=DARKEN(((int32*)d)[1])+c2;
  ((int32*)d)[2]=DARKEN(((int32*)d)[2])+c2;
  d+=pitch;
  ((int32*)d)[0]=DARKEN(((int32*)d)[0])+c2;
  ((int32*)d)[1]=c;
  ((int32*)d)[2]=c;
  ((int32*)d)[3]=DARKEN(((int32*)d)[3])+c2;         
  d+=pitch;
  ((int32*)d)[0]=DARKEN(((int32*)d)[0])+c2;
  ((int32*)d)[1]=c;
  ((int32*)d)[2]=c;
  ((int32*)d)[3]=DARKEN(((int32*)d)[3])+c2;         
  d+=pitch;
  ((int32*)d)[1]=DARKEN(((int32*)d)[1])+c2;
  ((int32*)d)[2]=DARKEN(((int32*)d)[2])+c2;

     }
     i*=5;
     i/=3*idx[yy][xx];
     if (i<2) i=2;
     if (i>256) i=256;
     idx[yy][xx]=i;

        // unlock surface
        surface.unlock();

     if ((zz%95)==0)
  {
   // copy surface to console
   surface.copy(console);

            // update console
            console.update();
  }
 }

    // update time
    time += delta;
 }

    delete [] MaskMap;
    delete vl;
    delete vl2;
}


int APIENTRY WinMain(HINSTANCE hInst,HINSTANCE hPrevInst,LPSTR lpCmdLine,int nCmdShow)
{
    try
    {
        // create format
        Format format(32,0x00FF0000,0x0000FF00,0x000000FF);

        // create console
        Console console;

        // open console
        console.open("mojo by statix",320,200,format);

        // create main drawing surface
        Surface surface(320,200,format);

        // do the light effect
        VLightPart(console,surface);

        // close console 
        console.close();

        // print message to stdout
        printf("mojo by alex \"statix\" evans\nto be used as an example of bad coding and good ptc\nno responsibility taken for this!\nenjoy ptc! it's great\n\n-statix 13/1/98");
    }
    catch (Error &error)
    {
        // report error
        error.report();
    }

    // exit
    return 0;
}
