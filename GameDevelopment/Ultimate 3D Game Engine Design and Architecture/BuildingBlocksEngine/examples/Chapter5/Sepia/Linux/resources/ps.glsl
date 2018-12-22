/*
   Ultimate Game Engine Design and Architecture
   Allen Sherrod
*/


varying vec2 texCoords;

uniform sampler2D decal;


void main()
{
   vec3 lumConst = vec3(0.30, 0.59, 0.11);
   vec4 light = vec4(1.0, 0.9, 0.5, 1.0);
   vec4 dark = vec4(0.2, 0.05, 0.0, 1.0);

   vec4 color = texture2D(decal, texCoords);
   float dp = dot(color.xyz, lumConst);
   
   vec4 sepia = lerp(dark, light, dp);
   vec3 luminance = lerp(color.xyz, dp, 0.5);
   vec3 final = lerp(luminance, sepia.xyz, 0.5);

   gl_FragColor = vec4(final, 1.0);
}