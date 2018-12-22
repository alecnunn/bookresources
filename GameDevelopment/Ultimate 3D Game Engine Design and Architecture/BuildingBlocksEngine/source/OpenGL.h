/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#ifndef _BB_OPENGL_H_
#define _BB_OPENGL_H_


#include<RenderInterface.h>
#include<GLHeader.h>
#include<vector>


DECLARE_ENGINE_NAMESPACE


struct GLSLShader
{
   GLhandleARB m_context;
   GLhandleARB m_vs;
   GLhandleARB m_ps;
};

struct GLSLParameter
{
   GLuint m_parameter;
};


class OpenGLRenderer : public RenderInterface
{
   BB_RTTI_EXPOSE_TYPE;
   
   public:
      OpenGLRenderer();
      virtual ~OpenGLRenderer();

      BRESULT Initialize(const RenderParams *params);
      void Shutdown();

      void SetViewPort(int x, int y, int width, int height);
      void ResizeWindow(float fov, float nearDist, float farDist,
                        int width, int height);
      void ResizeWindowOrtho(int left, int right, int bottom, int top,
                             float nearDist, float farDist);

      void SetClearColor(int r, int g, int b, int a);
      void StartRendering(bool color, bool depth, bool stencil);
      void EndRendering();


      BRESULT Enable(BB_RENDER_STATE state);
      BRESULT Disable(BB_RENDER_STATE state);
      void SetAlphaFunc(BB_ALPHA_STATE state, float val);

      void SetView(float posX, float posY, float posZ,
                   float lookX, float lookY, float lookZ,
                   float upX, float upY, float upZ);

      void SetMatrix(const float *matrix);
      void MultiplyToCurrentMatrix(const float *matrix);
      BRESULT SwitchMatrixType(BB_MATRIX_TYPE type);
      void GetMatrix(BB_MATRIX_TYPE type, float *matrix);

      void RotateMatrix(float angle, float x, float y, float z);
      void TranslateMatrix(float x, float y, float z);

      void LoadIdentityMatrix();
      void PushMatrix();
      void PopMatrix();

   private:
      unsigned int CreateGLTexture(unsigned char *image,
                                   BB_TEXTURE_TYPE type,
                                   BB_TEXTURE_FORMAT format,
                                   int width, int height,
                                   int components);

   public:
      void ApplyFilters(BB_TEXTURE &handle,
                        BB_FILTER_LIST_PTR filters);

      bool AddEmptyTexture(char *name, int width, int height,
                           int components, BB_TEXTURE_TYPE type,
                           BB_TEXTURE_FORMAT format,
                           BB_TEXTURE *handle);

      bool LoadTexFromFile(char *fileName, BB_TEXTURE_TYPE type,
                           BB_TEXTURE *handle);

      bool LoadTexFromMemory(char *name, char *fileData,
                             int length, BB_TEXTURE_TYPE type,
                             BB_TEXTURE *handle);

      bool ApplyTexture(int texUnit, BB_TEXTURE &handle);

      void DeleteTexture(BB_TEXTURE &handle);


      bool CreateShaderFromFile(char *vs, char *ps,
                                ShaderHandle *handle);

      bool CreateShaderFromMemory(char *vsFile, char *psFile,
                                  ShaderHandle *handle);

      void ApplyShader(ShaderHandle shader);

      void SetupShaderParameter(char *pname, ShaderHandle shader,
                                ParameterHandle *param);

      void SetShaderParameter1i(ParameterHandle param, float val);
      void SetShaderParameter1f(ParameterHandle param, float val);
      void SetShaderParameter2f(ParameterHandle param,
                                float x, float y);
      void SetShaderParameter3f(ParameterHandle param, float x,
                                float y, float z);
      void SetShaderParameter4f(ParameterHandle param, float x,
                                float y, float z, float w);

   private:
      BRESULT InitializeOS(const RenderParams *params);
      void InitializeExtensions();
      void ShutdownOS();

   public:
      void Render(BB_PRIMITIVE_TYPE type, VertexDescriptor *desc,
                  char *vertexData, unsigned int *indices,
                  int totalVertices, int totalIndices,
                  int stride);

   private:
      VertexDescriptor *m_currentDesc;
      bool m_vboSupport, m_fboSupport, m_glslSupport;
      GLuint m_vbo, m_ibo;

      ResourceManager<GLTexture, BB_TEXTURE> *m_texManager;

      std::vector<GLSLShader> m_shaders;
      std::vector<GLSLParameter> m_parameters;
};


END_ENGINE_NAMESPACE

#endif