#version 110

varying vec2 distanceFromCentre;

const int MAX_WAVES = 16; // maximum number of distortions we can have

const float speed = 1.0;

uniform float time; // s
uniform sampler2D texture;

//const float LARGE_NVE_TIME = -1000.0; // as default value of waves

uniform float timeOfWaves[MAX_WAVES]; /* = 
      { 0.0, LARGE_NVE_TIME, LARGE_NVE_TIME, LARGE_NVE_TIME,
        LARGE_NVE_TIME, LARGE_NVE_TIME, LARGE_NVE_TIME, LARGE_NVE_TIME,
        LARGE_NVE_TIME, LARGE_NVE_TIME, LARGE_NVE_TIME, LARGE_NVE_TIME,
        LARGE_NVE_TIME, LARGE_NVE_TIME, LARGE_NVE_TIME, LARGE_NVE_TIME };*/

vec2 getCoordOffsets(in float time)
{
   const float EXP_CONST = 20.0;
   const float SCALE_CONST = 0.2;

   vec2 directionVec = normalize(distanceFromCentre);
   vec2 offset = vec2(0.0);

   for (int i = 0; i < MAX_WAVES; ++i)
   {
      float distanceTravelled = (time - timeOfWaves[i]) * speed;
      //vec2 wavePos = distanceTravelled * directionVec;

      float difference = distanceTravelled - length(distanceFromCentre);
      float scalarOffset = SCALE_CONST*difference * exp(-EXP_CONST*abs(difference));
      vec2 vectorOffset = directionVec * scalarOffset;
      float attenuation = 1.0 / (distanceTravelled + 0.1);
      offset += (vectorOffset * attenuation);
   }
   return offset;
}

void main()
{
   vec2 texCoords = gl_TexCoord[0].st;
   texCoords += getCoordOffsets(time);
   gl_FragColor = gl_Color * texture2D(texture,texCoords);
} 
