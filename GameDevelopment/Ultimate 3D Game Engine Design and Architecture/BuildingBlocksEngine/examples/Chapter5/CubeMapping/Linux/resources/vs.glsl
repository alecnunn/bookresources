/*
   Ultimate Game Engine Design and Architecture
   Allen Sherrod
*/


varying vec3 texCoords;


void main()
{
   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
   texCoords = vec3(gl_MultiTexCoord0);
}


