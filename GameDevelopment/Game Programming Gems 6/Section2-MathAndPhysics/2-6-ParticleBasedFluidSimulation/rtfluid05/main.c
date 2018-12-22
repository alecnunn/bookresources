//#define CUBEMAP_ONLY

#include <memory.h>
#include <windows.h>

#include <math.h>
#include <stdio.h>
#include <assert.h>

#include "gpu_util.h"	

#include "sph_common.h"
#include "cpu_sph.h"
#include "sph_benchmark.h"
#include "sph_render.h"
 

#include <GL/glut.h>

//#define FRAME_TIMESTEP 0.010f
#define BENCHMARK_ITR 500
#define SMOOTHING_LENGTH 0.01f
//#define SMOOTHING_LENGTH 0.0125f
#define SEARCH_RADIUS (1*SMOOTHING_LENGTH)
#define N_LOOPS 2
#define N_STEPS 2
#define CUBE_LEN_X 10
#define CUBE_LEN_Y 10
#define LEN_Z 20
#define N_PARTICLES (CUBE_LEN_X*CUBE_LEN_Y*LEN_Z)
#define INTERACTIONS_SIZE 200
#define TIME_STEP 0.005f
#define EPSILON 0.0001f
#define VISCOSITY 0.2f
#define STIFF 1.5f
//#define VISCOSITY 0.08f
#define MASS 0.00020543f
//#define MASS (1*0.00040543f) // 0.2g
//#define WATER_COLUMN
//#define ISO_THRESHOLD 2200.0f
#define ISO_THRESHOLD 700.0f
//#define ISO_RADIUS 0.0115f
//#define MC_GRID_LEN 0.005f

// 2000 particles
//#define ISO_RADIUS 0.0115f
#define ISO_RADIUS 0.0115f
#define MC_GRID_LEN 0.005f

// 1000 particles
//#define ISO_RADIUS (0.0115*0.457/0.363)
//#define MC_GRID_LEN 0.005f*0.457/0.363

// 4000 particles
//#define ISO_RADIUS (0.0115*0.288/0.363)
//#define MC_GRID_LEN 0.005f*0.288/0.363



#ifdef SHADER_EMBEDDED
#define MEDIA_DIR "media\\"
#else
//#define MEDIA_DIR "..\\media\\"
#define MEDIA_DIR "media\\"
#endif


// Cubemap texture files
char *faceFile[6] = {
	"mu_posx.bmp",
	"mu_negx.bmp",
	"mu_posy.bmp",
	"mu_negy.bmp",
	"mu_posz.bmp",
	"mu_negz.bmp",
};



const float radius = 0.002f;


#define RANDF() ((rand()/(GLfloat)RAND_MAX - 0.5f))

float azimuth = 0.0f; /** angle of direction **/
float zenith = 0.0f; /** angle of elevation **/

vector3* vdata;
float timestep = TIME_STEP;
int n_steps;


/** states for GUI **/
BOOL left_down = FALSE;
BOOL right_down = FALSE;
BOOL shift_down = FALSE;
int prev_x;
int prev_y;
BOOL saving = FALSE;
BOOL animated = FALSE;
int win_width = 640;
int win_height = 480;

/** variables for FPS **/
int n_frames = 0;
char fps_str[255];
int start_time = 0;

vector3 center;

// Index of each frame for capturing screen
int image_frame = 0;

matrix4 rot;
matrix4 rotview;


GLuint wood_tex;

gput_mat vol_mat;
gput_light light0;

// SPH simulation instance
cpu_sph cpu;
sph_render render;
gput_pbuffer cubemap_pbuf;
gput_pbuffer background_pbuf;

GLuint cubemap_tex;



enum
{
	SPH_POINT, SPH_VOLUME, SPH_NODRAW, SPH_SPHERE
} draw_mode;


void print_help()
{
	const char* msg = 
		"\n"\
		"Left-drag             : Control your view\n"\
		"Right-drag            : Translate container\n"\
		"Left-drag with SHIFT  : Rotate container\n"\
		"SPACE                 : Toggle animation\n"\
		"'t'                   : Switch timestep LARGE/SMALL\n"\
		"'m'                   : Switch rendering mode PARTICLES/SURFACE\n"\
		"'q'                   : Quit program\n";

	printf("%s\n", msg);
}



void set_projview(GLfloat fov)
{
	/* set viewpoint */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(fov, (GLfloat)win_width/(GLfloat)win_height, 0.03, 30.0);
//	gluPerspective(60, (GLfloat)win_width/(GLfloat)win_height, 0.03, 30.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	gluLookAt(
		0.0, 0.25, 0.0,
		0.0, 0.0, 0.0,
		0.0, 0.0, 1.0);
//	glScalef(2.0f, 2.0f, 2.0f);
	glScalef(1.5f, 1.5f, 1.5f);
	//glScalef(1.4f, 1.4f, 1.4f);

	glMultMatrixf((GLfloat*)&rotview);
}

/**
 * Initialize background pbuffer for refraction
 */
void set_background_projview()
{
	set_projview(90);
}

void init_light()
{
	gput_light_init(&light0);
//	gput_light_diffuse(&light0, 0.8f, 0.8f, 0.8f, 1.0f);
	gput_light_diffuse(&light0, 0.8f, 0.2f, 0.2f, 1.0f);
	gput_light_ambient(&light0, 0.3f, 0.3f, 0.3f, 1.0f);
	gput_light_position(&light0, 0.2f, 0.7f, 0.5f, 0.0f);
	gput_light_bind(&light0, GL_LIGHT0);
}


/**
 * Load and bind cube map textures from files
 */
void init_cubemap()
{
	char* faces[6];
	int i;

	for (i = 0; i < 6; i++)
	{
		faces[i] = malloc(strlen(faceFile[i]) + strlen(MEDIA_DIR) + 1);
		sprintf(faces[i], "%s%s", MEDIA_DIR, faceFile[i]);
	}

	cubemap_tex = gput_cubemap_create_from_file(faces);

	for (i = 0; i < 6; i++)
		free(faces[i]);
}




void draw_cuboid()
{
	//float h = 13;
	//float w = 5;

	float h = 14;
	float w = 6;

	//return;

	gput_mat_init(&vol_mat);
	gput_mat_diffuse(&vol_mat, 0.3f, 0.3f, 1.0f, 0.4f);
	gput_mat_specular(&vol_mat, 0.8f, 0.8f, 0.8f, 0.4f);
	gput_mat_shininess(&vol_mat, 22);
	gput_mat_bind(&vol_mat);
/*
	init_light();
	glEnable(GL_LIGHT0);
	glDisable(GL_LIGHT1);
	glEnable(GL_LIGHTING);
	glDisable(GL_TEXTURE_CUBE_MAP_ARB);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
*/
	glDisable(GL_LIGHTING);
	/*
	glActiveTexture(GL_TEXTURE1_ARB);
	glDisable(GL_TEXTURE_CUBE_MAP_ARB);
	glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0_ARB);
	glDisable(GL_TEXTURE_CUBE_MAP_ARB);
	glBindTexture(GL_TEXTURE_2D, wood_tex);
	glEnable(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
*/

	glEnable(GL_TEXTURE_2D);
	//glDisable(GL_TEXTURE_2D);


	glColor3f(1.0f, 1.0f, 1.0f);
	//gput_printf("cuboid\n");

	//glColor4f(1.0f, 0.5f, 0.5f, 0.0f);
	glDisable(GL_CULL_FACE);
	glPushMatrix();
	glTranslatef(cpu.c.x, cpu.c.y, cpu.c.z);
	glScalef(0.005f, 0.005f, 0.005f);
	glMultMatrixf((GLfloat*)&cpu.r_rot);
	glTranslatef(-w*0.5f, -w*0.5f, h*0.5f);
	glBegin(GL_QUADS);
	// Top
	glNormal3f(0, 0, 1);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 0);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(w, 0.0f, 0);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(w, w, 0);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f, w, 0);

	// Bottom
	glNormal3f(0, 0, -1);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, 0.0f, -h);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(w, 0.0f, -h);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(w, w, -h);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f, w, -h);

	// Right
	glNormal3f(1, 0.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(w, 0.0f, 0.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(w, w, 0.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(w, w, -h);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(w, 0.0f, -h);

	// Left
	glNormal3f(-1, 0, 0);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 0.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(0.0f, w, 0.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(0.0f, w, -h);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f, 0.0f, -h);

	// Near
	glNormal3f(0, 1, 0);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, w, 0.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(w, w, 0.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(w, w, -h);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f, w, -h);

	// Far
	glNormal3f(0, -1, 0);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 0.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(w, 0.0f, 0.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(w, 0.0f, -h);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f, 0.0f, -h);

	glEnd();
	glEnable(GL_CULL_FACE);
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);

	check_gl_error();
}

void draw_cube()
{

	GLfloat l;
	//float r;
	//vector3 a;
	//matrix4 m;
	l = 0.027f;

	//return;

	init_light();
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_TEXTURE_CUBE_MAP_ARB);
	glColor3f(1.0f, 1.0f, 1.0f);
	//vec3_scale(&cpu.r, 1.0f/vec3_len(&cpu.r), &cpu.r);

	glEnable(GL_LIGHTING);

	glPushMatrix();

	glTranslatef(cpu.c.x, cpu.c.y, cpu.c.z);
	glTranslatef(l*0.35f, l*0.35f, l*0.35f);
	//glTranslatef(l*0.35f, l*0.35f, l*0.35f);
	glMultMatrixf((GLfloat*)&cpu.r_rot);
	//glMultMatrixf(&m);
	glutSolidCube(l);
	glPopMatrix();

	glDisable(GL_LIGHTING);
}


void init_background_pbuffer()
{
	gput_pbuffer_create(&background_pbuf, GL_TEXTURE_2D, 512, 512,
		WGL_DRAW_TO_PBUFFER_ARB, GL_TRUE,
		WGL_BIND_TO_TEXTURE_RGBA_ARB, GL_TRUE,
		0,		
		WGL_TEXTURE_FORMAT_ARB, WGL_TEXTURE_RGBA_ARB,
		WGL_TEXTURE_TARGET_ARB, WGL_TEXTURE_2D_ARB,
		0);
	glBindTexture(GL_TEXTURE_2D, background_pbuf.tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
}

void render_to_background_tex()
{
	gput_context pushed;

	gput_context_get_current(&pushed);
	gput_pbuffer_activate(&background_pbuf);
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, cubemap_tex);
	//gput_cubemap_enable();

	glClearColor(1.0, 1.0, 1.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	//r = 1.5f;
	//glScalef(r, r, r);
	
	glEnable(GL_DEPTH_TEST);
	set_projview(60);
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);

	//glActiveTexture(GL_TEXTURE0_ARB);
	glBindTexture(GL_TEXTURE_2D, wood_tex);
	draw_cuboid();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glFlush();

	gput_context_activate(&pushed);
	check_gl_error();
}

void render_background()
{
	glDepthMask(GL_FALSE);
	glDisable(GL_LIGHTING);
	glColor3f(1.0f, 1.0f, 1.0f);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_TEXTURE);
	glPushMatrix();
	glLoadIdentity();


	glEnable(GL_TEXTURE_2D);
	gput_pbuffer_bind(&background_pbuf);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex2f(-1.0f, -1.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex2f(1.0f, -1.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex2f(1.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex2f(-1.0f, 1.0f);
	glEnd();

	glDepthMask(GL_TRUE);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glMatrixMode(GL_TEXTURE);
	glPopMatrix();
}


void transform_glass(const matrix4* m)
{
	cpu_sph_transform_obstacles(&cpu, m);
}

void translate_glass(const vector3* t)
{
	matrix4 m;

	mat4_set_translate(&m, t->x, t->y, t->z);
	transform_glass(&m);
}



void init_obstacles()
{
	vector3 tmp;

	vec3_set(&tmp, 0.0f, 0.0f, -0.0f);
	translate_glass(&tmp);
}


/**
 * Initialize the states of particles
 */
void init_particles()
{
	int i;
	int x;
	int y;
	int z;
	GLfloat s;
	GLfloat s2;
	GLfloat s3;
	GLfloat cx;
	GLfloat cy;
	GLfloat cz;

	vector3 pos[N_PARTICLES];
	vector3 vel[N_PARTICLES];

	s = 0.006f;

	cx = cy = 0.0f;
#ifdef SHADER_EMBEDDED
	cz = 0.065f;
#else
	cz = 0.035f;
#endif

#ifdef WATER_COLUMN
	cz = 0.08f;
	cx = 0.023f;
	cy = 0.023f;
#endif

	s2 = 0.001f;
	s3 = 0.0001f;
	mat4_set_identity(&rot);
	mat4_set_identity(&rotview);

	for (x = 0; x < CUBE_LEN_X; x++)
		for (y = 0; y < CUBE_LEN_Y; y++)
			for (z = 0; z < LEN_Z; z++)
			{
				i = x + y*CUBE_LEN_X + z*CUBE_LEN_X*CUBE_LEN_Y;
				vec3_set(&pos[i], s*(x -CUBE_LEN_X/2)- cx + s2*RANDF(), s*(y - CUBE_LEN_Y/2) - cy + s2*RANDF(), 0.8*s*z - cz);
				vec3_set(&vel[i], 0.0f, 0.0f, 0.0f);
			}

	cpu_sph_create(&cpu, N_PARTICLES, pos, vel, SMOOTHING_LENGTH, VISCOSITY, MASS, STIFF, SEARCH_RADIUS, N_LOOPS);

	cpu.n_particles = N_PARTICLES;

	printf("Finished initializing %d particles.\n", N_PARTICLES);
}



void init()
{
	if (glewInit() != GLEW_OK)
	{
		fprintf(stderr, "Could not initialize GLEW\n");
		exit(1);
	}

#ifdef SHADER_EMBEDDED
	if ((!GLEW_ARB_fragment_program) || (!GLEW_ARB_vertex_program))
	{
		gput_printf("GL_ARB_fragment_program, GL_ARB_vertex_program are not supported\n");
		exit(1);
	}
#endif
	gput_printf("Initialized GLEW %s\n", glewGetString(GLEW_VERSION));

	gput_init();
	sph_render_create(&render);
	n_steps = 0;


	vec3_set(&center, 0.0f, 0.0f, 0.0f);
	init_particles();
	init_obstacles();


	// Set the configuration such as grid resolution for Marching Cubes
	sph_render_set_config(&render, MC_GRID_LEN, 2, ISO_RADIUS);

	vdata = vec3_alloc(N_PARTICLES);

	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);

	gput_mat_init(&vol_mat);
	gput_mat_diffuse(&vol_mat, 0.3f, 0.3f, 1.0f, 0.4f);
	gput_mat_specular(&vol_mat, 0.8f, 0.8f, 0.8f, 0.4f);
	gput_mat_shininess(&vol_mat, 22);

	// Initialize background pbuffer for refraction
	init_background_pbuffer();

	glGenTextures(1, &wood_tex);
	glBindTexture(GL_TEXTURE_2D, wood_tex);
	gput_load_texture(GL_TEXTURE_2D, "wood03.bmp");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	//glClearColor(0.7, 0.7, 0.7, 0.0);

	draw_mode = SPH_VOLUME;
	animated = FALSE;

	init_cubemap();

	render_to_background_tex();


#ifdef SHADER_EMBEDDED
	print_help();
#endif
}


void reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}



void color_by_power(float p)
{
	if (p < 1100.0f)
	{
		glColor3f(0.0f, 0.0f, p/1100.0f);
	}
	else if (p < 1400.0f)
	{
		glColor3f(0.0f, p/1400.0f, p/1100.0f);
	}
	else if (p < 1900.0f)
	{
		glColor3f(p / 2100.0f, p/1500.0f, 0.0f);
	}
	else glColor3f(p / 2100.0f, 0.1f, 0.0f);
}



void render_fluid()
{
	int i;

	cpu_sph_get_pos(&cpu, vdata);

	glMatrixMode(GL_MODELVIEW);
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);

	glBindTexture(GL_TEXTURE_2D, wood_tex);
	draw_cuboid();
	//	draw_cube();

	// Render fluid as points
	if (draw_mode == SPH_POINT)
	{
		glDisable(GL_TEXTURE_2D);

		glPointSize(5.0);
		glDisable(GL_LIGHTING);
		glBegin(GL_POINTS);

		for (i = 0; i < cpu.n_particles; i++)
		{
			color_by_power(1.0f/cpu.density[i]);
			glVertex3fv((GLfloat*)&vdata[i]);
		}
		glEnd();
	}
	// Render fluid as volume
	else if (draw_mode == SPH_VOLUME)
	{
		matrix4 inv_rot;


		glDisable(GL_LIGHTING);

		glActiveTextureARB(GL_TEXTURE0_ARB);
		gput_pbuffer_bind(&background_pbuf);
		glEnable(GL_TEXTURE_2D);
		glActiveTextureARB(GL_TEXTURE1_ARB);
		glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, cubemap_tex);
		glActiveTextureARB(GL_TEXTURE0_ARB);

		glMatrixMode(GL_TEXTURE);
		glPushMatrix();
		glLoadIdentity();
		mat4_invert(&inv_rot, &rotview);
		glMultMatrixf((GLfloat*)&inv_rot);
		glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
		glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
		gput_cubemap_enable();


		sph_render_draw_surface(&render, ISO_THRESHOLD);
		
		gput_cubemap_disable();
		glMatrixMode(GL_TEXTURE);
		glPopMatrix();

	}
	else if (draw_mode == SPH_NODRAW)
	{
	}
	// Render fluid as a set of spheres
	else if (draw_mode == SPH_SPHERE)
	{
		glEnable(GL_CULL_FACE);
		glDisable(GL_TEXTURE_2D);
		gput_mat_bind(&vol_mat);
		glEnable(GL_LIGHTING);
		for (i = 0; i < cpu.n_particles; i++)
		{
			glPushMatrix();
			glTranslatef(vdata[i].x, vdata[i].y, vdata[i].z);
			switch (i % 4)
			{
			case 0:	gput_mat_diffuse(&vol_mat, 1.0f, 0.1f, 0.1f, 1.0f); break;
			case 1: gput_mat_diffuse(&vol_mat, 0.0f, 1.0f, 0.1f, 1.0f); break;
			case 2: gput_mat_diffuse(&vol_mat, 0.0f, 0.1f, 1.0f, 1.0f); break;
			case 3: gput_mat_diffuse(&vol_mat, 1.0f, 1.0f, 0.1f, 1.0f); break;
			}

			gput_mat_bind(&vol_mat);
			glutSolidSphere(radius, 4, 4);
			glPopMatrix();
		}
	}
}

void display(void)
{
	DWORD t;
	char buf[200];

	// Render scene without fluid to background texture for refraction
	render_to_background_tex();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	set_projview(60);
	gput_cubemap_skybox();

	render_fluid();

	glFinish();

	// For getting frame rate
	if (n_frames++ == 50)
	{
		t = GetTickCount();
		sprintf(fps_str, "%.2f fps", n_frames*1000.0f/(t - start_time));
		start_time = t;
		n_frames = 0;
#ifndef SHADER_EMBEDDED
		printf("%s\n", fps_str);
#endif
	}

#ifdef SHADER_EMBEDDED
	{
		sprintf(buf, "%d particles; %s; %s; %s", N_PARTICLES,
			(timestep < 0.005f) ? "SLOW" : "FAST",
			animated ? "animated" : "stopped",
			fps_str);
		glutSetWindowTitle(buf);
	}
#endif

	glutSwapBuffers();

	// Caputre each frame
	if (saving)
	{
		sprintf(buf, "images\\frame_%.4d.bmp", image_frame++);
		gput_write_screen(buf, win_width, win_height);
	}
}



void idle(void)
{
	char buf[200];
	int i;


	for (i = 0; i < N_STEPS; i++)
	{
		cpu_sph_elapse(&cpu, timestep);
	}
	
	n_steps++;
#ifndef SHADER_EMBEDDED
	sprintf(buf, "%d particles; %6.3f sec. elapsed; timestep=%f", N_PARTICLES, N_LOOPS*N_STEPS*timestep*n_steps, timestep);
	glutSetWindowTitle(buf);
#endif

	// Construct surface by Marching Cubes algorithm
	sph_render_create_implicit(&render,
                               MASS*315.0f/(64.0f*M_PI*pow(ISO_RADIUS, 9)),
                               &vdata[cpu.fluid_start],
                               cpu.n_fluidp,
                               &cpu.mat_inv_col,
                               &cpu);

	glutPostRedisplay();
}

void key(unsigned char key, int x, int y)
{
	switch(key)
	{
	case 'v':
		sph_render_create_implicit(&render, MASS*315.0f/(64.0f*M_PI*pow(ISO_RADIUS, 9)), &vdata[cpu.fluid_start], cpu.n_fluidp, &cpu.mat_inv_col, &cpu);
		glutPostRedisplay();
		break;


	case 'u':
		cpu_sph_init_rigid_body(&cpu);
		glutPostRedisplay();
		break;


#ifndef SHADER_EMBEDDED
	case 'g':
		saving = !saving;
		break;

	case 'e':
		cpu_sph_elapse(&cpu, timestep);
		glutPostRedisplay();

		break;

	case 'b':
		print_benchmark(&cpu, BENCHMARK_ITR, timestep);
		glutPostRedisplay();

		break;
#endif /* SHADER_EMBEDDED */



	case 't':
		if (timestep < 0.005f)
			timestep = 0.005f;
		else timestep = 0.004f;
		break;

	case ' ':
		if (animated)
		{
			glutIdleFunc(NULL);
			animated = FALSE;
		}
		else
		{
			animated = TRUE;
			glutIdleFunc(idle);
		}
		start_time = GetTickCount();
		n_frames = 0;
		glutPostRedisplay();
		break;


	case 'm':
#ifdef SHADER_EMBEDDED
		if (draw_mode == SPH_VOLUME)
			draw_mode = SPH_SPHERE;
		else draw_mode = SPH_VOLUME;
#else
		draw_mode++;
		if (draw_mode > SPH_SPHERE)
			draw_mode = SPH_POINT;
#endif
		glutPostRedisplay();
		break;

	case 'q':
		exit(0);
	}
}

void motion(int x, int y)
{
	int dx;
	int dy;
	float tx;
	float ty;
	const float THRESHOLD = 0.006f;

	if (left_down)
	{
		//printf("dragged\n");
		dx = x - prev_x;
		dy = y - prev_y;

		if (shift_down)
		{
			dx = max(-6, min(6, dx));
			dy = max(-6, min(6, dy));
			mat4_set_identity(&rot);
			if (abs(dx) > abs(dy))
				mat4_set_rotate(&rot, dx, 0.0f, 0.0f, 1.0f);
			else
				mat4_set_rotate(&rot, -dy, 1.0f, 0.0f, 0.0f);
			//assert(0);
			transform_glass(&rot);
		}
		else
		{
			if (abs(dx) > abs(dy))
			{
				azimuth += dx;
//				zenith += dx;
			}
			else
			{
				zenith -= dy;
			}
			mat4_set_rotate(&rotview, azimuth, 0.0f, 1.0f, 0.0f);
			mat4_mull_rotate(&rotview, zenith, 1.0f, 0.0f, 0.0f);
			glutPostRedisplay();
		}

		prev_x = x;
		prev_y = y;

	}
	else if (right_down)
	{
		matrix4 m;
		dx = x - prev_x;
		dy = y - prev_y;

		//gput_printf("dx=%d, dy=%d\n", dx, dy);

		tx = dx*0.0009f;
		ty = dy*0.0009f;
		tx = glg_clamp(tx, -THRESHOLD, THRESHOLD);
		ty = glg_clamp(ty, -THRESHOLD, THRESHOLD);

		//mat4_set_translate(&m, -tx, ty, 0);
		mat4_set_translate(&m, -tx, 0, -ty);
		transform_glass(&m);
		
		
		prev_x = x;
		prev_y = y;

		//glutPostRedisplay();
	}
}

void mouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN)
	{
		prev_x = x;
		prev_y = y;
		if (button == GLUT_LEFT_BUTTON)
			left_down = TRUE;
		else if (button == GLUT_RIGHT_BUTTON)
			right_down = TRUE;
	}
	else if (state == GLUT_UP)
	{
		if (button == GLUT_LEFT_BUTTON)
			left_down = FALSE;
		else if (button == GLUT_RIGHT_BUTTON)
			right_down = FALSE;
	}

	shift_down = glutGetModifiers() & GLUT_ACTIVE_SHIFT;
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(win_width, win_height);
	glutCreateWindow(argv[0]);

	printf("GL_VERSION: %s\n", (char *) glGetString(GL_VERSION));
	printf("GL_RENDERER: %s\n", (char *) glGetString(GL_RENDERER));
	printf("GL_VENDOR: %s\n", (char *) glGetString(GL_VENDOR));

	init();
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutKeyboardFunc(key);
	glutMainLoop();


	return 0;
}