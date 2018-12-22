/*
   Ultimate Game Engine Design and Architecture
   Allen Sherrod
*/


varying vec2 texCoords;

uniform sampler2D decal;


void main()
{
   gl_FragColor = texture2D(decal, texCoords);
}