/*
   Ultimate Game Engine Design and Architecture
   Allen Sherrod
*/


varying vec2 texCoords;

uniform sampler2D decal;
uniform sampler2D decal2;


void main()
{
   gl_FragColor = texture2D(decal, texCoords) *
                  texture2D(decal2, texCoords);
}