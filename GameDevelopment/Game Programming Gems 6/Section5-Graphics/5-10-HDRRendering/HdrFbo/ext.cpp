/*
   Allen Sherrod
   HDR Rendering with Frame Buffer Objects
   September 1, 2005
*/


#include<windows.h>
#include<gl/gl.h>
#include<gl/glu.h>
#include<gl/glext.h>
#include"ext.h"


// GLSlang functions.
PFNGLCREATEPROGRAMOBJECTARBPROC glCreateProgramObjectARB = NULL;
PFNGLCREATESHADEROBJECTARBPROC glCreateShaderObjectARB = NULL;
PFNGLLINKPROGRAMARBPROC glLinkProgramARB = NULL;
PFNGLCOMPILESHADERARBPROC glCompileShaderARB = NULL;
PFNGLGETINFOLOGARBPROC glGetInfoLogARB = NULL;
PFNGLDELETEOBJECTARBPROC glDeleteObjectARB = NULL;
PFNGLUSEPROGRAMOBJECTARBPROC glUseProgramObjectARB = NULL;
PFNGLSHADERSOURCEARBPROC glShaderSourceARB = NULL;
PFNGLATTACHOBJECTARBPROC glAttachObjectARB = NULL;
PFNGLGETOBJECTPARAMETERIVARBPROC glGetObjectParameterivARB = NULL;
PFNGLGETUNIFORMLOCATIONARBPROC glGetUniformLocationARB = NULL;
PFNGLUNIFORM4FARBPROC glUniform4fARB = NULL;
PFNGLUNIFORM2FARBPROC glUniform2fARB = NULL;
PFNGLUNIFORM1FARBPROC glUniform1fARB = NULL;
PFNGLUNIFORM1IARBPROC glUniform1iARB = NULL;

// FBO functions.
PFNGLGENFRAMEBUFFERSEXTPROC glGenFramebuffersEXT = NULL;
PFNGLBINDFRAMEBUFFEREXTPROC glBindFramebufferEXT = NULL;
PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC glCheckFramebufferStatusEXT = NULL;
PFNGLFRAMEBUFFERTEXTURE2DEXTPROC glFramebufferTexture2DEXT = NULL;
PFNGLDELETEFRAMEBUFFERSEXTPROC glDeleteFramebuffersEXT = NULL;
PFNGLBINDRENDERBUFFEREXTPROC glBindRenderbufferEXT = NULL;
PFNGLGENRENDERBUFFERSEXTPROC glGenRenderbuffersEXT = NULL;
PFNGLRENDERBUFFERSTORAGEEXTPROC glRenderbufferStorageEXT = NULL;
PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC glFramebufferRenderbufferEXT = NULL;
PFNGLDELETERENDERBUFFERSEXTPROC glDeleteRenderbuffersEXT = NULL;


bool SetupGLSL()
{
   char *extensions = (char*)glGetString(GL_EXTENSIONS);

   // Check for the neccessary extensions.
   if(strstr(extensions, "GL_ARB_shading_language_100") == NULL)
      return false;

   if(strstr(extensions, "GL_ARB_shader_objects") == NULL)
      return 0;

   glCreateProgramObjectARB = (PFNGLCREATEPROGRAMOBJECTARBPROC)wglGetProcAddress("glCreateProgramObjectARB");
   glCreateShaderObjectARB = (PFNGLCREATESHADEROBJECTARBPROC)wglGetProcAddress("glCreateShaderObjectARB");
   glCompileShaderARB = (PFNGLCOMPILESHADERARBPROC)wglGetProcAddress("glCompileShaderARB");
   glLinkProgramARB = (PFNGLLINKPROGRAMARBPROC)wglGetProcAddress("glLinkProgramARB");
   glGetInfoLogARB = (PFNGLGETINFOLOGARBPROC)wglGetProcAddress("glGetInfoLogARB");
   glDeleteObjectARB  = (PFNGLDELETEOBJECTARBPROC)wglGetProcAddress("glDeleteObjectARB");
   glUseProgramObjectARB = (PFNGLUSEPROGRAMOBJECTARBPROC)wglGetProcAddress("glUseProgramObjectARB");
   glShaderSourceARB = (PFNGLSHADERSOURCEARBPROC)wglGetProcAddress("glShaderSourceARB");
   glAttachObjectARB = (PFNGLATTACHOBJECTARBPROC)wglGetProcAddress("glAttachObjectARB");
   glGetObjectParameterivARB = (PFNGLGETOBJECTPARAMETERIVARBPROC)wglGetProcAddress("glGetObjectParameterivARB");
   glGetUniformLocationARB = (PFNGLGETUNIFORMLOCATIONARBPROC)wglGetProcAddress("glGetUniformLocationARB");
   glUniform4fARB = (PFNGLUNIFORM4FARBPROC)wglGetProcAddress("glUniform4fARB");
   glUniform2fARB = (PFNGLUNIFORM2FARBPROC)wglGetProcAddress("glUniform2fARB");
   glUniform1fARB = (PFNGLUNIFORM1FARBPROC)wglGetProcAddress("glUniform1fARB");
   glUniform1iARB = (PFNGLUNIFORM1IARBPROC)wglGetProcAddress("glUniform1iARB");

   if(!glCreateProgramObjectARB || !glCreateShaderObjectARB ||  !glCompileShaderARB ||
      !glLinkProgramARB || !glGetInfoLogARB || !glDeleteObjectARB || !glUseProgramObjectARB ||
      !glShaderSourceARB || !glAttachObjectARB || !glGetObjectParameterivARB ||
      !glGetUniformLocationARB || !glUniform4fARB || !glUniform2fARB ||
      !glUniform1iARB || !glUniform1fARB)
      {
         return false;
      }

   return true;
}


bool CreateShader(char *VShader, char *PShader, GLhandleARB *glContext)
{
   char error[4096];
   int result;
   GLhandleARB glVertexShader;
   GLhandleARB glPixelShader;

   // Create the program object.
   *glContext = glCreateProgramObjectARB();

   // Load the vertex shader.
   char *ShaderCode = loadShader(VShader);
   if(!ShaderCode) return false;
   glVertexShader = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
   glShaderSourceARB(glVertexShader, 1, (const char**)&ShaderCode, NULL);
   glCompileShaderARB(glVertexShader);
   glGetObjectParameterivARB(glVertexShader, GL_OBJECT_COMPILE_STATUS_ARB, &result);
   glAttachObjectARB(*glContext, glVertexShader);

   // No londer needed.
   delete[] ShaderCode;
   ShaderCode = NULL;

   // Load the pixel shader.
   ShaderCode = loadShader(PShader);
   if(!ShaderCode) return false;
   glPixelShader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
   glShaderSourceARB(glPixelShader, 1, (const char**)&ShaderCode, NULL);
   glCompileShaderARB(glPixelShader);
   glGetObjectParameterivARB(glPixelShader, GL_OBJECT_COMPILE_STATUS_ARB, &result);
   glAttachObjectARB(*glContext, glPixelShader);

   // No longer needed.
   delete[] ShaderCode;
   ShaderCode = NULL;

   // Link our shaders.
   glLinkProgramARB(*glContext);
   glGetObjectParameterivARB(*glContext, GL_OBJECT_LINK_STATUS_ARB, &result);

   // Display any compile errors.
   if(!result)
      {
         glGetInfoLogARB(*glContext, sizeof(error), NULL, error);
         MessageBox(NULL, error, "Error linking shaders...", MB_OK);
         return false;
      }

   // Temp variables no longer needed.
   glDeleteObjectARB(glVertexShader);
   glDeleteObjectARB(glPixelShader);

   return true;
}


char* loadShader(char *shaderName)
{
   char tempLine[256] = {0};
   int count = 0;

   // input object.
   ifstream tempInput;

   // Open the shader file.
   tempInput.open(shaderName);

   // If there is any errors then return false.
   if(!tempInput.is_open()) return NULL;

   // Loop through each line of the file and get the total size.
   while(!tempInput.eof())
      {
         tempInput.getline(tempLine, 256, '\n');
         count++;
      }

   // Close the shader.
   tempInput.close();

   if(count == 0) return NULL;

   // Read in the data for use this time.
   ifstream input;

   // Create array to hold shader code.
   char *ShaderSource = new char[256 * count];

   // Re-open the shader and read in the whole thing into the array.
   input.open(shaderName);
   input.getline(ShaderSource , 256 * count, '\0');

   input.close(); // Close the shader.

   return ShaderSource;
}


bool SetupFBO()
{
   char *extension = (char*)glGetString(GL_EXTENSIONS);

   // Need this extension or we can't run the application.
   if(strstr(extension, "GL_EXT_framebuffer_object") == NULL) return false;

   // Get FBO functions.
   glGenFramebuffersEXT = (PFNGLGENFRAMEBUFFERSEXTPROC)wglGetProcAddress("glGenFramebuffersEXT");
   glBindFramebufferEXT = (PFNGLBINDFRAMEBUFFEREXTPROC)wglGetProcAddress("glBindFramebufferEXT");
   glCheckFramebufferStatusEXT = (PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC)wglGetProcAddress("glCheckFramebufferStatusEXT");
   glFramebufferTexture2DEXT = (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC)wglGetProcAddress("glFramebufferTexture2DEXT");
   glDeleteFramebuffersEXT = (PFNGLDELETEFRAMEBUFFERSEXTPROC)wglGetProcAddress("glDeleteFramebuffersEXT");
   glGenRenderbuffersEXT = (PFNGLGENRENDERBUFFERSEXTPROC)wglGetProcAddress("glGenRenderbuffersEXT");
   glBindRenderbufferEXT = (PFNGLBINDRENDERBUFFEREXTPROC)wglGetProcAddress("glBindRenderbufferEXT");
   glRenderbufferStorageEXT = (PFNGLRENDERBUFFERSTORAGEEXTPROC)wglGetProcAddress("glRenderbufferStorageEXT");
   glFramebufferRenderbufferEXT = (PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC)wglGetProcAddress("glFramebufferRenderbufferEXT");
   glDeleteRenderbuffersEXT = (PFNGLDELETERENDERBUFFERSEXTPROC)wglGetProcAddress("glDeleteRenderbuffersEXT");

   // Error checking.
   if(!glGenFramebuffersEXT || !glGenRenderbuffersEXT || !glBindFramebufferEXT || !glBindRenderbufferEXT ||
      !glCheckFramebufferStatusEXT || !glFramebufferTexture2DEXT || !glDeleteFramebuffersEXT ||
      !glRenderbufferStorageEXT || !glFramebufferRenderbufferEXT || !glDeleteRenderbuffersEXT) return false;

   return true;
}