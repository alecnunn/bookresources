//----------------------------------------------------------------------------
//
//cs_am3d.cpp
//
//Copyright © Rishi Ramraj, 2004
//----------------------------------------------------------------------------

//··········································································//
//    Source :: Inclusions
//··········································································//

#include "cs_am3d.h"
#include <stdio.h>
#include "texture.h"
#include "mth_vector.h"

//··········································································//
//    Source :: Definitions
//··········································································//

//··········································································//
//    Source :: Structures
//··········································································//

//··········································································//
//    Source :: Class Defs
//··········································································//

//··········································································//
//    Source :: Private Function Defs
//··········································································//

//··········································································//
//    Source :: Private Vars
//··········································································//

//··········································································//
//    Source :: External Function Implementation
//··········································································//

//··········································································//
//    Source :: External Class Implementation
//··········································································//

cs_am3d_c::cs_am3d_c(void)
{
	active = false;

	num_vertices = 0;
	vertices = NULL;

	num_triangles = 0;
	triangles = NULL;

	num_groups = 0;
	groups = NULL;

	num_textures = 0;
	textures = NULL;

	num_joints = 0;
	joints = NULL;
}

cs_am3d_c::~cs_am3d_c(void)
{	
	int loop;
	
	delete[] vertices;		
	delete[] triangles;
	delete[] joints;
	
	for(loop = 0; loop < num_groups; loop ++)
	{
		delete[] groups[loop].triangleIndices;
	}		
	delete[] groups;

	for(loop = 0; loop < num_textures; loop++)
	{
		glDeleteTextures(1,&textures[loop].texture);
		glDeleteTextures(1,&textures[loop].alpha);
	}
	delete[] textures;
}

bool cs_am3d_c::load(const char * path)
{
	unload();		
	
	FILE* file;
	int loop;

	//Open the .am3d model file
	if((file= fopen(path, "rb"))==NULL)
		return false;	

	//Read the vertex data in
	fread(&num_vertices, sizeof(word), 1, file);

	vertices = new am3d_vertex_t [num_vertices];
	if(vertices == NULL)
		return false;

	for(loop=0; loop<num_vertices; loop++)
	{
		fread( &vertices[loop].selected, sizeof(bool), 1, file);
		fread( &vertices[loop].boneID, sizeof(char), 1, file);
		fread( vertices[loop].vertex, sizeof(float), 3, file);		
	}	

	//Read the triangle data in
	fread(&num_triangles, sizeof(word), 1, file);

	triangles = new am3d_triangle_t [num_triangles];
	if(triangles == NULL)
		return false;

	for(loop=0; loop<num_triangles; loop++)
	{
		fread( &triangles[loop].selected,sizeof(bool),1,file);
		fread( triangles[loop].vertexIndices, sizeof(word),3,file);
		fread( triangles[loop].vertexNormals[0],sizeof(float),3,file);
		fread( triangles[loop].vertexNormals[1],sizeof(float),3,file);
		fread( triangles[loop].vertexNormals[2],sizeof(float),3,file);
		fread( triangles[loop].s,sizeof(float),3,file);
		fread( triangles[loop].t,sizeof(float),3,file);		
	}
	
	//Read the group data in
	fread(&num_groups, sizeof(word), 1, file);
	fread(&grp_solid, sizeof(word), 1, file);
	fread(&grp_textured, sizeof(word), 1, file);
	fread(&grp_masked, sizeof(word), 1, file);

	groups= new am3d_group_t [num_groups];
	if(groups == NULL)
		return false;
	
	for(loop=0; loop<num_groups; loop++)
	{
		fread( &groups[loop].selected, sizeof(bool), 1, file);
		fread(&groups[loop].numtriangles,	 sizeof(word), 1,	 file);
		groups[loop].triangleIndices= new word [groups[loop].numtriangles];
		fread( groups[loop].triangleIndices, sizeof(word), groups[loop].numtriangles,file);
		fread(&groups[loop].materialIndex,	 sizeof(char), 1, file);
	}

	//then the textures
	
	fread(&num_textures, sizeof(word), 1, file);
	if(num_textures == 0)
		return true;

	textures = new am3d_material_t [num_textures];
	if(textures == NULL)
		return false;

	am3d_material_path_t* paths;
	paths = new am3d_material_path_t [num_textures];
	if(paths == NULL)
		return false;

	for(loop=0; loop<num_textures; loop++)
	{
		fread(paths[loop].TexName,	sizeof(char), 128, file);
		fread(paths[loop].AlphaName, sizeof(char), 128, file);
	}

	if(fclose(file)!=0)
		return false;
	
	//load textures based on relative format

	char pathTemp[256], * pdest;
	int pathLength;
    if(pdest = strrchr( path, '/' ))
	{
		pathLength = pdest - path + 1;
	}
	else if(pdest = strrchr( path, '\\' ))
	{
		pathLength = pdest - path + 1;
	}
	else
	{
		pathLength = 0;
	}
	strncpy( pathTemp, path, pathLength );

	int loop1;
	bool success;

	if(paths)
	{
		for(loop = 0; loop < num_textures; loop++)
		{
			//check for texture redundancy
			success = false;
			for(loop1 = 0; loop1 < loop; loop1 ++)
			{
				if(!strcmp(paths[loop].TexName,paths[loop1].TexName))
				{
					textures[loop].texture = textures[loop1].texture;
					success = true;
				}
			}

			//The texture
			if(!success)
			{
				strcpy( pathTemp + pathLength, paths[loop].TexName + 1 );
			
				if(!ipic_build_tex(pathTemp,textures[loop].texture))
					textures[loop].texture = 0;
			}
			
			//The alpha map
			if(strchr(paths[loop].AlphaName,'.'))
			{
				//check for alpha redundancy
				success = false;
				for(loop1 = 0; loop1 < loop; loop1 ++)
				{
					if(!strcmp(paths[loop].AlphaName,paths[loop1].AlphaName))
					{
						textures[loop].alpha = textures[loop1].alpha;
						success = true;
					}
				}

				if(!success)
				{
					strcpy( pathTemp + pathLength, paths[loop].AlphaName + 1 );
				
					if(!ipic_build_tex(pathTemp,textures[loop].alpha))
						textures[loop].alpha = 0;
				}
			}
			else
				textures[loop].alpha = 0;			
		}
	}

	active = true;
	
	return true;
}

void cs_am3d_c::unload(void)
{
	if(active)
	{
		int loop1;
		
		delete[] vertices;		
		delete[] triangles;
		delete[] joints;
		
		for(loop1 = 0; loop1 < num_groups; loop1 ++)
		{
			delete [] groups[loop1].triangleIndices;
		}		
		delete[] groups;

		for(loop1 = 0; loop1 < num_textures; loop1++)
		{
			glDeleteTextures(1,&textures[loop1].texture);
			glDeleteTextures(1,&textures[loop1].alpha);
		}
		delete[] textures;

		num_vertices = 0;
		vertices = NULL;

		num_triangles = 0;
		triangles = NULL;

		num_groups = 0;
		groups = NULL;

		num_textures = 0;
		textures = NULL;

		num_joints = 0;
		joints = NULL;

		active = false;
	}
}

void cs_am3d_c::render_no_anim(void) const
{
	if(!active)
		return;

	int loop1, loop2, loop3;
	GLboolean TexGen[2];
	am3d_triangle_t* tri;
	am3d_vertex_t* vert;

	//Draw by group

	//Render Solid Groups
	for(loop1=0; loop1<grp_solid; loop1++ )
	{
		glBegin(GL_TRIANGLES);
		for(loop2=0; loop2<groups[loop1].numtriangles; loop2++)
		{
			tri= &triangles[groups[loop1].triangleIndices[loop2]];

			//Loop through the triangle's vertices, and output them!
			for(loop3=0; loop3<3; loop3++)
			{
				vert = &vertices[tri->vertexIndices[loop3]];

				glNormal3fv(tri->vertexNormals[loop3]);
				glVertex3fv(vert->vertex);				
			}
		}		
		glEnd();
	}
	
	//Render Textured Groups
	for(loop1=grp_solid; loop1<grp_textured; loop1++ )
	{
		glBindTexture(GL_TEXTURE_2D,textures[groups[loop1].materialIndex].texture);			
		
		glBegin(GL_TRIANGLES);

		for(loop2=0; loop2<groups[loop1].numtriangles; loop2++)
		{
			tri= &triangles[groups[loop1].triangleIndices[loop2]];
			for(loop3=0; loop3<3; loop3++)
			{
				glTexCoord2f(tri->s[loop3], tri->t[loop3]);
				vert = &vertices[tri->vertexIndices[loop3]];
				
				glNormal3fv( tri->vertexNormals[loop3]);
				glVertex3fv(vert->vertex);				
			}
		}

		glEnd();		
	}

	if(grp_masked)
	{
		glDepthMask(GL_FALSE);

		TexGen[0] = glIsEnabled(GL_TEXTURE_GEN_S);
		TexGen[1] = glIsEnabled(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
		//render masked groups
		for(loop1=grp_textured; loop1<grp_masked; loop1++ )
		{
			glBlendFunc(GL_DST_COLOR,GL_ZERO);
				
			glBindTexture(GL_TEXTURE_2D,textures[groups[loop1].materialIndex].alpha);
			glBegin(GL_TRIANGLES);
			for(loop2=0; loop2<groups[loop1].numtriangles; loop2++)
			{
				tri= &triangles[groups[loop1].triangleIndices[loop2]];

				for(loop3=0; loop3<3; loop3++)
				{
					glTexCoord2f(tri->s[loop3], tri->t[loop3]);				
					vert = &vertices[tri->vertexIndices[loop3]];
					
					glNormal3fv( tri->vertexNormals[loop3]);
					glVertex3fv(vert->vertex);				
				}
			}
			glEnd();

			glBlendFunc(GL_ONE, GL_ONE);
				
			glBindTexture(GL_TEXTURE_2D,textures[groups[loop1].materialIndex].texture);

			glBegin(GL_TRIANGLES);
			for(loop2=0; loop2<groups[loop1].numtriangles; loop2++)
			{
				tri= &triangles[groups[loop1].triangleIndices[loop2]];
				
				for(loop3=0; loop3<3; loop3++)
				{
					glTexCoord2f(tri->s[loop3], tri->t[loop3]);
					vert = &vertices[tri->vertexIndices[loop3]];
					
					glNormal3fv( tri->vertexNormals[loop3]);
					glVertex3fv(vert->vertex);
				}
			}
			glEnd();		
		}

		if(TexGen[0] == GL_TRUE)
			glEnable(GL_TEXTURE_GEN_S);
		if(TexGen[1] == GL_TRUE)
			glEnable(GL_TEXTURE_GEN_T);		

		glDepthMask(GL_TRUE);
	}//if(grp_masked)
}

void cs_am3d_c::render_no_anim(const void (*gl_cmd)(word selected_t, word t_index)) const
{
	if(!active || !gl_cmd)
		return;

	int loop1, loop2, loop3;
	GLboolean TexGen[2];
	am3d_triangle_t* tri;
	am3d_vertex_t* vert;

	//Draw by group

	//Render Solid Groups
	for(loop1=0; loop1<grp_solid; loop1++ )
	{
		glBegin(GL_TRIANGLES);

		if(groups[loop1].selected)
			gl_cmd(AM3D_GROUP_T_DEF,loop1);

		for(loop2=0; loop2<groups[loop1].numtriangles; loop2++)
		{
			tri= &triangles[groups[loop1].triangleIndices[loop2]];
			if(tri->selected)
				gl_cmd(AM3D_TRIANGLE_T_DEF,groups[loop1].triangleIndices[loop2]);

			//Loop through the triangle's vertices, and output them!
			for(loop3=0; loop3<3; loop3++)
			{
				vert = &vertices[tri->vertexIndices[loop3]];
				if(vert->selected)
					gl_cmd(AM3D_VERTEX_T_DEF,tri->vertexIndices[loop3]);

				glNormal3fv( tri->vertexNormals[loop3]);
				glVertex3fv(vert->vertex);
			}
		}		
		glEnd();
	}
	
	//Render Textured Groups
	for(loop1=grp_solid; loop1<grp_textured; loop1++ )
	{
		glBindTexture(GL_TEXTURE_2D,textures[groups[loop1].materialIndex].texture);			
		
		if(groups[loop1].selected)
			gl_cmd(AM3D_GROUP_T_DEF,loop1);
		
		glBegin(GL_TRIANGLES);
		for(loop2=0; loop2<groups[loop1].numtriangles; loop2++)
		{
			tri= &triangles[groups[loop1].triangleIndices[loop2]];
			if(tri->selected)
				gl_cmd(AM3D_TRIANGLE_T_DEF,groups[loop1].triangleIndices[loop2]);

			for(loop3=0; loop3<3; loop3++)
			{
				vert = &vertices[tri->vertexIndices[loop3]];
				if(vert->selected)
					gl_cmd(AM3D_VERTEX_T_DEF,tri->vertexIndices[loop3]);
				
				glTexCoord2f(tri->s[loop3], tri->t[loop3]);
				
				glNormal3fv( tri->vertexNormals[loop3]);
				glVertex3fv(vert->vertex);
			}
		}

		glEnd();		
	}

	if(grp_masked)
	{
		glDepthMask(GL_FALSE);

		TexGen[0] = glIsEnabled(GL_TEXTURE_GEN_S);
		TexGen[1] = glIsEnabled(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
		//render masked groups
		for(loop1=grp_textured; loop1<grp_masked; loop1++ )
		{
			glBlendFunc(GL_DST_COLOR,GL_ZERO);				
			glBindTexture(GL_TEXTURE_2D,textures[groups[loop1].materialIndex].alpha);

			if(groups[loop1].selected)
				gl_cmd(AM3D_GROUP_T_DEF,loop1);

			glBegin(GL_TRIANGLES);			
			for(loop2=0; loop2<groups[loop1].numtriangles; loop2++)
			{
				tri= &triangles[groups[loop1].triangleIndices[loop2]];
				if(tri->selected)
					gl_cmd(AM3D_TRIANGLE_T_DEF,groups[loop1].triangleIndices[loop2]);

				for(loop3=0; loop3<3; loop3++)
				{
					vert = &vertices[tri->vertexIndices[loop3]];
					if(vert->selected)
						gl_cmd(AM3D_VERTEX_T_DEF,tri->vertexIndices[loop3]);

					glTexCoord2f(tri->s[loop3], tri->t[loop3]);				
					
					glNormal3fv( tri->vertexNormals[loop3]);
					glVertex3fv(vert->vertex);
				}
			}
			glEnd();

			glBlendFunc(GL_ONE, GL_ONE);				
			glBindTexture(GL_TEXTURE_2D,textures[groups[loop1].materialIndex].texture);

			glBegin(GL_TRIANGLES);
			for(loop2=0; loop2<groups[loop1].numtriangles; loop2++)
			{
				tri= &triangles[groups[loop1].triangleIndices[loop2]];
				if(tri->selected)
					gl_cmd(AM3D_TRIANGLE_MASK_T_DEF,groups[loop1].triangleIndices[loop2]);
				
				for(loop3=0; loop3<3; loop3++)
				{
					vert = &vertices[tri->vertexIndices[loop3]];
					if(vert->selected)
						gl_cmd(AM3D_VERTEX_MASK_T_DEF,tri->vertexIndices[loop3]);

					glTexCoord2f(tri->s[loop3], tri->t[loop3]);
					
					glNormal3fv( tri->vertexNormals[loop3]);
					glVertex3fv(vert->vertex);
				}
			}
			glEnd();		
		}

		if(TexGen[0] == GL_TRUE)
			glEnable(GL_TEXTURE_GEN_S);
		if(TexGen[1] == GL_TRUE)
			glEnable(GL_TEXTURE_GEN_T);		

		glDepthMask(GL_TRUE);
	}//if(grp_masked)
}

void cs_am3d_c::render(void) const
{
	if(!active || !joints)
		return;

	int loop1, loop2, loop3;
	GLboolean TexGen[2];
	am3d_triangle_t* tri;
	am3d_vertex_t* vert;

	mth_vector vec;

	//Draw by group

	//Render Solid Groups
	for(loop1=0; loop1<grp_solid; loop1++ )
	{
		glBegin(GL_TRIANGLES);
		for(loop2=0; loop2<groups[loop1].numtriangles; loop2++)
		{
			tri= &triangles[groups[loop1].triangleIndices[loop2]];

			//Loop through the triangle's vertices, and output them!
			for(loop3=0; loop3<3; loop3++)
			{
				vert = &vertices[tri->vertexIndices[loop3]];

				if(vert->boneID != -1)
				{
					vec.set_vector(tri->vertexNormals[loop3]);
					vec.set_matrix_no_trans(joints[vert->boneID].final);
					glNormal3fv( vec.get_vec() );

					vec.set_vector(vert->vertex);
					vec.set_matrix(joints[vert->boneID].final);
					glVertex3fv( vec.get_vec() );
				}
				else
				{
					glNormal3fv(tri->vertexNormals[loop3]);
					glVertex3fv(vert->vertex);
				}
			}
		}		
		glEnd();
	}
	
	//Render Textured Groups
	for(loop1=grp_solid; loop1<grp_textured; loop1++ )
	{
		glBindTexture(GL_TEXTURE_2D,textures[groups[loop1].materialIndex].texture);			
		
		glBegin(GL_TRIANGLES);

		for(loop2=0; loop2<groups[loop1].numtriangles; loop2++)
		{
			tri= &triangles[groups[loop1].triangleIndices[loop2]];
			for(loop3=0; loop3<3; loop3++)
			{
				glTexCoord2f(tri->s[loop3], tri->t[loop3]);
				vert = &vertices[tri->vertexIndices[loop3]];
				
				if(vert->boneID != -1)
				{
					vec.set_vector(tri->vertexNormals[loop3]);
					vec.set_matrix_no_trans(joints[vert->boneID].final);
					glNormal3fv( vec.get_vec() );

					vec.set_vector(vert->vertex);
					vec.set_matrix(joints[vert->boneID].final);
					glVertex3fv( vec.get_vec() );
				}
				else
				{
					glNormal3fv(tri->vertexNormals[loop3]);
					glVertex3fv(vert->vertex);
				}
			}
		}

		glEnd();		
	}

	if(grp_masked)
	{
		glDepthMask(GL_FALSE);

		TexGen[0] = glIsEnabled(GL_TEXTURE_GEN_S);
		TexGen[1] = glIsEnabled(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
		//render masked groups
		for(loop1=grp_textured; loop1<grp_masked; loop1++ )
		{
			glBlendFunc(GL_DST_COLOR,GL_ZERO);
				
			glBindTexture(GL_TEXTURE_2D,textures[groups[loop1].materialIndex].alpha);
			glBegin(GL_TRIANGLES);
			for(loop2=0; loop2<groups[loop1].numtriangles; loop2++)
			{
				tri= &triangles[groups[loop1].triangleIndices[loop2]];

				for(loop3=0; loop3<3; loop3++)
				{
					glTexCoord2f(tri->s[loop3], tri->t[loop3]);				
					vert = &vertices[tri->vertexIndices[loop3]];
					
					if(vert->boneID != -1)
					{
						vec.set_vector(tri->vertexNormals[loop3]);
						vec.set_matrix_no_trans(joints[vert->boneID].final);
						glNormal3fv( vec.get_vec() );

						vec.set_vector(vert->vertex);
						vec.set_matrix(joints[vert->boneID].final);
						glVertex3fv( vec.get_vec() );
					}
					else
					{
						glNormal3fv(tri->vertexNormals[loop3]);
						glVertex3fv(vert->vertex);
					}
				}
			}
			glEnd();

			glBlendFunc(GL_ONE, GL_ONE);
				
			glBindTexture(GL_TEXTURE_2D,textures[groups[loop1].materialIndex].texture);

			glBegin(GL_TRIANGLES);
			for(loop2=0; loop2<groups[loop1].numtriangles; loop2++)
			{
				tri= &triangles[groups[loop1].triangleIndices[loop2]];
				
				for(loop3=0; loop3<3; loop3++)
				{
					glTexCoord2f(tri->s[loop3], tri->t[loop3]);
					vert = &vertices[tri->vertexIndices[loop3]];
					
					if(vert->boneID != -1)
					{
						vec.set_vector(tri->vertexNormals[loop3]);
						vec.set_matrix_no_trans(joints[vert->boneID].final);
						glNormal3fv( vec.get_vec() );

						vec.set_vector(vert->vertex);
						vec.set_matrix(joints[vert->boneID].final);
						glVertex3fv( vec.get_vec() );
					}
					else
					{
						glNormal3fv(tri->vertexNormals[loop3]);
						glVertex3fv(vert->vertex);
					}
				}
			}
			glEnd();		
		}

		if(TexGen[0] == GL_TRUE)
			glEnable(GL_TEXTURE_GEN_S);
		if(TexGen[1] == GL_TRUE)
			glEnable(GL_TEXTURE_GEN_T);		

		glDepthMask(GL_TRUE);
	}//if(grp_masked)
}

void cs_am3d_c::render( const void (*gl_cmd)(word selected_t, word t_index) ) const
{
	if(!active || !gl_cmd || !joints)
		return;

	int loop1, loop2, loop3;
	GLboolean TexGen[2];
	am3d_triangle_t* tri;
	am3d_vertex_t* vert;

	mth_vector vec;

	//Draw by group

	//Render Solid Groups
	for(loop1=0; loop1<grp_solid; loop1++ )
	{
		glBegin(GL_TRIANGLES);

		if(groups[loop1].selected)
			gl_cmd(AM3D_GROUP_T_DEF,loop1);

		for(loop2=0; loop2<groups[loop1].numtriangles; loop2++)
		{
			tri= &triangles[groups[loop1].triangleIndices[loop2]];
			if(tri->selected)
				gl_cmd(AM3D_TRIANGLE_T_DEF,groups[loop1].triangleIndices[loop2]);

			//Loop through the triangle's vertices, and output them!
			for(loop3=0; loop3<3; loop3++)
			{
				vert = &vertices[tri->vertexIndices[loop3]];
				if(vert->selected)
					gl_cmd(AM3D_VERTEX_T_DEF,tri->vertexIndices[loop3]);

				if(vert->boneID != -1)
				{
					vec.set_vector(tri->vertexNormals[loop3]);
					vec.set_matrix_no_trans(joints[vert->boneID].final);
					glNormal3fv( vec.get_vec() );

					vec.set_vector(vert->vertex);
					vec.set_matrix(joints[vert->boneID].final);
					glVertex3fv( vec.get_vec() );
				}
				else
				{
					glNormal3fv(tri->vertexNormals[loop3]);
					glVertex3fv(vert->vertex);
				}
			}
		}		
		glEnd();
	}
	
	//Render Textured Groups
	for(loop1=grp_solid; loop1<grp_textured; loop1++ )
	{
		glBindTexture(GL_TEXTURE_2D,textures[groups[loop1].materialIndex].texture);			
		
		if(groups[loop1].selected)
			gl_cmd(AM3D_GROUP_T_DEF,loop1);
		
		glBegin(GL_TRIANGLES);
		for(loop2=0; loop2<groups[loop1].numtriangles; loop2++)
		{
			tri= &triangles[groups[loop1].triangleIndices[loop2]];
			if(tri->selected)
				gl_cmd(AM3D_TRIANGLE_T_DEF,groups[loop1].triangleIndices[loop2]);

			for(loop3=0; loop3<3; loop3++)
			{
				vert = &vertices[tri->vertexIndices[loop3]];
				if(vert->selected)
					gl_cmd(AM3D_VERTEX_T_DEF,tri->vertexIndices[loop3]);
				
				glTexCoord2f(tri->s[loop3], tri->t[loop3]);
				
				if(vert->boneID != -1)
				{
					vec.set_vector(tri->vertexNormals[loop3]);
					vec.set_matrix_no_trans(joints[vert->boneID].final);
					glNormal3fv( vec.get_vec() );

					vec.set_vector(vert->vertex);
					vec.set_matrix(joints[vert->boneID].final);
					glVertex3fv( vec.get_vec() );
				}
				else
				{
					glNormal3fv(tri->vertexNormals[loop3]);
					glVertex3fv(vert->vertex);
				}
			}
		}

		glEnd();		
	}

	if(grp_masked)
	{
		glDepthMask(GL_FALSE);

		TexGen[0] = glIsEnabled(GL_TEXTURE_GEN_S);
		TexGen[1] = glIsEnabled(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
		//render masked groups
		for(loop1=grp_textured; loop1<grp_masked; loop1++ )
		{
			glBlendFunc(GL_DST_COLOR,GL_ZERO);				
			glBindTexture(GL_TEXTURE_2D,textures[groups[loop1].materialIndex].alpha);

			if(groups[loop1].selected)
				gl_cmd(AM3D_GROUP_T_DEF,loop1);

			glBegin(GL_TRIANGLES);			
			for(loop2=0; loop2<groups[loop1].numtriangles; loop2++)
			{
				tri= &triangles[groups[loop1].triangleIndices[loop2]];
				if(tri->selected)
					gl_cmd(AM3D_TRIANGLE_T_DEF,groups[loop1].triangleIndices[loop2]);

				for(loop3=0; loop3<3; loop3++)
				{
					vert = &vertices[tri->vertexIndices[loop3]];
					if(vert->selected)
						gl_cmd(AM3D_VERTEX_T_DEF,tri->vertexIndices[loop3]);

					glTexCoord2f(tri->s[loop3], tri->t[loop3]);				
					
					if(vert->boneID != -1)
					{
						vec.set_vector(tri->vertexNormals[loop3]);
						vec.set_matrix_no_trans(joints[vert->boneID].final);
						glNormal3fv( vec.get_vec() );

						vec.set_vector(vert->vertex);
						vec.set_matrix(joints[vert->boneID].final);
						glVertex3fv( vec.get_vec() );
					}
					else
					{
						glNormal3fv(tri->vertexNormals[loop3]);
						glVertex3fv(vert->vertex);
					}
				}
			}
			glEnd();

			glBlendFunc(GL_ONE, GL_ONE);				
			glBindTexture(GL_TEXTURE_2D,textures[groups[loop1].materialIndex].texture);

			glBegin(GL_TRIANGLES);
			for(loop2=0; loop2<groups[loop1].numtriangles; loop2++)
			{
				tri= &triangles[groups[loop1].triangleIndices[loop2]];
				if(tri->selected)
					gl_cmd(AM3D_TRIANGLE_MASK_T_DEF,groups[loop1].triangleIndices[loop2]);
				
				for(loop3=0; loop3<3; loop3++)
				{
					vert = &vertices[tri->vertexIndices[loop3]];
					if(vert->selected)
						gl_cmd(AM3D_VERTEX_MASK_T_DEF,tri->vertexIndices[loop3]);

					glTexCoord2f(tri->s[loop3], tri->t[loop3]);
					
					if(vert->boneID != -1)
					{
						vec.set_vector(tri->vertexNormals[loop3]);
						vec.set_matrix_no_trans(joints[vert->boneID].final);
						glNormal3fv( vec.get_vec() );

						vec.set_vector(vert->vertex);
						vec.set_matrix(joints[vert->boneID].final);
						glVertex3fv( vec.get_vec() );
					}
					else
					{
						glNormal3fv(tri->vertexNormals[loop3]);
						glVertex3fv(vert->vertex);
					}
				}
			}
			glEnd();		
		}

		if(TexGen[0] == GL_TRUE)
			glEnable(GL_TEXTURE_GEN_S);
		if(TexGen[1] == GL_TRUE)
			glEnable(GL_TEXTURE_GEN_T);		

		glDepthMask(GL_TRUE);
	}//if(grp_masked)
}

bool cs_am3d_c::apply_skel(const cs_sk3d_c * newSkel)
{
	if(!active || !newSkel)
		return false;

	if(newSkel->has_abs())
	{
		skeleton = (cs_sk3d_c *)newSkel;
		
		num_joints = skeleton->get_num_jts();
		delete[] joints;

		joints = new am3d_joint_t [num_joints];
		if(joints == NULL)
			return false;

		//apply the inverses to the model's vertices
		int loop, loop1;
		mth_vector_wrp vec;
		
		for(loop = 0; loop < num_vertices; loop++)
		{
			if(vertices[loop].boneID != -1)
			{
				//transform the vector
				vec.wrp_vector(vertices[loop].vertex);
				vec.set_matrix_inv(skeleton->get_abs(vertices[loop].boneID));				
			}		
		}

		//apply the inverses to the model's normals
		for(loop = 0; loop < num_triangles; loop++)
		{
			for(loop1 = 0; loop1 < 3; loop1 ++)
			{
				if(vertices[triangles[loop].vertexIndices[loop1]].boneID != -1)
				{
					vec.wrp_vector(triangles[loop].vertexNormals[loop1]);
					vec.set_matrix_inv_no_trans(skeleton->get_abs(vertices[triangles[loop].vertexIndices[loop1]].boneID));
				}					
			}
		}

		return true;
	}
	return false;
}

void cs_am3d_c::set_joint(float * jnt, int index)
{
	if(!active || (index >= num_joints))
		return;

	joints[index].final = jnt;
}

cs_sk3d_c * cs_am3d_c::get_skel(void) const
{
	return skeleton;
}

word * cs_am3d_c::gel_sel_vertices(int & num_vert) const
{
	if(!active)
		return 0;

	num_vert = 0;
	int loop;
	for(loop = 0; loop < num_vertices; loop++)
	{
		if(vertices[loop].selected)
			num_vert++;
	}

	if(!num_vert)
		return 0;

	word * ret_ary = new word[num_vert];
	if(!ret_ary)
		return 0;
	int ary_loop = 0;
	
	for(loop = 0; loop < num_vertices; loop++)
	{
		if(vertices[loop].selected)
		{
			ret_ary[ary_loop] = loop;
			ary_loop++;
		}
	}

	return ret_ary;
}

am3d_sel_vertex_t cs_am3d_c::get_vertex(const word index, const bool anim) const
{
	am3d_sel_vertex_t ret_val;

	if(!active || (index >= num_vertices))
	{
		ret_val.vertex[0] = 0.0f;
		ret_val.vertex[1] = 0.0f;
		ret_val.vertex[2] = 0.0f;
	}
	else
	{
		if(anim && vertices[index].boneID != -1)
		{
			//Transform the vector
			mth_vector vec(vertices[index].vertex);
			vec.set_matrix(joints[vertices[index].boneID].final);			
			ret_val.vertex[0] = vec[0];
			ret_val.vertex[1] = vec[1];
			ret_val.vertex[2] = vec[2];			
		}
		else
		{
			ret_val.vertex[0] = vertices[index].vertex[0];
			ret_val.vertex[1] = vertices[index].vertex[1];
			ret_val.vertex[2] = vertices[index].vertex[2];
		}		
	}

	return ret_val;
}

//··········································································//
//    Source :: Internal Function Implementation
//··········································································//

//··········································································//
//    Source :: Internal Class Implementation
//··········································································//

//----------------------------------------------------------------------------
//cs_am3d.cpp
//Copyright © Rishi Ramraj, 2004
//----------------------------------------------------------------------------
