/*
   Ultimate Game Engine Design and Architecture
   Allen Sherrod
*/


varying vec4 color;

uniform vec4 lightPos;
uniform vec4 lightColor;


void main()
{
   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

   vec3 n = gl_NormalMatrix * gl_Normal;

   vec4 lightVec = normalize(gl_Vertex - lightPos);
   float dp = max(dot(n, lightVec.xyz), 0);
   color = lightColor * dp;
}