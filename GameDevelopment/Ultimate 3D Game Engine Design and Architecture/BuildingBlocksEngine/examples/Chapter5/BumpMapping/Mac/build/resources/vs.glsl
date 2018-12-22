/*
   Ultimate Game Engine Design and Architecture
   Allen Sherrod
*/


varying vec2 texCoords;
varying vec4 pos;
varying vec3 normal;
varying vec4 sTangent;


void main()
{
   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

   pos = gl_Vertex;
   normal = gl_NormalMatrix * gl_Normal;
   sTangent = gl_Color;
   texCoords = gl_MultiTexCoord0.xy;
}