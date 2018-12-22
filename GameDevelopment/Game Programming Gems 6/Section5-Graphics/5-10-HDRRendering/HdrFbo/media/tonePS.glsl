
varying vec2 texCoords;

uniform samplerRECT fullTexture;
uniform float exposure;
uniform vec2 res;

float gamma = 0.55f;


void main()
{
   vec4 scenePass = f4texRECT(fullTexture, texCoords);

   // Since OpenGl texture rectangles have tex coord between 0
   // and the width/height of the image, we must convert this tex coord
   // to 0 to 1.  If we didn't have to use texture rectangles for floating
   // point textures, then we could specify tex coords the normal way and
   // not have to do this.
   vec2 newTexCoords = texCoords / res;
   
   // Convert from 0 to 1 range to -.5 to .5.
   newTexCoords -= 0.5;

   // Increase the color by the exp then use a pow to convert to 0 to 1 LDR range.
   scenePass = scenePass * exposure;
   gl_FragColor = pow(scenePass, gamma);
}