/*
   Ultimate Game Engine Design and Architecture
   Allen Sherrod
*/


varying vec2 texCoords;
varying vec3 pos;
varying vec3 normal;
varying vec3 sTangent;


void main()
{
   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

   pos = vec3(gl_Vertex.xyz);
   normal = gl_NormalMatrix * gl_Normal;
   sTangent = vec3(gl_Color.xyz);
   texCoords = gl_MultiTexCoord0.xy;
}