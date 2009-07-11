varying float facing; // reversed if <1;

uniform sampler2D texture;

uniform vec2 frameInfo; // [0] = frameRate (fps)
                        // [1] = noOfFrames

uniform float time; // in sec

vec2 texCoordDirection(in vec2 coord)
{
   if (facing < 0.0)
   {
      coord.s = -coord.s;
   }
   return coord;
}

vec2 getAnimationCoords(in vec2 coord)
{
   float frameRate = frameInfo[0];
   float noOfFrames = frameInfo[1];

   float activeFrame = floor(mod(frameRate*time, noOfFrames));
   //float offset = (1.0/noOfFrames)*activeFrame;

   coord.s = (coord.s + activeFrame) / noOfFrames;

   return coord;
}

void main()
{
   gl_FragColor = gl_Color * texture2D(texture, getAnimationCoords(texCoordDirection(gl_TexCoord[0].st)));
} 
