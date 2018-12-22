/*
   Ultimate Game Engine Design and Architecture
   Allen Sherrod
*/


varying vec3 texCoords;

uniform samplerCube decal;


void main()
{
   vec4 col = textureCubeLod(decal, texCoords, 1.0);
   gl_FragColor = col;
}