/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#ifndef _BB_RENDER_INTERFACE_H_
#define _BB_RENDER_INTERFACE_H_

#include<NameSpace.h>
#include<Defines.h>
#include<WindowHandle.h>
#include<RTTI.h>
#include<ResourceManager.h>
#include<VertexDescriptor.h>
#include<vector>


DECLARE_ENGINE_NAMESPACE


struct stTexture { };
typedef bbe::Handle<bbe::stTexture> BB_TEXTURE;

typedef int ShaderHandle;
typedef int ParameterHandle;


class RenderParams
{
   public:
      RenderParams()
      {
         m_fullscreen = 0;
         m_width = m_height = 0;
         m_colorBits = m_depthBits = m_stencilBits = 0;
         m_maxCacheBytes = m_maxCacheIndices = 0;
      }

   public:
      bool m_fullscreen;
      int m_width, m_height;
      int m_colorBits, m_depthBits, m_stencilBits;
      int m_maxCacheBytes, m_maxCacheIndices;

      WindowHandle m_handle;
};


class RenderInterface
{
   BB_RTTI_EXPOSE_TYPE;
   
   public:
      RenderInterface()
      {
         m_red = m_green = m_blue = 0;
         m_cubeMapSupport = false;
         m_multiTexSupport = false;
      }

      virtual ~RenderInterface() { }

   public:
      virtual BRESULT Initialize(const RenderParams *params)
      {
         if(params == NULL)
            return BB_FAIL;

         memcpy(&m_params, params, sizeof(RenderParams));

         return BB_SUCCESS;
      }

      virtual void Shutdown() = 0;

   public:
      virtual void SetViewPort(int x, int y, int width, int height) = 0;
      virtual void ResizeWindow(float fov, float nearDist, float farDist,
                                int width, int height) = 0;
      virtual void ResizeWindowOrtho(int left, int right, int bottom,
                                     int top, float nearDist,
                                     float farDist) = 0;

   public:
      virtual void SetClearColor(int r, int g, int b, int a) = 0;
      virtual void StartRendering(bool color, bool depth, bool stencil) = 0;
      virtual void EndRendering() = 0;

      virtual BRESULT Enable(BB_RENDER_STATE state) = 0;
      virtual BRESULT Disable(BB_RENDER_STATE state) = 0;
      virtual void SetAlphaFunc(BB_ALPHA_STATE state, float val) = 0;

   public:
      virtual void SetView(float posX, float posY, float posZ,
                           float lookX, float lookY, float lookZ,
                           float upX, float upY, float upZ) = 0;

      virtual void SetMatrix(const float *matrix) = 0;
      virtual void MultiplyToCurrentMatrix(const float *matrix) = 0;
      virtual BRESULT SwitchMatrixType(BB_MATRIX_TYPE type) = 0;
      virtual void GetMatrix(BB_MATRIX_TYPE type, float *matrix) = 0;

      virtual void RotateMatrix(float angle, float x, float y, float z) = 0;
      virtual void TranslateMatrix(float x, float y, float z) = 0;

      virtual void LoadIdentityMatrix() = 0;
      virtual void PushMatrix() = 0;
      virtual void PopMatrix() = 0;

   public:
      virtual void ApplyFilters(BB_TEXTURE &handle,
                                BB_FILTER_LIST_PTR filters) = 0;

      virtual bool AddEmptyTexture(char *name, int width, int height,
                                   int components, BB_TEXTURE_TYPE type,
                                   BB_TEXTURE_FORMAT format,
                                   BB_TEXTURE *handle) = 0;

      virtual bool LoadTexFromFile(char *fileName,
                                   BB_TEXTURE_TYPE type,
                                   BB_TEXTURE *handle) = 0;

      virtual bool LoadTexFromMemory(char *name, char *fileData,
                                     int length,
                                     BB_TEXTURE_TYPE type,
                                     BB_TEXTURE *handle) = 0;

      virtual bool ApplyTexture(int texUnit,
                                BB_TEXTURE &handle) = 0;

      virtual void DeleteTexture(BB_TEXTURE &handle) = 0;

   public:
      virtual bool CreateShaderFromFile(char *vs, char *ps,
                                        ShaderHandle *handle) = 0;

      virtual bool CreateShaderFromMemory(char *vsFile, char *psFile,
                                          ShaderHandle *handle) = 0;

      virtual void ApplyShader(ShaderHandle shader) = 0;

      virtual void SetupShaderParameter(char *pname, ShaderHandle shader,
                                ParameterHandle *param) = 0;

      virtual void SetShaderParameter1i(ParameterHandle param,
                                        float val) = 0;

      virtual void SetShaderParameter1f(ParameterHandle param,
                                        float val) = 0;

      virtual void SetShaderParameter2f(ParameterHandle param,
                                        float x, float y) = 0;

      virtual void SetShaderParameter3f(ParameterHandle param,
                                        float x, float y,
                                        float z) = 0;

      virtual void SetShaderParameter4f(ParameterHandle param,
                                        float x, float y,
                                        float z, float w) = 0;

   public:
      virtual void Render(BB_PRIMITIVE_TYPE type, VertexDescriptor *desc,
                          char *vertexData, unsigned int *indices,
                          int totalVertices, int totalIndices,
                          int stride) = 0;

   protected:
      int m_red, m_green, m_blue, m_alpha;
      bool m_multiTexSupport, m_cubeMapSupport;

      RenderParams m_params;
};


END_ENGINE_NAMESPACE

#endif