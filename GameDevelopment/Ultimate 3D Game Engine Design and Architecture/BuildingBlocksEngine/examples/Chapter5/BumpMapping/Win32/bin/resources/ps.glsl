/*
   Ultimate Game Engine Design and Architecture
   Allen Sherrod
*/


varying vec2 texCoords;
varying vec3 pos;
varying vec3 normal;
varying vec3 sTangent;

uniform sampler2D decal;
uniform sampler2D bump;
uniform vec4 lightPos;
uniform vec4 lightColor;


void main()
{
   vec3 lightVec = lightPos - pos;
   vec3 n = normalize(normal);

   vec3 binormal = cross(sTangent, n);
   mat3 tbnMatrix = mat3(sTangent, binormal, n);

   lightVec = tbnMatrix * lightVec;
   lightVec = normalize(lightVec);

   n = texture2D(bump, texCoords).xyz;
   n = (n - 0.5) * 2;

   float d = dot(lightVec.xyz, lightVec.xyz);
   float attenuationFactor = 1 / (d * d);

   float dp = max(dot(n, lightVec.xyz), 0);
   vec4 diffuse = vec4(attenuationFactor * lightColor.xyz * dp, 1);

   vec4 col = texture2D(decal, texCoords);

   gl_FragColor = col * (0.3 + diffuse);
}