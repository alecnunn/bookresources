/***********************************************
                                                                                
Demo for chapter "Gridless Controllable Fire"
in Games Programming Gems 5.
                                                                                
Author: Neeharika Adabala
                                                                                
Date: August 2004
                                                                                
************************************************/

/***********************************************
Minimum support to load textures in OpenGL
************************************************/

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <png.h>
#include <setjmp.h>
extern "C" {
	#include <jpeglib.h>
}

#include "texture.h"

Texture::Texture() {

}

Texture::Texture(const char *name,int flag,int texture_target) {
	load(name,flag,texture_target);
}

Texture::~Texture() {
	glDeleteTextures(1,&id);
	free(data);
}

void Texture::load(const char *name,int flag,int texture_target) {
	int width,height;
	data = load(name,&width,&height);
	if(!data) return;
	target = texture_target;
	glGenTextures(1,&id);
	glBindTexture(target,id);
	glTexParameteri(texture_target,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	if(flag & TRILINEAR) {
		glTexParameteri(target,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
	} else {
		glTexParameteri(target,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
	}
	if(flag & CLAMP) {
		glTexParameteri(target,GL_TEXTURE_WRAP_S,GL_CLAMP);
		glTexParameteri(target,GL_TEXTURE_WRAP_T,GL_CLAMP);
	} else if(flag & CLAMP_TO_EDGE) {
		glTexParameteri(target,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
		glTexParameteri(target,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
	} else {
		glTexParameteri(target,GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri(target,GL_TEXTURE_WRAP_T,GL_REPEAT);
	}
	if(flag & MIPMAP_SGIS) {
		glTexParameteri(target,GL_GENERATE_MIPMAP_SGIS,GL_TRUE);
		if(texture_target == TEXTURE_1D) {
			glTexImage1D(target,0,GL_RGBA,width,0,GL_RGBA,GL_UNSIGNED_BYTE,data);
		} else {
			glTexImage2D(target,0,GL_RGBA,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE,data);
		}
	} else {
		if(texture_target == TEXTURE_1D) {
			gluBuild1DMipmaps(target,GL_RGBA,width,GL_RGBA,GL_UNSIGNED_BYTE,data);
		} else {
			gluBuild2DMipmaps(target,GL_RGBA,width,height,GL_RGBA,GL_UNSIGNED_BYTE,data);
		}
	}
}

void Texture::enable() {
	glEnable(target);
}

void Texture::disable() {
	glDisable(target);
}

void Texture::bind() {
	glBindTexture(target,id);
}

unsigned char *Texture::load(const char *name,int *width,int *height) {
	char *ext = strrchr(name,'.');
	if(!ext) {
		fprintf(stderr,"unknown file format");
		return NULL;
	}
	unsigned char *data = NULL;
	if(!strcmp(ext,".png")) data = load_png(name,width,height);
	else if(!strcmp(ext,".jpg")) data = load_jpeg(name,width,height);
	return data;
}


/* load png image
 */
unsigned char *Texture::load_png(const char *name,int *width,int *height) {
	FILE *file = fopen(name,"rb");
	if(!file) {
		fprintf(stderr,"error open PNG file \"%s\"\n",name);
		return NULL;
	}
	png_byte sig[8];
	fread(sig,8,1,file);
	if(!png_check_sig(sig,8)) {
		fprintf(stderr,"error load png file \"%s\": wrong signature\n",name);
		fclose(file);
		return NULL;
	}
	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,0,0,0);
	if(!png_ptr) {
		fclose(file);
		return NULL;
	}
	png_infop info_ptr = png_create_info_struct(png_ptr);
	if(!info_ptr) {
		png_destroy_read_struct(&png_ptr,0,0);
		fclose(file);
		return NULL;
	}
	png_init_io(png_ptr,file);
	png_set_sig_bytes(png_ptr,8);
	png_read_info(png_ptr,info_ptr);
	unsigned long w,h;
	int bit_depth,color_type;
	png_get_IHDR(png_ptr,info_ptr,&w,&h,&bit_depth,&color_type,0,0,0);
	if(bit_depth == 16) png_set_strip_16(png_ptr);
	if(color_type == PNG_COLOR_TYPE_PALETTE) png_set_expand(png_ptr);
	if(bit_depth < 8) png_set_expand(png_ptr);
	if(png_get_valid(png_ptr,info_ptr,PNG_INFO_tRNS)) png_set_expand(png_ptr);
	if(color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA) png_set_gray_to_rgb(png_ptr);
	double gamma;
	if(png_get_gAMA(png_ptr,info_ptr,&gamma)) png_set_gamma(png_ptr,(double)2.2,gamma);
	png_read_update_info(png_ptr,info_ptr);
	png_get_IHDR(png_ptr,info_ptr,&w,&h,&bit_depth,&color_type,0,0,0);
	png_uint_32 row_bytes = png_get_rowbytes(png_ptr,info_ptr);
	png_uint_32 channels = png_get_channels(png_ptr,info_ptr);
	png_byte *img = (png_byte*)malloc(sizeof(png_byte) * row_bytes * h);
	png_byte **row = (png_byte**)malloc(sizeof(png_byte*) * h);
	for(int i = 0; i < (int)h; i++) row[i] = img + row_bytes * i;
	png_read_image(png_ptr,row);
	png_read_end(png_ptr,NULL);
	png_destroy_read_struct(&png_ptr,0,0);
	fclose(file);
	free(row);
	*width = w;
	*height = h;
	unsigned char *data = (unsigned char*)malloc(sizeof(unsigned char) * *width * *height * 4);
	unsigned char *ptr = data;
	for(int i = 0; i < *height; i++) {
		for(int j = 0; j < *width; j++) {
			int k = row_bytes * i + j * channels;
			*ptr++ = img[k + 0];
			*ptr++ = img[k + 1];
			*ptr++ = img[k + 2];
			if(channels == 4) *ptr++ = img[k + 3];
			else *ptr++ = 255;
		}
	}
	free(img);
	return data;
}

/* load jpeg image
 */
struct my_error_mgr {
	struct jpeg_error_mgr pub;
	jmp_buf setjmp_buffer;
};

typedef struct my_error_mgr *my_error_ptr;

static void my_error_exit(j_common_ptr cinfo) {
	my_error_ptr myerr = (my_error_ptr)cinfo->err;
	(*cinfo->err->output_message)(cinfo);
	longjmp(myerr->setjmp_buffer,1);
}

unsigned char *Texture::load_jpeg(const char *name,int *width,int *height) {
	struct jpeg_decompress_struct cinfo;
	struct my_error_mgr jerr;
	FILE *file;
	JSAMPARRAY buffer;
	int row_stride;
	long cont;
	JSAMPLE *data_buffer;
	int i,j;
	unsigned char *data;
	file = fopen(name,"rb");
	if(!file) {
		fprintf(stderr,"error open JPEG file \"%s\"\n",name);
		return NULL;
	}
	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;
	if(setjmp(jerr.setjmp_buffer)) {
		jpeg_destroy_decompress(&cinfo);
		fclose(file);
		return NULL;
	}
	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo,file);
	jpeg_read_header(&cinfo,TRUE);
	jpeg_start_decompress(&cinfo);
	row_stride = cinfo.output_width * cinfo.output_components;
	buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr)&cinfo,JPOOL_IMAGE,row_stride,1);
	data_buffer = (JSAMPLE*)malloc(cinfo.image_width * cinfo.image_height * cinfo.output_components);
	cont = 0;
	while(cinfo.output_scanline < cinfo.output_height) {
		jpeg_read_scanlines(&cinfo,buffer,1);
		memcpy(data_buffer + cinfo.image_width * cinfo.output_components * cont,buffer[0],row_stride);
		cont++;
	}
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	*width = cinfo.image_width;
	*height = cinfo.image_height;
	data = (unsigned char*)malloc(*width * *height * 4);
	switch(cinfo.output_components) {
		case 1:
			for(i = 0, j = 0; i < *width * *height; i++, j += 4) {
				data[j] = data[j + 1] = data[j + 2] = data_buffer[i];
				data[j + 3] = 255;
			}
			break;
		case 3:
			for(i = 0, j = 0; i < *width * *height * 3; i += 3, j += 4) {
				data[j] = data_buffer[i];
				data[j + 1] = data_buffer[i + 1];
				data[j + 2] = data_buffer[i + 2];
				data[j + 3] = 255;
			}
			break;
		default:
			free(data);
			free(data_buffer);
			return NULL;
	}
	free(data_buffer);
	fclose(file);
	return data;
}
