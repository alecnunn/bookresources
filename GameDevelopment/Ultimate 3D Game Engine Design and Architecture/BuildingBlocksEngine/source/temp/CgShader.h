/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#ifndef _CG_SHADER_H_
#define _CG_SHADER_H_

#include<cg/cg.h>


DECLARE_ENGINE_NAMESPACE


class CgParameter
{
   public:
      CgParameter()
      {
         m_parameter = 0;
      }


      void Initialize(char *pname, CGprogram shader)
      {
         m_parameter = cgGetNamedParameter(shader, pname);
      }


      CGparameter GetParameter()
      {
         return m_parameter;
      }


   private:
      CGparameter m_parameter;
};


class CgShader
{
   public:
      CgShader()
      {
         m_vs = 0;
         m_ps = 0;
      }
      
      ~CgShader()
      {

      }


      bool LoadShaderFromFile(const char *file, CGcontext &context,
                              CGprofile &vsProfile, CGprofile &psProfile)
      {
         m_vs = cgCreateProgramFromFile(context, CG_SOURCE, file,
                                        vsProfile, "mainVS", NULL);

         m_ps = cgCreateProgramFromFile(context, CG_SOURCE, file,
                                        psProfile, "mainPS", NULL);

         if(m_vs == 0 || m_ps == 0)
            return false;

         return true;
      }


      bool LoadShaderFromMemory(const char *data, CGcontext &context,
                                CGprofile &vsProfile, CGprofile &psProfile)
      {
         m_vs = cgCreateProgram(context, CG_SOURCE, data,
                                vsProfile, "mainVS", NULL);

         m_ps = cgCreateProgram(context, CG_SOURCE, data,
                                psProfile, "mainPS", NULL);

         if(m_vs == 0 || m_ps == 0)
            return false;

         return true;
      }


      void Shutdown()
      {
         cgDestroyProgram(m_vs);
         cgDestroyProgram(m_ps);
      }


      CGprogram GetVertexShader() { return m_vs; }
      CGprogram GetPixelShader() { return m_ps; }

   private:
      CGprogram m_vs;
      CGprogram m_ps;
};


END_ENGINE_NAMESPACE

#endif