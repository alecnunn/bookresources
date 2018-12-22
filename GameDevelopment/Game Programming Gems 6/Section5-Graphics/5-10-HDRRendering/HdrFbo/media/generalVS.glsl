

varying vec2 texCoords;
uniform vec2 offset;


void main()
{
   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
   texCoords = gl_MultiTexCoord0.xy + (offset * 0.5f);
}