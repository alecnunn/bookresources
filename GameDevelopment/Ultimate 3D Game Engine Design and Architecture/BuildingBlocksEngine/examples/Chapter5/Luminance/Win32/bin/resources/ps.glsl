/*
   Ultimate Game Engine Design and Architecture
   Allen Sherrod
*/


varying vec2 texCoords;

uniform sampler2D decal;


void main()
{
   vec3 lumConst = vec3(0.30, 0.59, 0.11);

   vec3 color = texture2D(decal, texCoords);
   float dp = dot(color, lumConst);

   gl_FragColor = vec4(dp, dp, dp, 1);
}