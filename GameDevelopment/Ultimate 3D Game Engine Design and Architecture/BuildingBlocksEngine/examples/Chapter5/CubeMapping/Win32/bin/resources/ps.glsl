/*
   Ultimate Game Engine Design and Architecture
   Allen Sherrod
*/


varying vec3 texCoords;

uniform samplerCube decal;


void main()
{
   gl_FragColor = textureCubeLod(decal, texCoords.xyz, 0);
}