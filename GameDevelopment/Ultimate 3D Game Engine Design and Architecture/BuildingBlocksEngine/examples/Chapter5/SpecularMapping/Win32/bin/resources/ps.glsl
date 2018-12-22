/*
   Ultimate Game Engine Design and Architecture
   Allen Sherrod
*/


varying vec2 texCoords;
varying vec3 pos;
varying vec3 normal;

uniform sampler2D decal;
uniform vec4 lightPos;
uniform vec4 lightColor;
uniform vec4 camPos;
uniform float shininess;


void main()
{
   vec3 lightVec = normalize(lightPos - pos);
   vec3 n = normalize(normal);

   float d = dot(lightVec.xyz, lightVec.xyz);
   float attenuationFactor = 1 / (d * d);

   float dp = max(dot(n, lightVec.xyz), 0);
   vec4 diffuse = vec4(attenuationFactor * lightColor.xyz * dp, 1);

   vec3 eyeVec = normalize(camPos.xyz - pos);
   vec3 halfVec = normalize(lightVec.xyz + eyeVec);
   float specularLight = pow(max(dot(n, halfVec), 0), shininess);
   vec4 specular = vec4(attenuationFactor * lightColor.xyz * specularLight, 1);

   vec4 col = texture2D(decal, texCoords).rgba;

   if(col.a == 0)
      specular = vec4(0, 0, 0, 0);

   gl_FragColor = col * (0.3 + diffuse + specular);
}