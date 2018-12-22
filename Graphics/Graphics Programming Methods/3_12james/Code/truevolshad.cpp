// Copyright (c) 2003 Rob James 
//#define CG_PROFILE_DEF CG_PROFILE_ARBFP1
#define CG_PROFILE_DEF CG_PROFILE_FP30

#ifdef WIN32
#  include <windows.h>
#  include <MovieMaker.h>
#  pragma warning (disable : 4786)
#endif
#include <string.h> 

#define GLH_EXT_SINGLE_FILE
#include <glh_extensions.h>
#include <glh_glut.h>
#include <cg/cg.h>
#include <Cg/cgGL.h>
#include <glh_nveb.h>
#include <data_path.h>
#include "objload.h"
#include "shadobject.h"
#include <nv_dds.h>

using namespace nv_dds;

// Array to hold our loaded models
model_s models[20];

MovieMaker  VCR;
int g_recording = 0;

//defines Shadow Buffer size and half size

#define DBSIZE 320
#define HDBSIZE 160

//#define DBSIZE 64
//#define HDBSIZE 32

//degrees to Radians macro
#define TORAD(x) ((x)*(3.141592654/180.0))

CGcontext		Context = NULL;

CGprogram	ProgramIter		= NULL;
CGprogram	FProgramIterDT		= NULL;
CGprogram	FProgramIterFOG	= NULL;
CGprogram	FProgramIterTEST	= NULL;
CGprogram	FProgramIterFINAL	= NULL;
CGprogram	FProgramIterSHADOW = NULL;
int shadowtest;
CGprogram	VProgramIter		= NULL;


CGparameter LightBind = NULL;
CGparameter FFooBind = NULL;
CGparameter LScaleBind = NULL;

CGparameter IncDec = NULL;


bool keys[255];
int  g_model=1;
int  g_model_count=0;
int  plevel = 122;

float g_lightrot = -10.0;
float g_lightrotd = 0.6;

float mrot = 13.1;
float rot = 230.0;//250.5;//270.6;//230.6;
float LightPos[4] = {0.0, 5.0, 20.0, 1.0};
float viewpos [3] = {0.0, 5.0, 55.0 };

#define DTOR (3.141592654/180.0)


void cgErrorCallback(void)
{
    CGerror LastError = cgGetError();

    if(LastError)
    {
        printf("%s\n\n", cgGetErrorString(LastError));
        printf("%s\n", cgGetLastListing(Context));
        printf("Cg error, exiting...\n");

        exit(0);
    }
} 


void matrixMult(float* M, float* v)
{
	float res[4];										// Hold Calculated Results
	res[0]=M[ 0]*v[0]+M[ 4]*v[1]+M[ 8]*v[2]+M[12]*v[3];
	res[1]=M[ 1]*v[0]+M[ 5]*v[1]+M[ 9]*v[2]+M[13]*v[3];
	res[2]=M[ 2]*v[0]+M[ 6]*v[1]+M[10]*v[2]+M[14]*v[3];
	res[3]=M[ 3]*v[0]+M[ 7]*v[1]+M[11]*v[2]+M[15]*v[3];
	v[0]=res[0];										// Results Are Stored Back In v[]
	v[1]=res[1];
	v[2]=res[2];
	v[3]=res[3];										// Homogenous Coordinate
}

// CFPBuffer Class
// based on Nvidia's CG SDK  simple_float_pbuffer demo source
class CFPBuffer
{
    public:
        CFPBuffer() 
          : width(0), height(0), handle(NULL), hDC(NULL), hRC(NULL), 
            hWindowDC(NULL), hWindowRC(NULL), valid(false)
        { }


		HPBUFFERARB handle;
    
        bool create(int w, int h, bool depth)
        {
            // clean up pbuffer resources if already created
            if (valid)
                destroy();
    
            width = w;
            height = h;
            
            // get a copy of the main windows device context and rendering
            // context
            hWindowDC = wglGetCurrentDC();
            hWindowRC = wglGetCurrentContext();
    
            int format = 0;
			
            unsigned int nformats;

			// This is going to be our 32 bit depth buffer
	
            int d_attribList[] = 
            {
                WGL_SUPPORT_OPENGL_ARB, TRUE,
				WGL_RED_BITS_ARB,               32,
				WGL_GREEN_BITS_ARB,             32,
				WGL_BLUE_BITS_ARB,              32,
				WGL_ALPHA_BITS_ARB,				32,
				WGL_STENCIL_BITS_ARB,           8,
                WGL_DEPTH_BITS_ARB,             24,
                WGL_FLOAT_COMPONENTS_NV,        GL_TRUE,
                WGL_DRAW_TO_PBUFFER_ARB,        GL_TRUE,
				WGL_BIND_TO_TEXTURE_RECTANGLE_FLOAT_RGBA_NV,	GL_TRUE,
				WGL_DOUBLE_BUFFER_ARB, GL_FALSE,				
				0
            };

            int attribList[] = 
            {
				WGL_SUPPORT_OPENGL_ARB, TRUE,
                WGL_RED_BITS_ARB,               8,
				WGL_GREEN_BITS_ARB,             8,
				WGL_BLUE_BITS_ARB,              8,
				WGL_ALPHA_BITS_ARB,				8,
				WGL_STENCIL_BITS_ARB,           8,
                WGL_DEPTH_BITS_ARB,             24,
                WGL_FLOAT_COMPONENTS_NV,        GL_FALSE,
                WGL_DRAW_TO_PBUFFER_ARB,        GL_TRUE,
				WGL_BIND_TO_TEXTURE_RECTANGLE_RGBA_NV,	FALSE,
				WGL_DOUBLE_BUFFER_ARB, FALSE,
				0
            };

			
			if (depth)
				wglChoosePixelFormatARB(hWindowDC, d_attribList, NULL, 1, &format, &nformats);
			else
				wglChoosePixelFormatARB(hWindowDC, attribList, NULL, 1, &format, &nformats);


            if (nformats == 0)
            {
                printf("Unable to find any RGBA32 floating point pixel formats\n");
                return false;
            }
        
            // clear attribute list
            attribList[0] = 0;
			for ( int a = 0; a < 2*7; a++ ) 
				attribList[a] = 0;

			/**/

			int niattribs = 0;
			attribList[2*niattribs  ] = WGL_TEXTURE_TARGET_ARB;
			attribList[2*niattribs+1] = WGL_TEXTURE_RECTANGLE_NV;
			niattribs++;

			if (depth)
			{
				attribList[2*niattribs  ] = WGL_TEXTURE_FORMAT_ARB;
				attribList[2*niattribs+1] = WGL_TEXTURE_FLOAT_RGBA_NV ;
				niattribs++;
			}
			else
			{
				attribList[2*niattribs  ] = WGL_TEXTURE_FORMAT_ARB;
				attribList[2*niattribs+1] = WGL_TEXTURE_RGBA_ARB;
				niattribs++;				
			}
			        
            handle = wglCreatePbufferARB(hWindowDC, format, width, height, attribList);
            if (handle == NULL) 
            {
                printf("Unable to create floating point pbuffer (wglCreatePbufferARB failed)\n");
                return false;
            }
        
            hDC = wglGetPbufferDCARB(handle);
            if (hDC == NULL) 
            {
                printf("Unable to retrieve handle to pbuffer device context\n");
                return false;
            }
        
            hRC = wglCreateContext(hDC);
            if (hRC == NULL) 
            {
                printf("Unable to create a rendering context for the pbuffer\n");
                return false;
            }    
            
            if (!wglShareLists(hWindowRC, hRC)) 
            {
                printf("Unable to share data between rendering contexts\n");
                return false;
            }
        
            valid = true;
    
            return true;
        }
    
        void activate()
        {
            wglMakeCurrent(hDC, hRC);
        }
    
        void deactivate()
        {
            wglMakeCurrent(hWindowDC, hWindowRC);
        }
    
        void destroy()
        {
            if (valid)
            {
                // Delete pbuffer and related
                wglDeleteContext(hRC);
                wglReleasePbufferDCARB(handle, hDC);
                wglDestroyPbufferARB(handle);
        
                // Return to the normal context
                wglMakeCurrent(hWindowDC, hWindowRC);
    
                width = 0;
                height = 0;
                handle = NULL;
                hDC = NULL;
                hRC = NULL;
                hWindowDC = NULL;
                hWindowRC = NULL;
                valid = false;
            }
        }
    
        ~CFPBuffer()
        {
            destroy();
        }
    
    private:
        int width;
        int height;

        
        HDC hDC;
        HGLRC hRC;

        HDC hWindowDC;
        HGLRC hWindowRC;

        bool valid;
};

// function definitions
void init_opengl();
void reshape(int w, int h);
void display(void);
void keyboard(unsigned char key, int x, int y);
void idle();

// variables 
CFPBuffer fpbuffer;
CFPBuffer fdbuffer;
GLuint fptexture;
GLuint fdtexture;
GLuint floortexture;
GLuint rooftexture;
GLuint walltexture;


void init_opengl()
{
    if (!glh_init_extensions(	"GL_ARB_multitexture " \
								"WGL_ARB_pbuffer " \
								"WGL_ARB_render_texture " \
								"WGL_NV_render_texture_rectangle "\
								"GL_NV_texture_rectangle " \
								"GL_ARB_texture_compression " \
								"GL_EXT_texture_compression_s3tc " \
								"GL_VERSION_1_2 " \
								"WGL_ARB_pixel_format " \
								"GL_NV_fragment_program " \
								"GL_NV_vertex_program " \
								"GL_EXT_blend_func_separate "\
								"GL_EXT_blend_subtract "\
								"GL_EXT_blend_minmax "\
								"GL_NV_float_buffer "))


    {
        printf("Unable to load the following extension(s): %s\n\nExiting...\n", 
               glh_get_unsupported_extensions());
        exit(-1);
    }

	
	data_path media;
    media.path.push_back(".");

    glClearColor(0.2, 0.2, 0.2, 1.0);

    glGenTextures(1, &fptexture);

	glActiveTextureARB( GL_TEXTURE0_ARB );
    glBindTexture(GL_TEXTURE_RECTANGLE_NV, fptexture);  

	glTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_FLOAT_RGBA32_NV, DBSIZE, DBSIZE, 0, GL_RGBA, GL_FLOAT, 0);
		
    glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MIN_FILTER,GL_NEAREST);// GL_NEAREST);//GL_LINEAR);
    glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MAG_FILTER,GL_NEAREST);// GL_NEAREST);//GL_LINEAR);
    glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	if (!fpbuffer.create(DBSIZE, DBSIZE, true))
	{ 
		printf("poo!\n");
        exit(-1);
	}
    
    fpbuffer.activate();
    reshape(DBSIZE, DBSIZE);
    fpbuffer.deactivate(); 	


	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glActiveTextureARB( GL_TEXTURE0_ARB );

	glGenTextures(1, &floortexture);
	glBindTexture(GL_TEXTURE_2D, floortexture);
	CDDSImage image;
    if (image.load(media.get_file("art//wood.dds")))   image.upload_texture2D();    
	//if (image.load(media.get_file("floor3.dds")))   image.upload_texture2D();    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glGenTextures(1, &rooftexture);
	glBindTexture(GL_TEXTURE_2D, rooftexture);
	if (image.load(media.get_file("art//roof.dds")))   image.upload_texture2D();    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glGenTextures(1, &walltexture);
	glBindTexture(GL_TEXTURE_2D, walltexture);
	//if (image.load(media.get_file("wall2.dds")))   image.upload_texture2D();    
	if (image.load(media.get_file("art//floor3.dds")))   image.upload_texture2D();    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	
	for (int x = 0;x < 255;x++) 
		keys[x] = false;

	keys['r'] = true;
	keys['R'] = true;
	//keys['t'] = true;
	//keys['y'] = true;


	// This first model is used as the light shade :)
	LoadAscObject("models//cone3b.asc", &models[g_model_count++]); 
	LoadAscObject("models//pole.asc", &models[g_model_count++]);	
	LoadAscObject("models//sphere2.asc", &models[g_model_count++]); 
	LoadAscObject("models//slit.asc", &models[g_model_count++]);	
	LoadAscObject("models//ss.asc", &models[g_model_count++]);	
	LoadAscObject("models//sss.asc", &models[g_model_count++]);	
	LoadAscObject("models//hs.asc", &models[g_model_count++]);	
	LoadAscObject("models//4holes.asc", &models[g_model_count++]);
	LoadAscObject("models//oct.asc", &models[g_model_count++]);	
	LoadAscObject("models//box.asc", &models[g_model_count++]);	
	LoadAscObject("models//5star.asc", &models[g_model_count++]);

}

void
init_cg()
{	
	GLuint ProgId = 0;

    data_path media;
    media.path.push_back(".");
    
	// create CGcontext - We only need one for all our vert and pixel shaders
    Context = cgCreateContext();
    assert(Context != NULL);    

	cgSetErrorCallback(cgErrorCallback);
	
    // adding source text to context 
    ProgramIter =  cgCreateProgramFromFile(Context,CG_SOURCE, media.get_file("cg_shimmerVP.cg").data(), CG_PROFILE_VP20, NULL, NULL);

    if(ProgramIter != NULL)
    {
        ProgId++;

        glGetError();

         cgGLLoadProgram(ProgramIter);        

        // get handle to shader parameter
        LightBind	= cgGetNamedParameter(ProgramIter, "LightPos");
		assert(LightBind != NULL);

		LScaleBind	= cgGetNamedParameter(ProgramIter, "LScale");
		assert(LScaleBind != NULL);
    }

	// Add vertex-program source text to vertex-profile context ********************************************
    FProgramIterSHADOW = cgCreateProgramFromFile(Context, CG_SOURCE,media.get_file("cg_shad_FP.cg").data(), CG_PROFILE_DEF, NULL,NULL);
 
    if(FProgramIterSHADOW != NULL) 
	{
        ProgId++;
		shadowtest = ProgId;
        cgGLLoadProgram(FProgramIterSHADOW);	
		
		IncDec = cgGetNamedParameter(FProgramIterSHADOW, "incdec");
	}
	
	
	// Add vertex-program source text to vertex-profile context ******************************************
    FProgramIterDT = cgCreateProgramFromFile(Context,CG_SOURCE, media.get_file("cg_shimmer_dtestFP.cg").data(), CG_PROFILE_DEF, NULL,NULL);
  
    if(FProgramIterDT != NULL) 
	{
        ProgId++;
        cgGLLoadProgram(FProgramIterDT);        
	}

	// Add vertex-program source text to vertex-profile context ********************************************
    FProgramIterFINAL = cgCreateProgramFromFile(Context,CG_SOURCE,media.get_file("cg_final_FP.cg").data(), CG_PROFILE_DEF, NULL,NULL);
  

    if(FProgramIterFINAL!= NULL) 
	{
        ProgId++;
        cgGLLoadProgram(FProgramIterFINAL);
			
	}
	//printf("\n%s\n", fixstr);
} 


// draw the nice simple room in which all the foggy action 
// takes place
void drawRoom(bool tex)	
{
	// Textured or not ?
	if (tex)
	{
		glColor3f(1.0, 1.0, 1.0);
		glEnable(GL_TEXTURE_2D);

		glActiveTextureARB( GL_TEXTURE0_ARB );
		glBindTexture(GL_TEXTURE_2D, floortexture);
	}

	// Begin Drawing Quads
	glBegin(GL_QUADS);									
	// Floor
	glNormal3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-40.0f,-10.0f,-40.0f);	
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-40.0f,-10.0f, 40.0f);	
	glTexCoord2f(1.0, 1.0);
	glVertex3f( 40.0f,-10.0f, 40.0f);	
	glTexCoord2f(1.0, 0.0);
	glVertex3f( 40.0f,-10.0f,-40.0f);	
	glEnd();						

	// Ceiling
	glBindTexture(GL_TEXTURE_2D, rooftexture);
	glBegin(GL_QUADS);				
	glNormal3f(0.0f,-1.0f, 0.0f);	
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-40.0f, 30.0f, 40.0f);	
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-40.0f, 30.0f,-40.0f);
	glTexCoord2f(1.0, 0.0);
	glVertex3f( 40.0f, 30.0f,-40.0f);	
	glTexCoord2f(1.0, 1.0);
	glVertex3f( 40.0f, 30.0f, 40.0f);	
	glEnd();						

	// Begin Drawing Quads
	glBindTexture(GL_TEXTURE_2D, walltexture);
	glBegin(GL_QUADS);
	// Front Wall
	glNormal3f(0.0f, 0.0f, 1.0f);	
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-40.0f, 30.0f,-40.0f);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-40.0f,-10.0f,-40.0f);
	glTexCoord2f(1.0, 0.0);
	glVertex3f( 40.0f,-10.0f,-40.0f);
	glTexCoord2f(1.0, 1.0);
	glVertex3f( 40.0f, 30.0f,-40.0f);
	glEnd();						

	glBegin(GL_QUADS);			
	// Back Wall
	glNormal3f(0.0f, 0.0f,-1.0f);	
	glTexCoord2f(1.0, 1.0);
	glVertex3f( 40.0f, 30.0f, 40.0f);
	glTexCoord2f(1.0, 0.0);
	glVertex3f( 40.0f,-10.0f, 40.0f);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-40.0f,-10.0f, 40.0f);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-40.0f, 30.0f, 40.0f);
	glEnd();						

	glBegin(GL_QUADS);				
	// Left Wall
	glNormal3f(1.0f, 0.0f, 0.0f);	
	glTexCoord2f(1.0, 1.0);
	glVertex3f(-40.0f, 30.0f, 40.0f);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-40.0f,-10.0f, 40.0f);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-40.0f,-10.0f,-40.0f);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(-40.0f, 30.0f,-40.0f);
	glEnd();						

	// Right Wall
	glBegin(GL_QUADS);		
	glNormal3f(-1.0f, 0.0f, 0.0f);
	glTexCoord2f(1.0, 0.0);
	glVertex3f( 40.0f, 30.0f,-40.0f);	
	glTexCoord2f(0.0, 0.0);
	glVertex3f( 40.0f,-10.0f,-40.0f);	
	glTexCoord2f(0.0, 1.0);
	glVertex3f( 40.0f,-10.0f, 40.0f);	
	glTexCoord2f(1.0, 1.0);
	glVertex3f( 40.0f, 30.0f, 40.0f);	
	glEnd();

	
	if (tex)
	{
		glDisable(GL_TEXTURE_2D);
	}

	
}


void drawSceneShadowVolumes(float *lp)
{
	float nLightPos[4]= {0.0, 00.0, 00.0, 1.0};

	glPushMatrix();
	glRotatef(rot, 0.0,1.0,0.0);

	cgGLSetParameter4f(LightBind, lp[0],lp[1],lp[2], 1.0);		
	// Draw the selected model
	glPushMatrix();
	if (keys['y'])
		glRotatef(mrot, 0.0, 1.0, 0.0);
	else
		glRotatef(mrot, 0.0, 0.0, 1.0);

	drawModelShadowVolume(&models[g_model], lp, 1.0);
	glPopMatrix();

	cgGLSetParameter4f(LightBind, 0,0,0, 1.0);		
	//Draw the light surround
	glPushMatrix();
	glTranslatef(LightPos[0], LightPos[1], LightPos[2]);
	glRotatef(g_lightrot, 1.0, 0.0, 0.0);
	if (!keys['o'])
		drawModelShadowVolume(&models[0], nLightPos, 5.0);
	glPopMatrix();

	glPopMatrix();
}


// The the scene with no texture, 
/*
void drawSceneDepth()
{
	glPushMatrix();

	glRotatef(rot, 0.0,1.0,0.0);
	drawRoom(true);
	
	
	glPushMatrix();
	if (keys['y'])
		glRotatef(mrot, 0.0, 1.0, 0.0);
	else
		glRotatef(mrot, 0.0, 0.0, 1.0);

	drawModel(&models[g_model]);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(LightPos[0], LightPos[1], LightPos[2]);
	glRotatef(g_lightrot, 1.0, 0.0, 0.0);
	if (!keys['o'])
		drawModel(&models[0]);
	glPopMatrix();	

	glPopMatrix();
}

*/

void drawCapQuad()
{	
	cgGLSetParameter4f(LightBind, LightPos[0], LightPos[1], LightPos[2], 1.0);	

	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	glRectf(-1,-1,1,1);

	glDisable(GL_CULL_FACE);

    glBegin(GL_QUADS);
        glTexCoord2f(0.0, 0.0);			glVertex2f(-1.0, -1.0);
        glTexCoord2f(DBSIZE, 0.0);		glVertex2f(1.0, -1.0);
        glTexCoord2f(DBSIZE, DBSIZE);	glVertex2f(1.0, 1.0);
        glTexCoord2f(0.0, DBSIZE);		glVertex2f(-1.0, 1.0);
    glEnd();
	
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();	
}

/*
void drawPbuffer()
{

	glDisable(GL_STENCIL_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glColor4f(1.0,1.0,1.0,1.0);


	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glRectf(-1,-1,1,1);

	glDisable(GL_BLEND);

	glEnable(GL_TEXTURE_RECTANGLE_NV);

	glActiveTextureARB( GL_TEXTURE0_ARB );
	glBindTexture(GL_TEXTURE_RECTANGLE_NV, fptexture);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glDisable(GL_CULL_FACE);

    glBegin(GL_QUADS);
        glTexCoord2f(0.0, 0.0);			glVertex2f(-1.0, -1.0);
        glTexCoord2f(DBSIZE, 0.0);		glVertex2f(1.0, -1.0);
        glTexCoord2f(DBSIZE, DBSIZE);	glVertex2f(1.0, 1.0);
        glTexCoord2f(0.0, DBSIZE);		glVertex2f(-1.0, 1.0);
    glEnd();
	
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glDisable(GL_TEXTURE_RECTANGLE_NV);	
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

*/

void drawFogQuad()
{
	// A nice white light/fog
	//glColor4f(1.0, 1.0, 1.0, 1.0);

	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	glRectf(-1,-1,1,1);

	glDisable(GL_CULL_FACE);

    glBegin(GL_QUADS);
        glTexCoord2f(0.0, 0.0);			glVertex2f(-1.0, -1.0);
        glTexCoord2f(DBSIZE, 0.0);		glVertex2f(1.0, -1.0);
        glTexCoord2f(DBSIZE, DBSIZE);	glVertex2f(1.0, 1.0);
        glTexCoord2f(0.0, DBSIZE);		glVertex2f(-1.0, 1.0);
    glEnd();
	
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

// The the scene with no texture, 
void drawSceneTest()
{
	//float LightPos[4] = {0.0, 0.0, 20.0, 1.0};
	float nLightPos[4]= {0.0, 00.0, 00.0, 1.0};
	float lp[4];
	float wlp[4];
	float Minv[16]; 

	glPushMatrix();
	glRotatef(rot, 0.0,1.0,0.0);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
    
	drawRoom(true);

	// This chunk just moves the light position into model space
	// for easy lights and shadows
	glPushMatrix();
	glLoadIdentity();									
	if (keys['y'])
		glRotatef(-mrot, 0.0, 1.0, 0.0);
	else
		glRotatef(-mrot, 0.0, 0.0, 1.0);

	glGetFloatv(GL_MODELVIEW_MATRIX,Minv);	
	lp[0] = LightPos[0];								
	lp[1] = LightPos[1];								
	lp[2] = LightPos[2];								
	lp[3] = LightPos[3];								
	matrixMult(Minv, lp);								
	glTranslatef(0.0, 0.0, 0.0);			            
	glGetFloatv(GL_MODELVIEW_MATRIX,Minv);				
	wlp[0] = 0.0f;										
	wlp[1] = 0.0f;										
	wlp[2] = 0.0f;										
	wlp[3] = 1.0f;
	matrixMult(Minv, wlp);								
	glPopMatrix();
	
	glPushMatrix();
	if (keys['y'])
		glRotatef(mrot, 0.0, 1.0, 0.0);
	else
		glRotatef(mrot, 0.0, 0.0, 1.0);

	glColor4f(1.0f, 0.2f, 0.1f, 1.0f);

	cgGLSetParameter4f(LightBind, lp[0],lp[1],lp[2], 1.0);
	//cgGLSetParameter4f(LightBind, 0.0,0.0,0.0, 1.0);
	// Draw the model we have selected
	drawModel(&models[g_model]);
	drawModelShadowCR(&models[g_model], lp, keys['v'], 2.0, false);

	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(LightPos[0], LightPos[1], LightPos[2]);
	glColor4f(0.3f, .7f, 0.7f, 1.0f);
	glRotatef(g_lightrot, 1.0, 0.0, 0.0);

	cgGLSetParameter4f(LightBind, 0.0,0.0,0.0, 1.0);
	// Draw the lamp-shade around the light
	if (!keys['o'])
	{
		drawModel(&models[0]);
		drawModelShadowCR(&models[0], nLightPos, keys['v'], 5.0, false); //true
	}

	glPopMatrix();

	if (keys['y'])
		glRotatef(mrot, 0.0, 1.0, 0.0);
	else
		glRotatef(mrot, 0.0, 0.0, 1.0);

	glColor4f(0.7f, 0.7f, 0.5f, 1.0f);
	glDisable(GL_LIGHTING);
	glDepthMask(GL_FALSE);
	glTranslatef(lp[0], lp[1], lp[2]);														
	glutSolidSphere(0.2f, 16, 8);

	glEnable(GL_LIGHTING);
	glDepthMask(GL_TRUE);			
	glPopMatrix();
}

void drawSceneShadowed(float *lp, bool applyShadow)
{
	glDisable(GL_TEXTURE_2D);
	
	cgGLEnableProfile(CG_PROFILE_VP20);	
	cgGLBindProgram(ProgramIter);
	
	glTrackMatrixNV( GL_VERTEX_PROGRAM_NV, 0, GL_MODELVIEW_PROJECTION_NV, GL_IDENTITY_NV );
	glTrackMatrixNV( GL_VERTEX_PROGRAM_NV, 4, GL_MODELVIEW,  GL_IDENTITY_NV );
	glTrackMatrixNV( GL_VERTEX_PROGRAM_NV, 8, GL_MODELVIEW, GL_INVERSE_TRANSPOSE_NV );
	
//	cgGLSetMatrixParameterfc(LScaleBind, (const float *)lscale);
	cgGLSetParameter4f(LightBind, lp[0],lp[1],lp[2], 1.0);		
	
	drawSceneTest();
	
	cgGLDisableProfile(CG_PROFILE_VP20);
	
	if (applyShadow)
		drawShadowQuad();
}

void updateTransforms ()
{
	if (keys['r'])	rot  += 0.2;
	if (keys['R'])	mrot += 3.4;
	if (keys['t'])	
	{
		g_lightrot += g_lightrotd;
		if (g_lightrot > 0.0 || g_lightrot < -25.0)
		{
			g_lightrotd = - g_lightrotd;
		}
	}	
}

// The very important bit...
void display()
{
	float lp[4];
	float Minv[16]; 
	float lscale[16];
	unsigned char  occtest;
	unsigned char pStencil [DBSIZE*DBSIZE];
	static int frame = 0;
	
	frame++;
	glClearColor(0.5, 0.0, 0.0, 0.0);	
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();

	glTranslatef(HDBSIZE, HDBSIZE, HDBSIZE);
	glScalef(HDBSIZE, HDBSIZE, HDBSIZE);
	
	glGetFloatv(GL_TEXTURE_MATRIX,(float*)lscale);
		
    glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
    glPushMatrix();

	gluLookAt(viewpos[0], viewpos[1], viewpos[2],  0.0,0.0,0.0, 0.0,1.0,0.0);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);


	updateTransforms();
	// Stick light pos into model space

	glPushMatrix();
	glLoadIdentity();
	if (keys['y'])
		glRotatef(-mrot, 0.0, 1.0, 0.0);
	else
		glRotatef(-mrot, 0.0, 0.0, 1.0);
	glGetFloatv(GL_MODELVIEW_MATRIX,Minv);
	lp[0] = LightPos[0];
	lp[1] = LightPos[1];
	lp[2] = LightPos[2];
	lp[3] = LightPos[3];
	matrixMult(Minv, lp);
	glPopMatrix();

	
	// The volumetric fog uses the pbuffer to sotr the following:
	// Storing in RGBA the following
	// R = Depth Peel Shadow Count
	// G = In Scatter Light
	// B = Fragment Depth - The Second Depth Buffer
	// A = Fragment Depth - The Second Depth Buffer

	// Step 1 - draw the scene into the pBuffer 
	fpbuffer.activate();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();

	gluLookAt(viewpos[0], viewpos[1], viewpos[2],  0.0,0.0,0.0, 0.0,1.0,0.0);

	glDisable(GL_TEXTURE_RECTANGLE_NV);
	glEnable(GL_DEPTH_TEST);

	glClearStencil(127);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);	
	
	cgGLEnableProfile(CG_PROFILE_VP20);	
	cgGLBindProgram(ProgramIter);

	cgGLBindProgram(FProgramIterDT);
	cgGLEnableProfile(CG_PROFILE_DEF);

	glTrackMatrixNV( GL_VERTEX_PROGRAM_NV, 0, GL_MODELVIEW_PROJECTION_NV, GL_IDENTITY_NV );
	glTrackMatrixNV( GL_VERTEX_PROGRAM_NV, 4, GL_MODELVIEW, GL_IDENTITY_NV );
	glTrackMatrixNV( GL_VERTEX_PROGRAM_NV, 8, GL_MODELVIEW, GL_INVERSE_TRANSPOSE_NV );

	//cgGLSetParameter4f(LightBind,    lp[0],lp[1],lp[2], 1.0);
	cgGLSetMatrixParameterfc(LScaleBind, (float *)lscale);

	// Set the initial frag depth and the stencil count.
	
	drawSceneShadowed(lp, false);

	glPopMatrix();

	// Step 2 - mimic the stencil buffer shadow volume count
	// Render the shadow volume geometry storing Shadow Count into R & B

	cgGLDisableProfile(CG_PROFILE_DEF);
	cgGLDisableProfile(CG_PROFILE_VP20);
	
	glReadPixels(  0, 0,  DBSIZE,  DBSIZE, GL_STENCIL_INDEX ,  GL_UNSIGNED_BYTE,  &pStencil);
	glColorMask(1, 0, 0, 0);	// ONLY update x, our stencil count
	glDrawPixels(DBSIZE, DBSIZE,GL_RED, GL_UNSIGNED_BYTE, &pStencil);
	glColorMask(1, 1, 1, 1);	
	

	// Third Stage 
	// Accumulate the in-scatter light while depth peeling 
	// the shadow volumes from back to front

	glEnable(GL_DEPTH_TEST);	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();
	gluLookAt(viewpos[0], viewpos[1], viewpos[2],  0.0,0.0,0.0, 0.0,1.0,0.0);
	
	cgGLEnableProfile(CG_PROFILE_VP20);	

	glTrackMatrixNV( GL_VERTEX_PROGRAM_NV, 0, GL_MODELVIEW_PROJECTION_NV, GL_IDENTITY_NV );
	glTrackMatrixNV( GL_VERTEX_PROGRAM_NV, 4, GL_MODELVIEW, GL_IDENTITY_NV );
	glTrackMatrixNV( GL_VERTEX_PROGRAM_NV, 8, GL_MODELVIEW, GL_INVERSE_TRANSPOSE_NV );

	cgGLSetMatrixParameterfc(LScaleBind, (const float *)lscale);
	cgGLSetParameter4f(LightBind,    lp[0],lp[1],lp[2], 1.0);

	glDepthMask(GL_TRUE);

	// clear to near for depth peeling
	glClearDepth( 0.0 ); 
	glDisable(GL_BLEND);
	glCullFace(GL_BACK);

	int x=0;
	/**/

	// peel the next layer into the pbuffer
	cgGLDisableProfile(CG_PROFILE_DEF);
	cgGLBindProgram(FProgramIterSHADOW);
	cgGLEnableProfile(CG_PROFILE_DEF);
	glDepthFunc(GL_GEQUAL);	
	glEnable(GL_CULL_FACE);

	do
	{	// Grab the pbuffer, copy and bind		
		glActiveTextureARB( GL_TEXTURE1_ARB );
		glBindTexture(GL_TEXTURE_RECTANGLE_NV, fptexture);
		
		if (0)
			glCopyTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_FLOAT_RGBA32_NV, 0, 0, DBSIZE, DBSIZE, 0); 
		else
			glCopyTexSubImage2D(GL_TEXTURE_RECTANGLE_NV, 0, 0, 0, 0, 0, DBSIZE, DBSIZE);
	
		glClear(GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);				
		
		glEnable(GL_OCCLUSION_TEST_HP); // Cool!
		
		glCullFace(GL_BACK);
		cgGLSetParameter1f(IncDec, -0.01);		
		drawSceneShadowVolumes(lp);		// prime the depth buffer and accumulate in-scatter

		glCullFace(GL_FRONT);
		cgGLSetParameter1f(IncDec, 0.01);		
		drawSceneShadowVolumes(lp);		// prime the depth buffer and accumulate in-scatter
		
		glDisable(GL_OCCLUSION_TEST_HP);

		glGetBooleanv(GL_OCCLUSION_TEST_RESULT_HP,&occtest);// When we have peeled all out layers then this returns 0
		//printf("..%d\n",x);
		x++;
	}while (occtest && x < plevel); // catch all 
/**/
	
	glCullFace(GL_BACK);
	//glColorMask(1, 1, 1, 1);
	
	//printf("Peeled %d layers (%d) \n",x,plevel);
	/**/
		
	// Draw a final full screen 'cap'
	// To accumulate any left over light not capped by
	// visible shadow volumes
	glDepthFunc(GL_GREATER);
	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	glBlendEquationEXT(GL_FUNC_ADD_EXT);
	glBlendFunc(GL_ONE,GL_ZERO);
		
	if (keys['c'])
	{	
		// peel the next layer into the pbuffer
		cgGLDisableProfile(CG_PROFILE_DEF);
		cgGLBindProgram(FProgramIterSHADOW);
		cgGLEnableProfile(CG_PROFILE_DEF);
		drawCapQuad();	

		glEnable(GL_DEPTH_TEST);
		glClearDepth( 1.0 );  
		glPopMatrix();

		//Final pass comming up!    
		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);		
		glDepthFunc(GL_LESS);
		glEnable(GL_CULL_FACE);

		cgGLDisableProfile(CG_PROFILE_DEF);
		cgGLDisableProfile(CG_PROFILE_VP20);

		glActiveTextureARB( GL_TEXTURE1_ARB );
		glBindTexture(GL_TEXTURE_RECTANGLE_NV, fptexture);
		glEnable(GL_TEXTURE_RECTANGLE_NV);
		glCopyTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_FLOAT_RGBA32_NV, 0, 0, DBSIZE, DBSIZE, 0); 
	}
	else
	{
		glEnable(GL_DEPTH_TEST);
		glClearDepth( 1.0 );  
		glColorMask(1, 1, 1, 1);
		glPopMatrix();

		//Final pass comming up!    
		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);		
		glDepthFunc(GL_LESS);
		glEnable(GL_CULL_FACE);

		cgGLDisableProfile(CG_PROFILE_DEF);
		cgGLDisableProfile(CG_PROFILE_VP20);
	}

	// Phew, now on to the actual final render!
	// Turn off the pbuffer.
	fpbuffer.deactivate();
	
	drawSceneShadowed(lp, true);
		
	// Draw the fog quad
	if (!keys['p'])
	{
		cgGLEnableProfile(CG_PROFILE_DEF);
		cgGLBindProgram(FProgramIterFINAL);	

		// Now draw the scene blending in the surface shadow and in-scatter light
		glActiveTextureARB( GL_TEXTURE0_ARB );
		glBindTexture(GL_TEXTURE_RECTANGLE_NV, fptexture);
		glEnable(GL_TEXTURE_RECTANGLE_NV);
		
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		drawFogQuad();
		
		glDisable(GL_BLEND);
		
		cgGLDisableProfile(CG_PROFILE_DEF);
		glDisable(GL_TEXTURE_RECTANGLE_NV);
		glActiveTextureARB( GL_TEXTURE0_ARB );
	}
		
	glPopMatrix();
	glutSwapBuffers();

	//g_model = (g_model + 1)%g_model_count;

	if (g_recording) VCR.Snap();
}




void reshape(int w, int h)
{
    if (h == 0) h = 1;
    
    glViewport(0, 0, w, h);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    gluPerspective(60.0, (GLfloat)w/(GLfloat)h, 0.10, 1500.0);// 2000.0);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


void keyboard(unsigned char key, int x, int y)
{
	static int e = GL_FILL;
	
	printf("%c pressed\n",key);
	
	keys[key] = !keys[key];
    switch (key) 
    {
	case '}':
		mrot+=10.0;
	break;

	case '{':
		mrot-=10.0;		
	break;

	case ']':
		plevel ++;
	break;

	case '[':
		plevel--;
		if (plevel<0) plevel = 0;
	break;

	case '*':
		rot += 20.0;
		break;
		
	case '/':
		rot -= 20.0;
		break;
		
	case 'w':		
        e  = (e == GL_FILL) ? GL_LINE : GL_FILL;
        glPolygonMode(GL_FRONT,e);
		break;
		
	case '+':
		rot += 90.0;
		break;
		
	case '-':
		rot -= 90.0;
		break;
		
	case 'm':
		g_model = (g_model + 1)%g_model_count;
		break;
		
	case 'M':
		g_model = (g_model - 1)%g_model_count;
		break;		

	case 'A':
		if (g_recording)
		{
			printf("Finish Capture\n");
			g_recording = 0;
			VCR.EndCapture();
		}
		break;

	case 'a':
		if (!g_recording)
		{
			printf("Start Capture\n");
			g_recording = 1;
			VCR.StartCapture("shadow.avi");
		}
			break;
		
	case 27:
	case 'q':		
		if (g_recording)
		{
			printf("Finish Capture\n");
			g_recording = 0;
			VCR.EndCapture();
		}
		
		cgDestroyProgram(ProgramIter);
		cgDestroyProgram(FProgramIterDT);
		cgDestroyProgram(FProgramIterSHADOW);
		cgDestroyProgram(FProgramIterFINAL);
		cgDestroyContext(Context);
		exit(0);
		break;
    }
    glutPostRedisplay();
}

void idle()
{
    glutPostRedisplay();
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
    
	//glutInitWindowSize(128,128 ); //small for testing
	glutInitWindowSize(400, 400);

	if (0)
	{
		glutGameModeString("640x480:32");
		glutEnterGameMode();
	}
	else
	{	
		glutCreateWindow("Misty Room by Rob James");
	}

    glutReshapeFunc(reshape);

	init_opengl();
	init_cg();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutIdleFunc(idle);

    glutMainLoop();

    return 0;
}

