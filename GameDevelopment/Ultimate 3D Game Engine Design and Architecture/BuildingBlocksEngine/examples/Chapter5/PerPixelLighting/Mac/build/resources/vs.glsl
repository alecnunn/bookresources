/*
   Ultimate Game Engine Design and Architecture
   Allen Sherrod
*/


varying vec2 texCoords;
varying vec3 pos;
varying vec3 normal;


void main()
{
   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
   pos = gl_Vertex.xyz;
   normal = gl_NormalMatrix * gl_Normal;

   texCoords = gl_MultiTexCoord0.xy;
}