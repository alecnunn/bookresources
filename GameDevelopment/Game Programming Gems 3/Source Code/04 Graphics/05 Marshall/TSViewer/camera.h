/*
IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING. 

By downloading, copying, installing or using the software you agree to this 
license. If you do not agree to this license, do not download, install, 
copy or use the software. 

Intel Open Source License for Triangle Strip Creation, Optimizations, and Rendering Gem

Copyright (c) 2001- 2002, Intel Corporation.
All rights reserved. 

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met: 
* Redistributions of source code must retain the above copyright notice, this 
list of conditions and the following disclaimer. 
* Redistributions in binary form must reproduce the above copyright notice, this 
list of conditions and the following disclaimer in the documentation and/or 
other materials provided with the distribution. 
* Neither the name of Intel Corporation nor the names of its contributors may be 
used to endorse or promote products derived from this software without specific 
prior written permission. 

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ''AS IS'' 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE INTEL CORPORATION OR CONTRIBUTORS BE LIABLE FOR ANY 
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON 
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
EXPORT LAWS: THIS LICENSE ADDS NO RESTRICTIONS TO THE EXPORT LAWS OF YOUR 
JURISDICTION. It is each licensee's responsibility to comply with any export 
regulations applicable in each licensee's jurisdiction. 
*/

// camera class definition

#ifndef CAMERA
#define CAMERA

#include <stdio.h>
//#include "vect3d.h"

#include "mtxlib.h"

class Camera
{
public:
	//X,Y,Z of the camera system
    vector3 X,Y,Z;
	//Orig of the camera
    vector3 Orig;
	//Center of projection
    vector3 COP;
	//NearZ plane, FarZ plane, ViewPlane
    float NearZ, FarZ, ViewZ;
	//left, right, top, and bottom of view window
    float wL, wR, wT, wB;
	//perspective matrix
	float pm[16];
	//inverse, transposed, perspective matrix (for multiplying normals)
	//see page 597 in OpenGL programming Guide (Woo, version 1.1) 'red book'
	//for why this is needed to transform normals
	float pinvM[16];

    Camera() 
    {
        X.set(1,0,0);Y.set(0,1,0);Z.set(0,0,1);
        Orig.set(0,0,0);COP.set(0,0,(float)2.4142);
        NearZ=1;FarZ=-1;ViewZ=0;wL=-1;wR=1;wT=1;wB=-1;
    }

	//same as the openGL lookat command
	//uses the same model as chapter 6 of Foley, VanDam
    void Lookat(vector3 Eye, vector3 ViewRefPt, vector3 ViewUp)
    {
        /* '/' is cross-product */
        Z=Eye-ViewRefPt;
        Z.normalize();
        X=CrossProduct(ViewUp,Z);
        X.normalize();
        Y=CrossProduct(Z,X);
        Y.normalize();
        Orig=Eye;
      //  Print();
    }
	
	//for perspective projection
    void GenPerspective(float Xfov, float Yfov, float Ndist, float Fdist)
    {
        COP.set(0,0,0);
        NearZ = Ndist; FarZ=-Fdist; ViewZ=-1;
        wR=(float)tan(Xfov*0.5f); wL=-(float)wR;wT=(float)tan(Yfov*.5f);wB=-(float)wT;
		StorePerspectiveMatrix();
		StoreInvTransPerspectiveMatrix();
    }

    void Perspective(float Yfov, float Aspect, float Ndist, float Fdist)
    {
        Yfov *= 0.0174532f;  //convert to radians
        float Xfov = Yfov*Aspect;
        GenPerspective(Xfov, Yfov, Ndist, Fdist);

    }

	void StorePerspectiveMatrix()
	{
		pm[0] = (float)(2.0 * NearZ)/(wR - wL);
		pm[5] = (float)(2.0 * NearZ)/(wT - wB);
		pm[8] = (float)(wR + wL)/(wR - wL);
		pm[9] = (float)(wT + wB)/(wT - wB);
		pm[10] = (float)(-(FarZ + NearZ))/(FarZ - NearZ);
		pm[11] = -1.0;
		pm[14] = (float)(-2.0 * FarZ * NearZ)/(FarZ - NearZ);

		pm[1] = pm[2] = pm[3] = pm[4] = pm[6] = pm[7] = pm[12] = pm[13] = pm[15] = 0.0;
	}


	void StoreInvTransPerspectiveMatrix()
	{
		pinvM[0] = (float)((wR-wL)/(2.0*NearZ));
		pinvM[3] = (float)((wR+wL)/(2.0*NearZ));
		pinvM[5] = (float)((wT-wB)/(2.0*NearZ));
		pinvM[7] = (float)((wT+wB)/(2.0*NearZ));
		pinvM[11] = -1.0;
		pinvM[14] = (float)(-(FarZ-NearZ)/(2.0*FarZ*NearZ));
		pinvM[15] = (float)((FarZ+NearZ)/(2.0*FarZ*NearZ));

		pinvM[1] = pinvM[2] = pinvM[4] = pinvM[6] = pinvM[8] = pinvM[9] = pinvM[10] = 0.0;
		pinvM[12] = pinvM[13] = 0.0;
	}

	vector3 MultByInvTransPerspective(vector3 v)
	{
		float r[4];
		vector3 result;
		r[0] = pinvM[0]*v.x;
		r[1] = pinvM[5]*v.y;
		r[2] = pinvM[14]*v.z;
		r[3] = pinvM[3]*v.x+pinvM[7]*v.y+pinvM[11]*v.z;
		printf("r: %f %f %f %f\n",r[0],r[1],r[2],r[3]);

		if(fabs(r[3]) < .000001) {
			result.x = r[0];
			result.y = r[1];
			result.z = r[2];
		//	printf("TOO SMALL\n");
		}
		else {
			result.x = r[0]/r[3];
			result.y = r[0]/r[3];
			result.z = r[0]/r[3];
		}
		return result;
}


	vector3 Vec3dToPerspective(vector3 vP)
	{
		vector3 v;
		float w;
		v.x = (pm[0] * vP.x) + (pm[8] * vP.z);
		v.y = (pm[5] * vP.y) + (pm[9] * vP.z);
		v.z = (pm[10] * vP.z) + pm[14];
		w = (pm[11] * vP.z);
		
		if(fabs(w) < .000001) {
		//	printf("TOO SMALL\n");
		}
		else {
			 v.x = v.x/w;
			 v.y = v.y/w;
			 v.z = v.z/w;
		}
		return v;
	}
 

    void Frustum(float l, float r, float b, float t, float Ndist, float Fdist)
    {
        COP.set(0,0,0);
        NearZ=-Ndist;FarZ=-Fdist;ViewZ=-Ndist;
        wR=r;wL=l;wB=b;wT=t;
    }

    //routines to set camera params
    void SetAxes(vector3 x, vector3 y, vector3 z) { X=x;Y=y;Z=z; }
    void SetOrig(vector3 orig) { Orig=orig; }
    void SetCOP(vector3 cop) { COP = cop; }
    void SetNearFar(float n, float f) {NearZ=n;FarZ=f;}
    void SetViewPlane(float v) { ViewZ = v; }
    void SetWindowExtents(float l, float r, float t, float b) {wL=l;wR=r;wT=t;wB=b;}
    void SetCOP(float Xfov) {COP.z = (float)(ViewZ+(((wR-wL)/2.0f)/tan(Xfov/2.0f))); }
    vector3 wCOP() const
    {
        return (X*COP.x + Y*COP.y + Z*COP.z + Orig);
    }


    vector3 WorldToCam(vector3 wP)
    {
        wP -= Orig; 
        vector3 cP(DotProduct(X,wP),DotProduct(Y,wP),DotProduct(Z,wP)); 
        return cP;
    }

    vector3 CamToWorld(vector3 cP)
    {
        vector3 wP = X*cP.x + Y*cP.y + Z*cP.z + Orig; 
        return(wP);
    }

	vector3 CamToWorldRot(vector3 cP)
	{
		vector3 wP = X*cP.x + Y*cP.y + Z*cP.z;
		wP += Orig;
		return(wP);
	}
	

    void Pitch(float Rads) //about cam x
    {
        vector3 WorldCOP = wCOP();
        float cR = (float)cos(Rads),sR=(float)sin(Rads); //adjust axis
        vector3 NewY(cR*Y.x - sR*Z.x, cR*Y.y - sR*Z.y, cR*Y.z - sR*Z.z);
        vector3 NewZ(sR*Y.x + cR*Z.x, sR*Y.y + cR*Z.y, sR*Y.z + cR*Z.z);
        Y=NewY;
        Z=NewZ;
        Orig = WorldCOP - (X*COP.x+Y*COP.y+Z*COP.z); //adjust origin
    }

    void Yaw(float Rads) //about cam y
    {
        vector3 WorldCOP = wCOP();
        float cR = (float)cos(Rads),sR=(float)sin(Rads); //adjust axis
        vector3 NewX(cR*X.x - sR*Z.x, cR*X.y - sR*Z.y, cR*X.z - sR*Z.z);
        vector3 NewZ(sR*X.x + cR*Z.x, sR*X.y + cR*Z.y, sR*X.z + cR*Z.z);
        X=NewX;
        Z=NewZ;
        Orig = WorldCOP - (X*COP.x+Y*COP.y+Z*COP.z); //adjust origin
    }

    void Roll(float Rads) //about cam z
    {
        vector3 WorldCOP = wCOP();
        float cR = (float)cos(Rads),sR=(float)sin(Rads); //adjust axis
        vector3 NewX(cR*X.x + sR*Y.x, cR*X.y + sR*Y.y, cR*X.z + sR*Y.z);
        vector3 NewY(cR*Y.x - sR*X.x, cR*Y.y - sR*X.y, cR*Y.z - sR*X.z);
        X=NewX;
        Y=NewY;
        Orig = WorldCOP - (X*COP.x+Y*COP.y+Z*COP.z); //adjust origin
    }

    void Move(vector3 Trans)
    {
        //find translation in world coords and update cam origin
        Orig += (X*Trans.x + Y*Trans.y + Z*Trans.z);
    }

    void TrackballRotate(vector3 COR, float Rads, int cAxis)
    {
        //Find COR in CAM Coords before orientation xform
        vector3 cCORbefore = WorldToCam(COR);

        if(cAxis==0) 
            Pitch(-Rads);
        else if(cAxis == 1)
            Yaw(-Rads);
        else if(cAxis == 2)
            Roll(-Rads);

        vector3 cCORafter = WorldToCam(COR);
        //now fix the origin of the cam so that cor is in same place in both coord sys
        vector3 Trans = cCORafter - cCORbefore;
        Move(Trans);
    }
    float *GetViewMatrix(float *M)
    {
        vector3 Tr = -Orig;
        M[0]=X.x; M[4] = X.y; M[8] = X.z; M[12] = DotProduct(X,Tr);
        M[1]=Y.x; M[5] = Y.y; M[9] = Y.z; M[13] = DotProduct(Y,Tr);
        M[2]=Z.x; M[6] = Z.y; M[10] = Z.z; M[14] = DotProduct(Z,Tr);
        M[3]=0; M[7] = 0; M[11] = 0; M[15] = 1;
        return M;
    }

    void Print()
    {
        printf("Camera System Parameters\n");
        printf("      X: (%.3f, %.3f, %.3f)\n",X.x,X.y,X.z);
        printf("      Y: (%.3f, %.3f, %.3f)\n",Y.x,Y.y,Y.z);
        printf("      Z: (%.3f, %.3f, %.3f)\n",Z.x,Z.y,Z.z);
        printf("   Orig: (%.3f, %.3f, %.3f)\n",Orig.x,Orig.y,Orig.z);
        printf("    COP: (%.3f, %.3f, %.3f)\n",COP.x,COP.y,COP.z);
        printf("NFVLRTB: (%.0f, %.0f, %.0f, %.0f, %.0f, %.0f, %.0f)\n",
               NearZ, FarZ, ViewZ, wL, wR, wT, wB);
    }
};
    
#endif