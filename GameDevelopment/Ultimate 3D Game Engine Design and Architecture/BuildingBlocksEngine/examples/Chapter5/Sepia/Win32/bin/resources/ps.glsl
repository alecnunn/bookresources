/*
   Ultimate Game Engine Design and Architecture
   Allen Sherrod
*/


varying vec2 texCoords;

uniform sampler2D decal;


void main()
{
   vec3 lumConst = vec3(0.30, 0.59, 0.11);
   vec3 light = vec3(1, 0.9, 0.5);
   vec3 dark = vec3(0.2, 0.05, 0);

   vec3 color = texture2D(decal, texCoords);
   float dp = dot(color, lumConst);
   vec3 sepia = lerp(dark, light, dp);
   vec3 luminance = lerp(color, dp, 0.5);
   vec3 final = lerp(luminance, sepia, 0.5);

   gl_FragColor = vec4(final, 1);
}