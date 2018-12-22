/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#include<stdio.h>
#include<OpenGL.h>
#include<TGA.h>
#include<CubeMap.h>
#include<File.h>


DECLARE_ENGINE_NAMESPACE


RTTI RenderInterface::m_RTTIType("RenderInterface", NULL);
RTTI OpenGLRenderer::m_RTTIType("OpenGLRenderer", &RenderInterface::m_RTTIType);


OpenGLRenderer::OpenGLRenderer()
{
   m_currentDesc = 0;
   m_vbo = m_ibo = 0;
   m_vboSupport = false;
   m_fboSupport = false;
   m_glslSupport = false;

   m_texManager = new ResourceManager<GLTexture, BB_TEXTURE>(257);
}


OpenGLRenderer::~OpenGLRenderer()
{
   Shutdown();
}


BRESULT OpenGLRenderer::Initialize(const RenderParams *params)
{
   BRESULT result;

   result = InitializeOS(params);

   if(result != BB_SUCCESS)
      {
         return result;
      }

   InitializeExtensions();

   if(params->m_maxCacheBytes <= 0 || params->m_maxCacheIndices <= 0)
      {
         m_vboSupport = false;
      }

   if(m_vboSupport)
      {
         glGenBuffersARB(1, &m_vbo);
         glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vbo);
         glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

         glGenBuffersARB(1, &m_ibo);
         glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_ibo);
         glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
      }

   return BB_SUCCESS;
}


void OpenGLRenderer::Shutdown()
{
   if(m_vboSupport) 
      {
         if(m_vbo)
            {
               glDeleteBuffersARB(1, &m_vbo);
            }

         if(m_ibo)
            {
               glDeleteBuffersARB(1, &m_ibo);
            }

         m_vbo = m_ibo = 0;
      }

   if(m_texManager != NULL)
      {
         delete m_texManager;
         m_texManager = NULL;
      }

   for(int i = 0; i < (int)m_shaders.size(); i++)
      {
         glDeleteObjectARB(m_shaders[i].m_vs);
         glDeleteObjectARB(m_shaders[i].m_ps);
         glDeleteObjectARB(m_shaders[i].m_context);
      }

   m_shaders.clear();
   m_parameters.clear();

   ShutdownOS();
}


void OpenGLRenderer::SetViewPort(int x, int y, int width, int height)
{
   glViewport(x, y, width, height);
}


void OpenGLRenderer::ResizeWindow(float fov, float nearDist, float farDist,
                                  int width, int height)
{
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();

   gluPerspective(fov, (GLfloat)width / (GLfloat)height,
                  (double)nearDist, (double)farDist);

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}


void OpenGLRenderer::ResizeWindowOrtho(int left, int right, int bottom, int top,
                                        float nearDist, float farDist)
{
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();

   glOrtho(left, right, bottom, top, nearDist, farDist);

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}


void OpenGLRenderer::SetClearColor(int r, int g, int b, int a)
{
   glClearColor((float)r / 255, (float)g / 255,
                (float)b / 255, (float)a / 255);

   m_red = r;
   m_green = g;
   m_blue = b;
   m_alpha = a;
}


void OpenGLRenderer::StartRendering(bool color, bool depth, bool stencil)
{
   GLbitfield flags = 0;

   if(color) flags |= GL_COLOR_BUFFER_BIT;
   if(depth) flags |= GL_DEPTH_BUFFER_BIT;
   if(stencil) flags |= GL_STENCIL_BUFFER_BIT;

   glClear(flags);
}


BRESULT OpenGLRenderer::Enable(BB_RENDER_STATE state)
{
   switch(state)
      {
         case BB_DEPTH_TESTING:
            glEnable(GL_DEPTH_TEST);
         break;

         case BB_SMOOTH_SHADING:
            glShadeModel(GL_SMOOTH);
         break;

         case BB_TEXTURE_2D:
            glEnable(GL_TEXTURE_2D);
         break;

         case BB_TEXTURE_CUBE:
            if(m_cubeMapSupport)
               glEnable(GL_TEXTURE_CUBE_MAP_ARB);
         break;

         case BB_ALPHA_TESTING:
            glEnable(GL_ALPHA_TEST);
         break;

         case BB_BACK_CCW_CULLING:
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            glFrontFace(GL_CCW);
         break;

         case BB_BACK_CW_CULLING:
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            glFrontFace(GL_CW);
         break;

         case BB_FRONT_CCW_CULLING:
            glEnable(GL_CULL_FACE);
            glCullFace(GL_FRONT);
            glFrontFace(GL_CCW);
         break;

         case BB_FRONT_CW_CULLING:
            glEnable(GL_CULL_FACE);
            glCullFace(GL_FRONT);
            glFrontFace(GL_CW);
         break;

         default:
            return BB_FAIL;
            break;
      };

   return BB_SUCCESS;
}


BRESULT OpenGLRenderer::Disable(BB_RENDER_STATE state)
{
   switch(state)
      {
         case BB_DEPTH_TESTING:
            glDisable(GL_DEPTH_TEST);
         break;

         case BB_SMOOTH_SHADING:
            glShadeModel(GL_FLAT);
         break;

         case BB_TEXTURE_2D:
            glDisable(GL_TEXTURE_2D);
         break;

         case BB_TEXTURE_CUBE:
            if(m_cubeMapSupport)
               glDisable(GL_TEXTURE_CUBE_MAP_ARB);
         break;

         case BB_ALPHA_TESTING:
            glDisable(GL_ALPHA_TEST);
         break;

         case BB_BACK_CCW_CULLING:
         case BB_FRONT_CCW_CULLING:
         case BB_BACK_CW_CULLING:
         case BB_FRONT_CW_CULLING:
            glDisable(GL_CULL_FACE);
         break;

         default:
            return BB_FAIL;
            break;
      };

   return BB_SUCCESS;
}


void OpenGLRenderer::SetAlphaFunc(BB_ALPHA_STATE state, float val)
{
   switch(state)
      {
         case BB_ALPHA_LESS:
            glAlphaFunc(GL_LESS, val);
         break;

         case BB_ALPHA_GREATER:
            glAlphaFunc(GL_GREATER, val);
         break;

         default:
            break;
      };
}


BRESULT OpenGLRenderer::SwitchMatrixType(BB_MATRIX_TYPE type)
{
   switch(type)
      {
         case BB_PROJECTION_MATRIX:
            glMatrixMode(GL_PROJECTION_MATRIX);
            break;

         case BB_MODELVIEW_MATRIX:
            glMatrixMode(GL_MODELVIEW_MATRIX);
            break;

         case BB_TEXTURE_MATRIX:
            glMatrixMode(GL_TEXTURE_MATRIX);
            break;

         default:
            return BB_FAIL;
            break;
      }

   return BB_SUCCESS;
}


void OpenGLRenderer::GetMatrix(BB_MATRIX_TYPE type, float *matrix)
{
   switch(type)
      {
         case BB_PROJECTION_MATRIX:
            glGetFloatv(GL_PROJECTION_MATRIX, matrix);
            break;

         case BB_MODELVIEW_MATRIX:
            glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
            break;

         case BB_TEXTURE_MATRIX:
            glGetFloatv(GL_TEXTURE_MATRIX, matrix);
            break;

         default:
            break;
      }
}


void OpenGLRenderer::RotateMatrix(float angle, float x, float y, float z)
{
   glRotatef(angle, x, y, z);
}


void OpenGLRenderer::TranslateMatrix(float x, float y, float z)
{
   glTranslatef(x, y, z);
}


void OpenGLRenderer::SetView(float posX, float posY, float posZ,
                             float lookX, float lookY, float lookZ,
                             float upX, float upY, float upZ)
{
   gluLookAt(posX, posY, posZ, lookX, lookY, lookZ, upX, upY, upZ);
}


void OpenGLRenderer::SetMatrix(const float *matrix)
{
   if(matrix != NULL)
      glLoadMatrixf(matrix);
}


void OpenGLRenderer::MultiplyToCurrentMatrix(const float *matrix)
{
   if(matrix != NULL)
      glMultMatrixf(matrix);
}


void OpenGLRenderer::LoadIdentityMatrix()
{
   glLoadIdentity();
}


void OpenGLRenderer::PushMatrix()
{
   glPushMatrix();
}


void OpenGLRenderer::PopMatrix()
{
   glPopMatrix();
}


void OpenGLRenderer::ApplyFilters(BB_TEXTURE &handle, BB_FILTER_LIST_PTR filters)
{
   GLTexture *ptr = m_texManager->GetResource(handle);

   if(ptr == NULL)
      return;

   GLenum texType = 0;
   BB_FILTER_TYPE filterItem;
   char *ext = NULL;
   GLfloat max;

   if(ptr->GetTextureType() == BB_TEX2D_TYPE)
      texType = GL_TEXTURE_2D;
   else
      texType = GL_TEXTURE_CUBE_MAP_ARB;

   glBindTexture(texType, ptr->GetID());

   if(filters != NULL)
      {
         for(int i = 0; i < (int)filters->size(); i++)
            {
               filterItem = (*filters)[i];

               if(filterItem == BB_MIN_POINT_FILTER)
                  {
                     glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                  }
               else if(filterItem == BB_MIN_LINEAR_FILTER)
                  {
                     glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                  }
               else if(filterItem == BB_MAG_POINT_FILTER)
                  {
                     glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                  }
               else if(filterItem == BB_MAG_LINEAR_FILTER)
                  {
                     glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                  }
               else if(filterItem == BB_MIP_POINT_FILTER)
                  {
                     glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
                     glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
                  }
               else if(filterItem == BB_MIP_LINEAR_FILTER)
                  {
                     glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                     glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                  }
               else if(filterItem == BB_USE_ANSIO_FILTER)
                  {
                     ext = (char*)glGetString(GL_EXTENSIONS);

                     if(strstr(ext, "GL_EXT_texture_filter_anisotropic") != 0)
                        {
                           glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max);
                           glTexParameterf(texType, GL_TEXTURE_MAX_ANISOTROPY_EXT, max);
                        }
                  }
               else if(filterItem == BB_S_REPEAT_FILTER)
                  {
                     glTexParameterf(texType, GL_TEXTURE_WRAP_S, GL_REPEAT);
                  }
               else if(filterItem == BB_T_REPEAT_FILTER)
                  {
                     glTexParameterf(texType, GL_TEXTURE_WRAP_T, GL_REPEAT);
                  }
               else if(filterItem == BB_R_REPEAT_FILTER)
                  {
                     glTexParameterf(texType, GL_TEXTURE_WRAP_R, GL_REPEAT);
                  }
               else if(filterItem == BB_S_CLAMP_FILTER)
                  {
                     glTexParameterf(texType, GL_TEXTURE_WRAP_S, GL_CLAMP);
                  }
               else if(filterItem == BB_T_CLAMP_FILTER)
                  {
                     glTexParameterf(texType, GL_TEXTURE_WRAP_T, GL_CLAMP);
                  }
               else if(filterItem == BB_R_CLAMP_FILTER)
                  {
                     glTexParameterf(texType, GL_TEXTURE_WRAP_R, GL_CLAMP);
                  }
               else if(filterItem == BB_S_EDGE_CLAMP_FILTER)
                  {
                     glTexParameterf(texType, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                  }
               else if(filterItem == BB_T_EDGE_CLAMP_FILTER)
                  {
                     glTexParameterf(texType, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                  }
               else if(filterItem == BB_R_EDGE_CLAMP_FILTER)
                  {
                     glTexParameterf(texType, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
                  }
            }
      }

   glBindTexture(texType, 0);
}


unsigned int OpenGLRenderer::CreateGLTexture(unsigned char *image, BB_TEXTURE_TYPE type,
                                             BB_TEXTURE_FORMAT format, int width, int height,
                                             int components)
{
   GLuint tex = 0;
   GLenum glTexType = 0, glTexFormat = 0;

   glGenTextures(1, &tex);

   if(type == BB_TEX2D_TYPE)
      {
         glTexType = GL_TEXTURE_2D;
      }
   else
      {
         if(!m_cubeMapSupport)
            return 0;

         glTexType = GL_TEXTURE_CUBE_MAP_ARB;
      }

   if(format == BB_TEX_UNSIGNED_BYTE)
      {
         glTexFormat = GL_UNSIGNED_BYTE;
      }
   else
      {
         glTexFormat = GL_FLOAT;
      }

   glEnable(glTexType);
   glBindTexture(glTexType, tex);

   // RGB or RGBA.
   int c = components == 3 ? GL_RGB : GL_RGBA;

   if(type == BB_TEX2D_TYPE)
      {
         if(image == NULL)
            {
               glTexImage2D(glTexType, 0, components, width, height,
                            0, c, glTexFormat, 0);
            }
         else
            {
               gluBuild2DMipmaps(glTexType, components, width, height,
                                 c, glTexFormat, image);
            }
      }
   else
      {
         unsigned char *img = NULL;
         unsigned char *ptr = image;

         int size = width * height * components;

         GLenum targets[] = { GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB,
                              GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB,
                              GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB,
                              GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB,
                              GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB,
                              GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB };

         if(image != NULL)
            {
               img = new unsigned char[size];
            }

         for(int i = 0; i < 6; i++)
            {
               if(image)
                  {
                     memcpy(img, ptr, size);
                     ptr += size;

                     gluBuild2DMipmaps(targets[i], components, width, height,
                                       c, glTexFormat, img);
                  }
               else
                  {
                     glTexImage2D(targets[i], 0, components, width, height,
                                 0, c, glTexFormat, 0);
                  }
            }

         if(img != NULL)
            {
               delete[] img;
            }
      }

   glBindTexture(glTexType, 0);

   return tex;
}


bool OpenGLRenderer::AddEmptyTexture(char *name, int width, int height,
                                     int components, BB_TEXTURE_TYPE type,
                                     BB_TEXTURE_FORMAT format, BB_TEXTURE *handle)
{
   unsigned int tex = 0;
   int index = -1;

   if(handle == NULL)
      return false;

   handle->Nullify();

   // RGB or RGBA.
   if(components != 3 && components != 4)
      return false;

   index = m_texManager->GetRegisteredIndex(name);

   if(index == -1)
      {
         tex = CreateGLTexture(NULL, type, format, width,
                               height, components);

         if(tex <= 0)
            return false;

         m_texManager->Create(new GLTexture(tex, type), name, handle);
      }
   else
      {
         handle->Initialize(index);
      }

   return true;
}


bool OpenGLRenderer::LoadTexFromFile(char *fileName, BB_TEXTURE_TYPE type,
                                     BB_TEXTURE *handle)
{
   unsigned char *image = NULL;
   unsigned int tex = 0;
   int w = 0, h = 0, c = 0;
   int index = -1;

   if(handle == NULL)
      return false;

   handle->Nullify();

   index = m_texManager->GetRegisteredIndex(fileName);

   if(index == -1)
      {
         if(type == BB_TEX2D_TYPE)
            image = LoadTGA(fileName, w, h, c);
         else
            image = LoadCubeMap(fileName, w, h, c);

         if(image == NULL)
            return false;

         tex = CreateGLTexture(image, type, BB_TEX_UNSIGNED_BYTE,
                               w, h, c);
         
         delete[] image;

         m_texManager->Create(new GLTexture(tex, type), fileName, handle);
      }
   else
      {
         handle->Initialize(index);
      }

   return true;
}


bool OpenGLRenderer::LoadTexFromMemory(char *name, char *fileData,
                                       int length, BB_TEXTURE_TYPE type,
                                       BB_TEXTURE *handle)
{
   unsigned char *image = NULL;
   unsigned int tex = 0;
   int w = 0, h = 0, c = 0;
   int index = -1;

   if(handle == NULL || name == NULL || fileData == NULL)
      {
         return false;
      }

   handle->Nullify();

   index = m_texManager->GetRegisteredIndex(name);

   if(index == -1)
      {
         if(type == BB_TEX2D_TYPE)
            image = LoadTGAFromMemory(fileData, length, w, h, c);
         else
            image = LoadCubeMapFromMemory(fileData, w, h, c);

         if(image == NULL)
            return false;

         tex = CreateGLTexture(image, type, BB_TEX_UNSIGNED_BYTE,
                               w, h, c);

         delete[] image;

         m_texManager->Create(new GLTexture(tex, type), name, handle);
      }
   else
      {
         handle->Initialize(index);
      }

   return true;
}


bool OpenGLRenderer::ApplyTexture(int texUnit, BB_TEXTURE &handle)
{
   GLTexture *ptr = m_texManager->GetResource(handle);

   if(texUnit < 0 || ptr == NULL)
      return false;

   GLenum texType = 0;
   unsigned int id = 0;

   id = ptr->GetID();

   if(ptr->GetTextureType() == BB_TEX2D_TYPE)
      {
         texType = GL_TEXTURE_2D;
      }
   else
      {
         texType = GL_TEXTURE_CUBE_MAP_ARB;
      }

   if(m_multiTexSupport)
      {
         glActiveTextureARB(GL_TEXTURE0_ARB + texUnit);
      }

   glEnable(texType);
   glBindTexture(texType, id);

   if(m_multiTexSupport)
      {
         glActiveTextureARB(GL_TEXTURE0_ARB);
      }

   return true;
}


void OpenGLRenderer::DeleteTexture(BB_TEXTURE &handle)
{
   m_texManager->Release(handle);
}


bool OpenGLRenderer::CreateShaderFromFile(char *vs, char *ps, ShaderHandle *handle)
{
   if(vs == NULL || handle == NULL)
      return false;
   
   FileInputStream vsFile, psFile;
   int vsFileSize = 0, psFileSize = 0;
   char *vsFileData = NULL, *psFileData = NULL;

   vsFile.OpenFile(vs, BB_TEXT_FILE);
   vsFileSize = vsFile.GetFileSize();

   if(vsFileSize > 0)
      {
         vsFileData = new char[vsFileSize + 1];
         memset(vsFileData, 0, vsFileSize);

         vsFile.Read(vsFileData, vsFileSize);
      }

   if(ps != NULL)
      {
         psFile.OpenFile(ps, BB_TEXT_FILE);
         psFileSize = psFile.GetFileSize();
      }

   if(psFileSize > 0 && ps != NULL)
      {
         psFileData = new char[psFileSize + 1];
         memset(psFileData, 0, psFileSize);

         psFile.Read(psFileData, psFileSize);
      }

   bool result = CreateShaderFromMemory(vsFileData, psFileData,
                                        handle);

   delete[] vsFileData;
   delete[] psFileData;

   return result;
}


bool OpenGLRenderer::CreateShaderFromMemory(char *vsFile, char *psFile,
                                            ShaderHandle *handle)
{
   if(vsFile == NULL || handle == NULL || m_glslSupport == false)
      return false;

   if(m_glslSupport == false)
      printf("No GLSL support found!\n");

   *handle = -1;

   GLint result;
   GLSLShader glslShader;

   glslShader.m_context = glCreateProgramObjectARB();

   glslShader.m_vs = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
   glShaderSourceARB(glslShader.m_vs, 1, (const char**)&vsFile, NULL);
   glCompileShaderARB(glslShader.m_vs);
   glGetObjectParameterivARB(glslShader.m_vs, GL_OBJECT_COMPILE_STATUS_ARB, &result);
   glAttachObjectARB(glslShader.m_context, glslShader.m_vs);

   char error[4096];
   
   if(!result)
      {
         glGetInfoLogARB(glslShader.m_vs, sizeof(error),
                         NULL, error);

         // TO DO: Log errors!
         printf("Vertex Shader Errors: ");
         printf(error);
         printf("\n");

         return false;
      }

   if(psFile != NULL)
      {
         glslShader.m_ps = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
         glShaderSourceARB(glslShader.m_ps, 1, (const char**)&psFile, NULL);
         glCompileShaderARB(glslShader.m_ps);
         glGetObjectParameterivARB(glslShader.m_ps, GL_OBJECT_COMPILE_STATUS_ARB, &result);
         glAttachObjectARB(glslShader.m_context, glslShader.m_ps);
      }

   glLinkProgramARB(glslShader.m_context);

   if(!result)
      {
         glGetInfoLogARB(glslShader.m_ps, sizeof(error),
                         NULL, error);

         // TO DO: Log errors!
         printf("Pixel Shader Errors: ");
         printf(error);
         printf("\n");

         return false;
      }

   glGetObjectParameterivARB(glslShader.m_context,
                             GL_OBJECT_LINK_STATUS_ARB,
                             &result);
   if(!result)
      {
         glGetInfoLogARB(glslShader.m_context, sizeof(error),
                         NULL, error);

         // TO DO: Log or display errors!
         printf(error);
         printf("/n");

         return false;
      }

   m_shaders.push_back(glslShader);

   *handle = (int)m_shaders.size() - 1;

   return true;
}


void OpenGLRenderer::SetupShaderParameter(char *pname,
                                          ShaderHandle shader,
                                          ParameterHandle *param)
{
   if(param == NULL || pname == NULL)
      return;

   if(shader < 0 || shader >= (int)m_shaders.size())
      return;

   GLSLParameter glslParam;

   glslParam.m_parameter = glGetUniformLocationARB(m_shaders[shader].m_context, pname);

   m_parameters.push_back(glslParam);
   *param = (int)m_parameters.size() - 1;
}


void OpenGLRenderer::SetShaderParameter1i(ParameterHandle param, float val)
{
   if(param >= 0 && param < (int)m_parameters.size() && m_glslSupport != false)
      glUniform1iARB(m_parameters[param].m_parameter, val);
}


void OpenGLRenderer::SetShaderParameter1f(ParameterHandle param, float val)
{
   if(param >= 0 && param < (int)m_parameters.size() && m_glslSupport != false)
      glUniform1fARB(m_parameters[param].m_parameter, val);
}


void OpenGLRenderer::SetShaderParameter2f(ParameterHandle param, float x, float y)
{
   if(param >= 0 && param < (int)m_parameters.size() && m_glslSupport != false)
      glUniform2fARB(m_parameters[param].m_parameter, x, y);
}


void OpenGLRenderer::SetShaderParameter3f(ParameterHandle param, float x, float y, float z)
{
   if(param >= 0 && param < (int)m_parameters.size() && m_glslSupport != false)
      glUniform3fARB(m_parameters[param].m_parameter, x, y, z);
}


void OpenGLRenderer::SetShaderParameter4f(ParameterHandle param, float x, float y, float z, float w)
{
   if(param >= 0 && param < (int)m_parameters.size() && m_glslSupport != false)
      glUniform4fARB(m_parameters[param].m_parameter, x, y, z, w);
}


void OpenGLRenderer::ApplyShader(ShaderHandle shader)
{
   if(m_glslSupport == false)
      return;

   if(shader < 0)
      glUseProgramObjectARB(0);
   else
      glUseProgramObjectARB(m_shaders[shader].m_context);
}


void OpenGLRenderer::Render(BB_PRIMITIVE_TYPE type, VertexDescriptor *desc,
                            char *vertexData, unsigned int *indices,
                            int totalVBytes, int totalIndices, int stride)
{
   if(desc == NULL || type == BB_PRIMITIVE_NULL || totalVBytes <= 0 ||
      stride <= 0 || vertexData == NULL)
      {
         return;
      }

   bool isIndexed = false;

   if(indices != NULL && totalIndices > 0)
      {
         isIndexed = true;
      }

   GLint size = 0, status = 0, i = 0;
   bool useArrays = false;
   VertexElement el;
   int currentOffset = 0;
   GLenum clientState = 0, mode = 0;
   unsigned int *ptrIndex = NULL;
   char *ptrVerts = NULL;

   if(m_currentDesc == NULL || m_currentDesc != desc)
      {
         m_currentDesc = desc;

         for(i = 0; i < m_currentDesc->GetTotalElements(); i++)
            {
               el = m_currentDesc->GetElement(i);

               switch(el.m_type)
                  {
                     case BB_ELEMENT_TYPE_VERTEX_3F:
                        clientState = GL_VERTEX_ARRAY;
                     break;

                     case BB_ELEMENT_TYPE_COLOR_3F:
                        clientState = GL_COLOR_ARRAY;
                     break;
                     
                     case BB_ELEMENT_TYPE_NORMAL_3F:
                        clientState = GL_NORMAL_ARRAY;
                     break;
                     
                     case BB_ELEMENT_TYPE_TEX1_2F:
                     case BB_ELEMENT_TYPE_TEX2_2F:
                     case BB_ELEMENT_TYPE_TEX3_2F:
                     case BB_ELEMENT_TYPE_TEX4_2F:
                     case BB_ELEMENT_TYPE_TEX5_2F:
                     case BB_ELEMENT_TYPE_TEX6_2F:
                     case BB_ELEMENT_TYPE_TEX7_2F:
                     case BB_ELEMENT_TYPE_TEX8_2F:
                        if(m_multiTexSupport)
                           {
                              glClientActiveTextureARB(GL_TEXTURE0_ARB +
                                 (el.m_type - BB_ELEMENT_TYPE_TEX1_2F));
                           }

                        clientState = GL_TEXTURE_COORD_ARRAY;
                     break;

                     case BB_ELEMENT_TYPE_TEX1_3F:
                     case BB_ELEMENT_TYPE_TEX2_3F:
                     case BB_ELEMENT_TYPE_TEX3_3F:
                     case BB_ELEMENT_TYPE_TEX4_3F:
                     case BB_ELEMENT_TYPE_TEX5_3F:
                     case BB_ELEMENT_TYPE_TEX6_3F:
                     case BB_ELEMENT_TYPE_TEX7_3F:
                     case BB_ELEMENT_TYPE_TEX8_3F:
                        if(m_multiTexSupport)
                           {
                              glClientActiveTextureARB(GL_TEXTURE0_ARB +
                                 (el.m_type - BB_ELEMENT_TYPE_TEX1_3F));
                           }

                        clientState = GL_TEXTURE_COORD_ARRAY;
                     break;

                     default:
                        clientState = 0;
                     break;
                  }

               if(clientState)
                  glDisableClientState(clientState);
            }
      }

   if(!m_vboSupport)
      {
         useArrays = true;
      }
   else
      {
         glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vbo);

         size = totalVBytes;
         glBufferDataARB(GL_ARRAY_BUFFER_ARB, size, vertexData, GL_STATIC_DRAW_ARB);
         glGetBufferParameterivARB(GL_ARRAY_BUFFER_ARB, GL_BUFFER_SIZE_ARB, &status);

         if(status <= 0)
            {
               useArrays = true;
               glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
            }

         glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_ibo);

         size = sizeof(unsigned int) * totalIndices;
         glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, size, indices, GL_STATIC_DRAW_ARB);
         glGetBufferParameterivARB(GL_ELEMENT_ARRAY_BUFFER_ARB, GL_BUFFER_SIZE_ARB, &status);

         if(status <= 0 && useArrays)
            {
               useArrays = true;
               glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
            }
      }
      
   if(!useArrays)
      {
         if(isIndexed)
            {
               ptrIndex = 0;
            }

         ptrVerts = 0;
      }
   else
      {
         if(isIndexed)
            {
               ptrIndex = indices;
            }

         ptrVerts = vertexData;
      }

   for(i = 0; i < m_currentDesc->GetTotalElements(); i++)
      {
         el = m_currentDesc->GetElement(i);

         switch(el.m_type)
            {
               case BB_ELEMENT_TYPE_IGNORE_2F:
                  currentOffset += sizeof(float) * 2;
                  break;

               case BB_ELEMENT_TYPE_IGNORE_3F:
                  currentOffset += sizeof(float) * 3;
                  break;

               case BB_ELEMENT_TYPE_VERTEX_3F:
                  glEnableClientState(GL_VERTEX_ARRAY);
                  glVertexPointer(3, GL_FLOAT, stride, ptrVerts + currentOffset);
                  currentOffset += sizeof(float) * 3;
               break;

               case BB_ELEMENT_TYPE_COLOR_3F:
                  glEnableClientState(GL_COLOR_ARRAY);
                  glColorPointer(3, GL_FLOAT, stride, ptrVerts + currentOffset);
                  currentOffset += sizeof(float) * 3;
               break;
               
               case BB_ELEMENT_TYPE_NORMAL_3F:
                  glEnableClientState(GL_NORMAL_ARRAY);
                  glNormalPointer(GL_FLOAT, stride, ptrVerts + currentOffset);
                  currentOffset += sizeof(float) * 3;
               break;
               
               case BB_ELEMENT_TYPE_TEX1_2F:
               case BB_ELEMENT_TYPE_TEX2_2F:
               case BB_ELEMENT_TYPE_TEX3_2F:
               case BB_ELEMENT_TYPE_TEX4_2F:
               case BB_ELEMENT_TYPE_TEX5_2F:
               case BB_ELEMENT_TYPE_TEX6_2F:
               case BB_ELEMENT_TYPE_TEX7_2F:
               case BB_ELEMENT_TYPE_TEX8_2F:
                  if(m_multiTexSupport)
                     {
                        glClientActiveTextureARB(GL_TEXTURE0_ARB +
                           (el.m_type - BB_ELEMENT_TYPE_TEX1_2F));

                        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
                        glTexCoordPointer(2, GL_FLOAT, stride, ptrVerts + currentOffset);
                     }

                  currentOffset += sizeof(float) * 2;
               break;

               case BB_ELEMENT_TYPE_TEX1_3F:
               case BB_ELEMENT_TYPE_TEX2_3F:
               case BB_ELEMENT_TYPE_TEX3_3F:
               case BB_ELEMENT_TYPE_TEX4_3F:
               case BB_ELEMENT_TYPE_TEX5_3F:
               case BB_ELEMENT_TYPE_TEX6_3F:
               case BB_ELEMENT_TYPE_TEX7_3F:
               case BB_ELEMENT_TYPE_TEX8_3F:
                  if(m_multiTexSupport)
                     {
                        glClientActiveTextureARB(GL_TEXTURE0_ARB +
                           (el.m_type - BB_ELEMENT_TYPE_TEX1_3F));

                        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
                        glTexCoordPointer(3, GL_FLOAT, stride, ptrVerts + currentOffset);
                     }

                  currentOffset += sizeof(float) * 3;
               break;

               default: break;
            }
      }

   switch(type)
      {
         case BB_PRIMITIVE_POINT_LIST:
            mode = GL_POINTS;
         break;

         case BB_PRIMITIVE_TRI_LIST:
            mode = GL_TRIANGLES;
         break;

         case BB_PRIMITIVE_TRI_STRIP:
            mode = GL_TRIANGLE_STRIP;
         break;

         case BB_PRIMITIVE_LINE_LIST:
            mode = GL_LINES;
         break;

         case BB_PRIMITIVE_LINE_STRIP:
            mode = GL_LINE_STRIP;
         break;

         default:
            break;
      }

   if(isIndexed)
      {
         glDrawElements(mode, totalIndices, GL_UNSIGNED_INT, ptrIndex);
      }
   else
      {
         glDrawArrays(mode, 0, (totalVBytes / stride));
      }


   if(m_vboSupport)
      {
         glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
         glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
      }
}


END_ENGINE_NAMESPACE
