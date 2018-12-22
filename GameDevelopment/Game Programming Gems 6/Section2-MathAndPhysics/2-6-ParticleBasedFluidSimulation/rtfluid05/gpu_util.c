/**
 * Uitility functions for OpenGL
 *
 * Copyright (c) Takashi AMADA  All Rights Reseved
 */

#include "gpu_util.h"

#include <windows.h>
#include <GL/glut.h>

//#define _USE_DEVIL_
#ifdef _USE_DEVIL_
#include <il/il.h>
#endif

#define MAX_FORMATS 1

#ifndef NO_NVCG
static CGprogram passthru;
static CGcontext cg_context;
#endif

void gput_init()
{
#ifndef NO_NVCG
	cg_context = cgCreateContext();
#endif
#ifdef _USE_DEVIL_
	ilInit();
#endif
}

#ifndef NO_NVCG
CGcontext gput_cg_context()
{
	return cg_context;
}
#endif

/**
 * Print GPU profile
 */
void gput_print_gpu_profile()
{
	GLint i;

	glGetIntegerv(GL_MAX_TEXTURE_UNITS, &i);
	gput_printf("GL_MAX_TEXTURE_UNITS = %d\n", i);
}

void gput_mat_init(gput_mat* mat)
{
}

void gput_mat_diffuse(gput_mat* mat, GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
	mat->diffuse[0] = r;
	mat->diffuse[1] = g;
	mat->diffuse[2] = b;
	mat->diffuse[3] = a;
}

void gput_mat_specular(gput_mat* mat, GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
	mat->specular[0] = r;
	mat->specular[1] = g;
	mat->specular[2] = b;
	mat->specular[3] = a;
}

void gput_mat_shininess(gput_mat* mat, GLfloat s)
{
	mat->shininess = s;
}

void gput_mat_bind(gput_mat* mat)
{
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat->diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat->specular);
	glMaterialf(GL_FRONT, GL_SHININESS, mat->shininess);
}

void gput_light_init(gput_light* l)
{
}

void gput_light_position(gput_light* l, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
	l->position[0] = x;
	l->position[1] = y;
	l->position[2] = z;
	l->position[3] = w;
}

void gput_light_diffuse(gput_light* l, GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
	l->diffuse[0] = r;
	l->diffuse[1] = g;
	l->diffuse[2] = b;
	l->diffuse[3] = a;
}

void gput_light_ambient(gput_light* l, GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
	l->ambient[0] = r;
	l->ambient[1] = g;
	l->ambient[2] = b;
	l->ambient[3] = a;
}

void gput_light_bind(gput_light* l, GLenum light)
{
	glLightfv(light, GL_POSITION, l->position);
	glLightfv(light, GL_DIFFUSE, l->diffuse);
	glLightfv(light, GL_AMBIENT, l->ambient);
}


void gput_write_screen(const char* filename, int width, int height)
{
#ifdef _USE_DEVIL_
	char* pixels;
	ILuint image0;

	pixels = (char*)malloc(3*width*height*sizeof(char));
	glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);

	ilGenImages(1, &image0);
	ilBindImage(image0);
	ilTexImage(width, height, 1, 3, IL_RGB, IL_UNSIGNED_BYTE, pixels);
	ilSetData(pixels);
	ilSaveImage(filename);
	ilDeleteImages(1, &image0);
	free(pixels);
#endif
}

GPUT_BOOL gput_load_texture(GLenum target, const char* filename)
{
	HANDLE hFile;
	DWORD read;
	DWORD fileSizeHigh;
	DWORD fileSize;
	char* buf;

//	char* buf2;
//	int i;

	BITMAPFILEHEADER* pBMFH;
	BITMAPINFOHEADER* pBMIH;

	hFile = CreateFile(filename,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);
	
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return GPUT_FALSE;
	}

	fileSize = GetFileSize(hFile, &fileSizeHigh);
	buf = (char*)malloc(fileSize*sizeof(char));//new char[fileSize];

	if (!ReadFile(hFile, buf, fileSize, &read, NULL))
	{
		return GPUT_FALSE;
	}

	pBMFH = (BITMAPFILEHEADER*)buf;
	pBMIH = (BITMAPINFOHEADER*)(buf + sizeof(BITMAPFILEHEADER));

	buf += (fileSize - 3*pBMIH->biWidth*pBMIH->biHeight);
/*
	buf2 = (char*)malloc(3*pBMIH->biWidth*pBMIH->biHeight);

	for (i = 3*pBMIH->biWidth*pBMIH->biHeight - 1; i >= 0; i--)
	{
		buf2[i] = buf[3*pBMIH->biWidth*pBMIH->biHeight - i - 1];
	}*/
/*
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);*/
//	glTexImage2D(GL_TEXTURE_2D, 0, 3, tex_width, tex_height, 0, GL_RGB, GL_UNSIGNED_BYTE, pix);
	glTexImage2D(target, 0, 3, pBMIH->biWidth, pBMIH->biHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, buf);

	//free(buf2);

//	memcpy(pixels, buf, 3*pBMIH->biWidth*pBMIH->biHeight);

	buf -= (fileSize - 3*pBMIH->biWidth*pBMIH->biHeight);

	CloseHandle(hFile);
	free(buf);

	return GPUT_TRUE;
}

void gput_profile_init(gput_profile* p)
{
	p->start = 0;
	p->elapsed = 0;
	p->count = 0;
}

void gput_profile_begin(gput_profile* p)
{
	p->start = GetTickCount();
	p->count++;
}

void gput_profile_end(gput_profile* p)
{
	DWORD elapsed;
	glFinish();
	elapsed = GetTickCount() - p->start;
	p->elapsed += elapsed;
	//gput_printf("%s: %d msec. elapsed.\n", name, GetTickCount() - bm_start);
}

void gput_profile_print(gput_profile* p, const char* name)
{
	gput_printf("%s: %d msec. elapsed.\n", name, p->elapsed/p->count);
}

void check_gl_error(void)
{
	GLenum err;
	err = glGetError();
	if (err != GL_NO_ERROR)
	{
		gput_printf("GL Error: %x\n", err);
		assert(0);
	}
}

#ifndef NO_NVCG
void check_cg_error(void)
{
	CGerror err = cgGetError();

	if (err != CG_NO_ERROR)
	{
		gput_printf("Cg Error: %s\n", cgGetErrorString(err));
		assert(0);
		//exit(1);
	}
}


CGprogram gput_create_program_from_file(const char* program_file, CGprofile profile, CGcontext context)
{
	CGprogram new_fp;

	new_fp = cgCreateProgramFromFile(context,
		CG_SOURCE, program_file,
		profile,
		NULL, NULL);

	cgGLLoadProgram(new_fp);

	check_cg_error();

	return new_fp;
}

void gput_cache_program(CGprogram prog, CGprofile prof)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	cgGLEnableProfile(prof);
	cgGLBindProgram(prog);
	glBegin(GL_POINTS);
	glVertex2i(0, 0);
	glEnd();
	cgGLDisableProfile(prof);
}

CGprogram gput_create_program(const char* src, CGprofile profile, CGcontext context)
{
	CGprogram new_fp;

	new_fp = cgCreateProgram(context,
		CG_SOURCE, src,
		profile,
		NULL, NULL);

//	cgCompileProgram(new_fp);
	cgGLLoadProgram(new_fp);

	/*
	cgGLEnableProfile(profile);
	cgGLBindProgram(new_fp);
	glBegin(GL_POINTS);
	glVertex2i(0, 0);
	glEnd();
	cgGLDisableProfile(profile);
	*/

	check_cg_error();

	return new_fp;
}
#endif

GLuint gput_cubemap_create_from_file(const char* filename[6])
{
	int i;
	GLuint tex;

	static GLenum targets[6] = {
		GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB
	};

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, tex);

	for (i = 0; i < 6; i++)
		if (!gput_load_texture(targets[i], filename[i]))
			gput_printf("Cannot load texture '%s'\n", filename[i]);

	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return tex;
}

void gput_cubemap_enable()
//void gput_cubemap_enable(GLuint tex)
{
	GLint mode;

//	glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, map->tex);

	glEnable(GL_TEXTURE_CUBE_MAP_ARB);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glEnable(GL_TEXTURE_GEN_R);

	//mode = GL_NORMAL_MAP_EXT;
	mode = GL_REFLECTION_MAP_ARB;

	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, mode);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, mode);
	glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, mode);
}

//void gput_cubemap_disable()
void gput_cubemap_disable()
{
	glDisable(GL_TEXTURE_CUBE_MAP_ARB);
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_GEN_R);
}


void gput_cubemap_skybox()
{
    GLfloat s_plane[] = { 1.0, 0.0, 0.0, 0.0 };
    GLfloat t_plane[] = { 0.0, 1.0, 0.0, 0.0 };
    GLfloat r_plane[] = { 0.0, 0.0, 1.0, 0.0 };

	glEnable(GL_TEXTURE_CUBE_MAP_ARB);
	
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glEnable(GL_TEXTURE_GEN_R);

	glMatrixMode(GL_TEXTURE);
	glPushMatrix();
	glLoadIdentity();
	glTexGenfv(GL_S, GL_EYE_PLANE, s_plane);
    glTexGenfv(GL_T, GL_EYE_PLANE, t_plane);
    glTexGenfv(GL_R, GL_EYE_PLANE, r_plane);

	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glDisable(GL_CULL_FACE);
	glutSolidCube(2.0);
	glEnable(GL_CULL_FACE);

	glDisable(GL_TEXTURE_CUBE_MAP_ARB);
	glMatrixMode(GL_TEXTURE);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_GEN_R);
}

void gput_vert2i_tex1f(int vx, int vy, GLfloat tx)
{
	glMultiTexCoord1fARB(GL_TEXTURE0_ARB, tx);
	glVertex2i(vx, vy);
}

void gput_vert2i_tex2f(int vx, int vy,
					   GLfloat tx, GLfloat ty)
{
	glMultiTexCoord2fARB(GL_TEXTURE0_ARB, tx, ty);
	glVertex2i(vx, vy);
}

void gput_vert2i_2tex2f(int vx, int vy,
						GLfloat x0, GLfloat y0, 
						GLfloat x1, GLfloat y1)
{
	glMultiTexCoord2fARB(GL_TEXTURE0_ARB, x0, y0);
	glMultiTexCoord2fARB(GL_TEXTURE1_ARB, x1, y1);
	glVertex2i(vx, vy);
}


void gput_vert2i_3tex2f(int vx, int vy,
						GLfloat x0, GLfloat y0, 
						GLfloat x1, GLfloat y1,
						GLfloat x2, GLfloat y2)
{
	glMultiTexCoord2fARB(GL_TEXTURE0_ARB, x0, y0);
	glMultiTexCoord2fARB(GL_TEXTURE1_ARB, x1, y1);
	glMultiTexCoord2fARB(GL_TEXTURE2_ARB, x2, y2);
	glVertex2i(vx, vy);
}


void gput_set_orthorenderview(int x, int y, int view_width, int view_height)
{
	glViewport(x, y, view_width, view_height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, view_width, 0.0, view_height);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void gput_dump_frame(const char* name, int n_components, int x0, int y0, int width, int height)
{
	GLfloat* buf;
	int x;
	int y;
	int index;

	gput_printf("pbufferf_dump: you should fix\n");

	/*
	buf = (GLfloat*)malloc(pbuf->width*pbuf->height*4*sizeof(GLfloat));
	glReadPixels(0, 0, pbuf->width, pbuf->height, GL_RGBA, GL_FLOAT, buf);
	*/

	buf = (GLfloat*)malloc(width*height*4*sizeof(GLfloat));
	glReadPixels(x0, y0, width, height, GL_RGBA, GL_FLOAT, buf);
	gput_printf("NAME: %s -------------------\n", name);

	for (y = 0; y < height; y++)
		for (x = 0; x < width; x++)
		{
			index = 4*(x + y*width);
			switch (n_components)
			{
			case 1:
				gput_printf("(%d, %d); x=%e\n", 
					x + x0, y + y0, buf[index]);
				break;
			case 2:
				gput_printf("(%d, %d); x=%e, y=%e\n", 
					x + x0, y + y0, buf[index], buf[index + 1]);
				break;
			case 3:
				gput_printf("(%d, %d); x=%.4f, y=%.4f, z=%.4f\n", 
					x + x0, y + y0, buf[index], buf[index + 1], buf[index + 2]);
				break;
			case 4:
				gput_printf("(%d, %d); x=%e, y=%e, z=%e w=%e\n", 
					x + x0, y + y0, buf[index], buf[index + 1], buf[index + 2], buf[index + 3]);
				break;
			}
		}

	check_gl_error();

	//gput_pbuffer_end(pbuf);
	free(buf);
}


void DrawQuad(int w, int h, int tw, int th)
{
    glBegin(GL_QUADS);
    glTexCoord2f(0,         0);         glVertex2f(0,        0);
    glTexCoord2f((float)tw, 0);         glVertex2f((float)w, 0);
    glTexCoord2f((float)tw, (float)th); glVertex2f((float)w, (float) h);
    glTexCoord2f(0,         (float)th); glVertex2f(0,        (float) h);
    glEnd();
}



#ifndef NO_NVCG
void gput_set_transferfp(CGprogram fp)
{
	passthru = fp;
}
#endif

void gput_pbuffer_float_set(gput_pbuffer* buf, int width, int height, void* pixels)
{
	gput_context c;

	gput_context_get_current(&c);
	gput_pbuffer_activate(buf);
	

#ifndef NO_NVCG
	cgGLBindProgram(passthru);
	cgGLEnableProfile(CG_PROFILE_FP30);
	glActiveTextureARB(GL_TEXTURE0_ARB);
    glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_FLOAT_RGB_NV, width, height, 0, GL_RGB, GL_FLOAT, pixels);
    DrawQuad(width, height, width, height);
    cgGLDisableProfile(CG_PROFILE_FP30);
#endif
	//gput_pbuffer_end(buf);

	gput_context_activate(&c);
}






GLuint gput_create_float_buffer(GLint internalformat, GLsizei width, GLsizei height, GLenum format, void* pixels)
{
	GLuint tex;

	glGenTextures(1, &tex);
//	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, tex);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, tex);

	glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, internalformat,
		width, height, 0, format, GL_FLOAT, pixels);

	/*
	glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_FLOAT_R32_NV,
		width, height, 0, GL_RGB, GL_FLOAT, pixels);
*/
	check_gl_error();
	
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP);

	return tex;
}

void gput_bind_float_buffer(GLuint tex)
{
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, tex);
}


void gput_intlist_create(gput_intlist* l)
{
	l->size = 0;
	l->cap = 10;
	l->array = (GLint*)malloc(l->cap*sizeof(GLint));
}

void gput_intlist_pushpair(gput_intlist* l, GLint i0, GLint i1)
{
	if (l->cap < (l->size + 2))
	{
		GLint* newarray;

		newarray = (GLint*)malloc(2*l->cap*sizeof(GLint));
		l->cap *= 2;
		memcpy(newarray, l->array, l->size*sizeof(GLint));
		free(l->array);
		l->array = newarray;
	}

	l->array[l->size] = i0;
	l->array[l->size + 1] = i1;
	l->size += 2;
}

void gput_intlist_push(gput_intlist* l, GLint i)
{
	if (l->cap == l->size)
	{
		GLint* newarray;

		newarray = (GLint*)malloc(2*l->cap*sizeof(GLint));
		l->cap *= 2;
		memcpy(newarray, l->array, l->size*sizeof(GLint));
		free(l->array);
		l->array = newarray;
	}

	l->array[l->size] = i;
	l->size++;
}


void gput_intlist_destroy(gput_intlist* l)
{
	free(l->array);
}

void gput_pbuffer_create_array(gput_pbuffer* buf, GLenum target, int width, int height,
							   const GLint* iattr, const GLint* pattr)
{
	unsigned int pformats[MAX_FORMATS];
	GLuint nformat;
	HDC hdc;

	const float fattriblist[] =
	{
		0
	};

	hdc = wglGetCurrentDC();

	if (!hdc) 
	{
		gput_printf("unable to get current device context\n");
		return;
	}

	if (!wglChoosePixelFormatARB(hdc, iattr, fattriblist, MAX_FORMATS, pformats, &nformat)) 
	{
		gput_printf("can't choose pixel format\n");
		return;
	}
	

	buf->hpbuf = wglCreatePbufferARB(hdc, pformats[0], width, height, pattr);
	
	if (!buf->hpbuf) {
		gput_printf("can't create pbuffer\n");
		return;
	}

	buf->hdc = wglGetPbufferDCARB(buf->hpbuf);
	if (!buf->hdc) {
		gput_printf("can't get pbuffer device context.\n");
		return;
	}

	buf->hglrc = wglCreateContext(buf->hdc);
	if (!buf->hglrc) {
		gput_printf("can't create context for pbuffer.\n");
		return;
	}

	/* Share OpenGL context between pbuffer & window */
	wglShareLists(wglGetCurrentContext(), buf->hglrc);
	//buf->hglrc = wglGetCurrentContext();

	buf->target = target;

	//wglGetLastError();
	check_gl_error();
}

void gput_pbuffer_cubemap_create(gput_pbuffer* buf, int width, int height)
{
	GLint iattr[] = {
		WGL_DRAW_TO_PBUFFER_ARB, GL_TRUE,
		WGL_BIND_TO_TEXTURE_RGB_ARB, GL_TRUE,
		WGL_DOUBLE_BUFFER_ARB, GL_FALSE,
		0
	};
	GLint pattr[] = {
		WGL_TEXTURE_FORMAT_ARB, WGL_TEXTURE_RGB_ARB,
		WGL_TEXTURE_TARGET_ARB, WGL_TEXTURE_CUBE_MAP_ARB,
		0
	};

	gput_pbuffer_create_array(buf, GL_TEXTURE_CUBE_MAP_ARB, width, height, iattr, pattr);
	glGenTextures(1, &buf->tex);
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, buf->tex);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void gput_pbuffer_create(gput_pbuffer* buf, GLenum target, int width, int height, ...)
{
#define MAX_PBUF_PARAM 10
	va_list list;
	int n_attr;
	int count;
	GLint iattr[2*MAX_PBUF_PARAM + 1];
	GLint pattr[2*MAX_PBUF_PARAM + 1];
	GLint* attr;

	n_attr = 0;
	count = 0;
	attr = iattr;
	va_start(list, height);
	for (;;)
	{
		GLint type;
		GLint value;
		
		type = va_arg(list, GLint);
		if (type == 0)
		{
			attr[2*count] = 0;
			if (n_attr == 0)
			{
				n_attr++;
				count = 0;
				attr = pattr;
				continue;
			}
			else break;
		}
		value = va_arg(list, GLint);
		attr[2*count]     = type;
		attr[2*count + 1] = value;

		count++;
		if (count == MAX_PBUF_PARAM)
		{
			gput_printf("gput_pbuffer_create: Too many attributes\n");
			assert(0);
		}
	}
	va_end(list);

	gput_pbuffer_create_array(buf, target, width, height, iattr, pattr);
}
			


void gput_context_get_current(gput_context* c)
{
	c->hdc = wglGetCurrentDC();
	c->hglrc = wglGetCurrentContext();
}

void gput_context_activate(gput_context* c)
{
	if (!wglMakeCurrent(c->hdc, c->hglrc))
	{
		gput_printf("gput_context_activate: Cannot switch OpenGL context\n");
		assert(0);
	}
}


void gput_pbuffer_activate(gput_pbuffer* buf)
{
	//buf->windowid = glutGetWindow();
	
	/*
	if (!wglReleaseTexImageARB(buf->hpbuf, WGL_FRONT_LEFT_ARB))
	{
		gput_printf("pbuffer_begin: Cannot release\n");
		return;
	}*/
	
	if (!wglMakeCurrent(buf->hdc, buf->hglrc))
		assert(0);
}

/*
void gput_pbuffer_end(gput_pbuffer* buf)
{
	glutSetWindow(buf->windowid);
	//wglMakeCurrent(
}
*/

void gput_pbuffer_bind(gput_pbuffer* pbuf)
{
	glBindTexture(pbuf->target, pbuf->tex);
	//glBindTexture(target, pbuf->tex);
	/*
	if (!wglReleaseTexImageARB(pbuf->hpbuf, WGL_FRONT_LEFT_ARB))
	{
		gput_printf("pbuffer_bind: Cannot release\n");
		return;
	}*/
	wglBindTexImageARB(pbuf->hpbuf, WGL_FRONT_LEFT_ARB);
	/*

	if (!wglBindTexImageARB(pbuf->hpbuf, WGL_FRONT_LEFT_ARB))
	{
		gput_printf("cannot bind\n");
		return;
	}*/
}


void gput_pbuffer_float_create(gput_pbuffer* buf, int n_components, int width, int height)
{
	unsigned int pformats[MAX_FORMATS];
	GLuint nformat;
	HDC hdc;
	gput_context c;

	int iattriblist[] = 
	{
		WGL_DRAW_TO_PBUFFER_ARB, GL_TRUE,
		WGL_BIND_TO_TEXTURE_RECTANGLE_FLOAT_R_NV, 1,
		WGL_FLOAT_COMPONENTS_NV, GL_TRUE,
		WGL_COLOR_BITS_ARB, 32,
		0
	};

	const float fattriblist[] =
	{
		0
	};

	int pattriblist[] = 
	{
		WGL_TEXTURE_FORMAT_ARB, WGL_TEXTURE_FLOAT_R_NV,
		WGL_TEXTURE_TARGET_ARB, WGL_TEXTURE_RECTANGLE_NV,
		0
	};

	hdc = wglGetCurrentDC();

	if (!hdc) 
	{
		gput_printf("unable to get current device context\n");
		return;
	}

	switch (n_components)
	{
	case 1:
		iattriblist[2] = WGL_BIND_TO_TEXTURE_RECTANGLE_FLOAT_R_NV;
		iattriblist[7] = 32;
		pattriblist[1] = WGL_TEXTURE_FLOAT_R_NV;
		break;
	case 2:
		iattriblist[2] = WGL_BIND_TO_TEXTURE_RECTANGLE_FLOAT_RG_NV;
		iattriblist[7] = 64;
		pattriblist[1] = WGL_TEXTURE_FLOAT_RG_NV;
		break;
	case 3:
		iattriblist[2] = WGL_BIND_TO_TEXTURE_RECTANGLE_FLOAT_RGB_NV;
		iattriblist[7] = 96;
		pattriblist[1] = WGL_TEXTURE_FLOAT_RGB_NV;
		break;
	case 4:
		iattriblist[2] = WGL_BIND_TO_TEXTURE_RECTANGLE_FLOAT_RGBA_NV;
		iattriblist[7] = 128;
		pattriblist[1] = WGL_TEXTURE_FLOAT_RGBA_NV;
		break;
	default:
		assert(0);
	}

	if (!wglChoosePixelFormatARB(hdc, iattriblist, fattriblist, MAX_FORMATS, pformats, &nformat)) 
	{
		gput_printf("can't choose pixel format\n");
		return;
	}

	buf->hpbuf = wglCreatePbufferARB(hdc, pformats[0],
		width, height, pattriblist);
	if (!buf->hpbuf) {
		gput_printf("can't create pbuffer\n");
		return;
	}

	buf->hdc = wglGetPbufferDCARB(buf->hpbuf);
	if (!buf->hdc) {
		gput_printf("can't get pbuffer device context.\n");
		return;
	}

	buf->hglrc = wglCreateContext(buf->hdc);
	if (!buf->hglrc) {
		gput_printf("can't create context for pbuffer.\n");
		return;
	}

	wglShareLists(wglGetCurrentContext(), buf->hglrc);

	buf->target = GL_TEXTURE_RECTANGLE_ARB;

	/* create the pbuffer texture object. */
	glGenTextures(1, &buf->tex);

	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, buf->tex);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP);	

    /* EBUG HERE */


	{
		//gput_context c;

		gput_context_get_current(&c);
		gput_pbuffer_activate(buf);
		gput_set_orthorenderview(0, 0, width, height);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#ifndef NO_NVCG
		cgGLBindProgram(passthru);
		cgGLEnableProfile(CG_PROFILE_FP30);
		glActiveTextureARB(GL_TEXTURE0_ARB);
		glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_FLOAT_RGB_NV, width, height, 0, GL_RGB, GL_FLOAT, NULL);
		DrawQuad(width, height, width, height);
		cgGLDisableProfile(CG_PROFILE_FP30);
		//gput_pbuffer_end(buf);
#endif
		gput_context_activate(&c);
	}
	//gput_pbuffer_float_set(buf, width, height, NULL);
	//glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_FLOAT_RGB_NV, width, height, 0, GL_RGB, GL_FLOAT, NULL);

}
