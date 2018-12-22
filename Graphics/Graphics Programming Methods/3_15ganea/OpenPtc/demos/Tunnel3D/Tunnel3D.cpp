//
// Tunnel3D demo for OpenPTC 1.0 C++ API
//
// Realtime raytraced tunnel
// Copyright (c) 1998 Christian Nentwich (brn@eleet.mcb.at)
// This source code is licensed under the GNU LGPL
//
// And do not just blatantly cut&paste this into your demo :)
//

#include "ptc.h"
#include <math.h>
#include <string.h>
#include <iostream.h>
#include <fstream.h>
#include "Tunnel3D.h"


/* VECTOR ROUTINES */

void vector_normalize(Vector v)
{ float length;

  length=v[0]*v[0]+v[1]*v[1]+v[2]*v[2];
  length=(float)sqrt(length);

  if(length!=0)
  { v[0]/=length;
    v[1]/=length;
    v[2]/=length;
  }
  else
  { v[0]=v[1]=v[2]=0;
  }
}

void vector_times_matrix(Vector v,Matrix m,Vector res)
{ int i,j;

  for(j=0;j<3;j++)
  { res[j]=0;

    for(i=0;i<3;i++)
    res[j]+=m[j][i]*v[i];
  }
}

void matrix_idle(Matrix m)
{ memset(m,0,sizeof(Matrix));

  m[0][0]=1;
  m[1][1]=1;
  m[2][2]=1;
  m[3][3]=1;
}

void matrix_rotate_x(Matrix m,int angle,float *sintab,float *costab)
{ Matrix tmp,tmp2;

  matrix_idle(tmp);

  tmp[1][1]=costab[angle];
  tmp[2][1]=sintab[angle];
  tmp[1][2]=-sintab[angle];
  tmp[2][2]=costab[angle];

  matrix_times_matrix(tmp,m,tmp2);
  memcpy(m,tmp2,sizeof(Matrix));

}

void matrix_rotate_y(Matrix m,int angle,float *sintab,float *costab)
{ Matrix tmp,tmp2;

  matrix_idle(tmp);

  tmp[0][0]=costab[angle];
  tmp[2][0]=-sintab[angle];
  tmp[0][2]=sintab[angle];
  tmp[2][2]=costab[angle];

  matrix_times_matrix(tmp,m,tmp2);
  memcpy(m,tmp2,sizeof(Matrix));

}

void matrix_rotate_z(Matrix m,int angle,float *sintab,float *costab)
{ Matrix tmp,tmp2;

  matrix_idle(tmp);

  tmp[0][0]=costab[angle];
  tmp[1][0]=sintab[angle];
  tmp[0][1]=-sintab[angle];
  tmp[1][1]=costab[angle];

  matrix_times_matrix(tmp,m,tmp2);
  memcpy(m,tmp2,sizeof(Matrix));

}

void matrix_times_matrix(Matrix m1,Matrix m2,Matrix res)
{ int i,j,k;

  for(j=0;j<4;j++)
  for(i=0;i<4;i++)
  { res[i][j]=0;

    for(k=0;k<4;k++)
    res[i][j]+=m1[k][j]*m2[i][k];
  }
}

/* TUNNEL ROUTINES */

RayTunnel::RayTunnel(float rad)
{ float x,y;

  radius=rad;
  radius_sqr=rad*rad;

  sintab=new float[1024];         // Set trigonometry and lookups
  costab=new float[1024];
  u_array=new int[64*26];
  v_array=new int[64*26];
  l_array=new int[64*26];
  norms=new float[64*26][3];

  lookup=new int32[65*256];
  pal=new char8[768];

  for(int i=0;i<1024;i++)
  { sintab[i]=(float)sin(i*M_PI/512);
    costab[i]=(float)cos(i*M_PI/512);
  }

  // Generate normal vectors

  y=-100.0;
  Vector tmp;

  for(int j=0;j<26;j++)
  { x=-160.0;

    for(int i=0;i<41;i++)
    { tmp[0]=x; tmp[1]=y; tmp[2]=128;
      vector_normalize(tmp);

      memcpy(norms[j*64+i],tmp,sizeof(Vector));
      x+=8.0;
    }
    y+=8.0;
  }

  // Reset tunnel and light position and all angles                          
  pos[0]=0.0; pos[1]=0.0; pos[2]=0.0;
  light[0]=1.0; light[1]=1.0; light[2]=0.0;

  xa=ya=za=0;

  lightstatus=0;

  // Normalize light vector to length 1.0
  vector_normalize(light);
}


RayTunnel::~RayTunnel()
{ delete [] pal;
  delete [] lookup;
  delete [] norms;
  delete [] l_array;
  delete [] v_array;
  delete [] u_array;
  delete [] costab;
  delete [] sintab;
}
 

void RayTunnel::load_texture()
{
  // Allocate tunnel texture 33 bytes too big

  tunneltex=new char8[65536+33];
  char8 *tmp=new char8[65536];
  if(tunneltex==NULL || tmp==NULL) exit(0);

  // Align the texture on a 64k boundary

  while(((int)tunneltex)&0xffff!=0)
  tunneltex++;

  ifstream texfile("Tunnel3D.raw",ios::binary);
  texfile.read((char*)pal,768);
  texfile.read((char*)tmp,65536);
  texfile.close();

  // Generate lookup table for lighting (65 because of possible inaccuracies)

  for(int j=0;j<65;j++)
  { for(int i=0;i<256;i++)
    { int32 r=pal[i*3]<<2;
      int32 g=pal[i*3+1]<<2;
      int32 b=pal[i*3+2]<<2;

      r=(r*j)>>6;
      g=(g*j)>>6;
      b=(b*j)>>6;

      if(r>255) r=255;
      if(g>255) g=255;
      if(b>255) b=255;

      lookup[j*256+i]=(r<<16)|(g<<8)|b;
    }
  }

  // Arrange texture for cache optimised mapping

  for(j=0;j<256;j++)
  for(int i=0;i<256;i++)
  { int newoffs=((i<<8)&0xf800)+(i&0x0007)+((j<<3)&0x07f8);
    *(tunneltex+newoffs)=*(tmp+j*256+i);
  }

  delete [] tmp;
}


void RayTunnel::interpolate()
{ Vector ray,intsc,norm,lvec;
  float x,y,a,b,c,discr,t,res;

  if(lightstatus)                      // Lightsource locked to viewpoint
  memcpy(light,pos,3*sizeof(float));

  matrix_idle(rot);
  matrix_rotate_x(rot,xa&0x3ff,sintab,costab);
  matrix_rotate_y(rot,ya&0x3ff,sintab,costab);
  matrix_rotate_z(rot,za&0x3ff,sintab,costab);

  // Constant factor
  c=2.0f*(pos[0]*pos[0]+pos[1]*pos[1]-radius_sqr);


  // Start raytracing

  y=-100.0;
  for(int j=0;j<26;j++)
  { x=-160.0;

    for(int i=0;i<41;i++)
    { vector_times_matrix(norms[(j<<6)+i],rot,ray);

      a=2.0f*(ray[0]*ray[0]+ray[1]*ray[1]);
      b=2.0f*(pos[0]*ray[0]+pos[1]*ray[1]);

      discr=b*b-a*c;
      if(discr>=0)
      { discr=(float)sqrt(discr);
        t=(-b+discr)/a;

 // Calculate intersection point

        intsc[0]=pos[0]+t*ray[0];
        intsc[1]=pos[1]+t*ray[1];
        intsc[2]=pos[2]+t*ray[2];

 // Calculate texture index at intersection point (cylindrical mapping)
 // Try and adjust the 0.2 to stretch/shrink the texture

        u_array[(j<<6)+i]=((int)(intsc[2]*0.2))<<16;
        v_array[(j<<6)+i]=((int)(fabs(atan2(intsc[1],intsc[0])*256/M_PI)))<<16;


 // Calculate the dotproduct between the normal vector and the vector
 // from the intersection point to the lightsource

        norm[0]=intsc[0]/radius;
        norm[1]=intsc[1]/radius;
        norm[2]=0.0;

        lvec[0]=intsc[0]-light[0];
        lvec[1]=intsc[1]-light[1];
        lvec[2]=intsc[2]-light[2];
        vector_normalize(lvec);

        res=lvec[0]*norm[0]+lvec[1]*norm[1]+lvec[2]*norm[2];

 // Scale the light a bit
        res*=res;
        if(res<0) res=0; if(res>1) res=1;
        res*=63.0;

 // Put it into the light array

        l_array[(j<<6)+i]=((int)res)<<16;
      }
      else
      { u_array[(j<<6)+i]=0;
        v_array[(j<<6)+i]=0;
        l_array[(j<<6)+i]=0;
      }

      x+=8;
    }

    y+=8;
  }

}


void RayTunnel::draw(int32 *dest)
{ int x,y,lu,lv,ru,rv,liu,liv,riu,riv,iu,iv,i,j,ll,rl,lil,ril,l,il;
  unsigned int iadr,adr,til_u,til_v,til_iu,til_iv;
  char8 bla;

  for(j=0;j<25;j++)
  { for(i=0;i<40;i++)
    { iadr=(j<<6)+i;

      // Set up gradients
 
      lu=u_array[iadr]; ru=u_array[iadr+1];
      liu=(u_array[iadr+64]-lu)>>3;
      riu=(u_array[iadr+65]-ru)>>3;

      lv=v_array[iadr]; rv=v_array[iadr+1];
      liv=(v_array[iadr+64]-lv)>>3;
      riv=(v_array[iadr+65]-rv)>>3;

      ll=l_array[iadr]; rl=l_array[iadr+1];
      lil=(l_array[iadr+64]-ll)>>3;
      ril=(l_array[iadr+65]-rl)>>3;
     
      for(y=0;y<8;y++)
      { iu=(ru-lu)>>3;
        iv=(rv-lv)>>3;
        l=ll;
        il=(rl-ll)>>3;

 // Mess up everything for the sake of cache optimised mapping :)

        til_u=((lu<<8)&0xf8000000)+((lu>>1)&0x00007fff)+(lu&0x00070000);
        til_v=((lv<<3)&0x07f80000)+((lv>>1)&0x00007fff);
        til_iu=(((iu<<8)&0xf8000000)+((iu>>1)&0x00007fff)+
                 (iu&0x00070000))|0x07f88000;
        til_iv=(((iv<<3)&0x07f80000)+((iv>>1)&0x00007fff))|0xf8078000;

        adr=til_u+til_v;

        for(x=0;x<8;x++)
        {
   // Interpolate texture u,v and light
          til_u+=til_iu;
          til_v+=til_iv;
          l+=il;

          adr>>=16;

          til_u&=0xf8077fff;
          til_v&=0x07f87fff;

          bla=*(tunneltex+adr);

          adr=til_u+til_v;
          
   // Look up the light and write to buffer
          *(dest+((j<<3)+y)*320+(i<<3)+x)=lookup[((l&0x3f0000)>>8)+bla];
        }

        lu+=liu; ru+=riu;
        lv+=liv; rv+=riv;
        ll+=lil; rl+=ril;
      }
    }
  }
}


// tilt rotates the viewer in the tunnel in a relative / absolute way
void RayTunnel::tilt(int x,int y,int z)
{ xa=(xa+x)&0x3ff; ya=(ya+y)&0x3ff; za=(za+z)&0x3ff;
}

void RayTunnel::tilt(int x,int y,int z,char abs)
{ xa=x&0x3ff; ya=y&0x3ff; za=z&0x3ff;
}


// Relative / absolute move
void RayTunnel::move(float dx,float dy,float dz)
{ pos[0]+=dx; pos[1]+=dy; pos[2]+=dz;
}

void RayTunnel::move(float x,float y,float z,char abs)
{ pos[0]=x; pos[1]=y; pos[2]=z;
}


// Relative / absolute move for the lightsource
void RayTunnel::movelight(float dx,float dy,float dz)
{ light[0]+=dx; light[1]+=dy; light[2]+=dz;
}

void RayTunnel::movelight(float x,float y,float z,char abs)
{ light[0]=x; light[1]=y; light[2]=z;
}


// Lock lightsource to the viewer
void RayTunnel::locklight(char lock)
{ lightstatus=lock&1;
}


int APIENTRY WinMain(HINSTANCE hInst,HINSTANCE hPrevInst,LPSTR lpCmdLine,int nCmdShow)
{

  try {
    Format format(32,0x00FF0000,0x0000FF00,0x000000FF);

    Console console;
    console.open("Tunnel3D demo",320,200,format);

    Surface surface(320,200,format);

    // Create a tunnel, radius=700
    RayTunnel tunnel(700);

    tunnel.load_texture();

    // Light follows the viewer
    tunnel.locklight(1);


    float posz=80.0,phase_x=0.0,phase_y=0.0;
    int angle_x=6,angle_y=2;

    while (!console.key())
    {
       int32 *buffer=(int32*)surface.lock();
       
       tunnel.interpolate();

       // Draw to offscreen boffer
       tunnel.draw(buffer);

       surface.unlock();
       
       // And copy to screen
       surface.copy(console);

       console.update();

       tunnel.tilt(angle_x,angle_y,0);
       tunnel.move((float)sin(phase_x),(float)cos(phase_y),posz);

       phase_x+=0.2f;
       phase_y+=0.1f;
    }

    // close console
    console.close();
  }
  catch(Error error)
  { error.report();

  }
  return 0;
}
