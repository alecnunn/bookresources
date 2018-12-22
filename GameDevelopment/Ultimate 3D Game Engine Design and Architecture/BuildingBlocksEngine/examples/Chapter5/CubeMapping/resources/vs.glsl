/*
   Ultimate Game Engine Design and Architecture
   Allen Sherrod
*/


varying vec3 texCoords;


void main()
{
   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
   texCoords = gl_MultiTexCoord0.xyz;
}