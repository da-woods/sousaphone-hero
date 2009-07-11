// in this program we ignore only use the z component of the vertex position and use the x and y as an offset

uniform vec4 ranges; // defines the x and y ranges the insect must appear between
                       // in the sense of mix1 = indix 1, maxx index2, miny index3, maxy index4

uniform vec4 insectParams;   // (should remain constant between calls)
                             // [0] = speed
                             // [1] = wobble frequency
                             // [2] = wobble amplitude
                             // [3] = seed

uniform float time;

varying float facing; // reversed if <1;

float calculatePointSize() // scales point size with x axis and clips to allowed values
{
   float pointSize = (gl_Point.size / gl_NormalMatrix[0][0]); // scales with the x axis
   pointSize = clamp(pointSize, gl_Point.sizeMin, gl_Point.sizeMax);

   return pointSize;
}

float noisedw(in float seed)
{
   //SEED = (1103515245*SEED + 12345) mod 2**15
   float seed2 = mod((214013.0*seed + 2531011.0),32768.0);
   float seed3 = mod((214013.0*seed2 + 2531011.0),32768.0);

   return seed2 + (seed3/32768.0);
}

vec2 noisedw2(in float seed)
{
   vec2 answer;
   answer[0] = noisedw(seed);
   answer[1] = noisedw(answer[0]);
   return answer;
}

vec4 noisedw4(in float seed)
{
   vec4 answer;
   answer[0] = noisedw(seed);
   answer[1] = noisedw(answer[0]);
   answer[2] = noisedw(answer[1]);
   answer[3] = noisedw(answer[2]);
   return answer;
}

void generateConstants(out vec4 constants1, out vec4 constants2, out vec4 wobbleConsts, out vec4 startingPos)
{
   float seed = insectParams[3];
   constants1 = fract(noisedw4(seed));
   constants2 = fract(noisedw4(constants1[3]));
   wobbleConsts = 0.9 + mod(noisedw4(constants2[3]),0.2); // 10% +/- about 100
   startingPos = mod(noisedw4(wobbleConsts[3]),10.0);
   return;
}

vec4 getPosition(const in vec4 constants1, const in vec4 constants2, const in vec4 wobbleConsts, const in vec4 startingPos)
{
   //const float time = insectParams[0];
   float speed = insectParams[0];

   vec4 internalConsts = constants2;// / 32768.0;
   //vec4(0.123,0.765,0.425,1.0);//mod(constants2,1.1111111111111);//fract(constants2);

   float x = constants1[0]*sin(speed*time*internalConsts[0] + startingPos[0])
           + constants1[1]*sin(speed*time*internalConsts[1] + startingPos[1]);
   float xVariation = abs(constants1[0]) + abs(constants1[1]); // x is in the range -xVariation -> xVariation

   // bind it to xRange
   //x = ((x - xMin)/(xMax-xMin)) * (ranges[1] - ranges[0]) + ranges[0];
   x = mix(ranges[0], ranges[1], (x+xVariation)/(2.0*xVariation));

   float baseY = constants1[2]*sin(speed*time*internalConsts[2] + startingPos[2])
               + constants1[3]*sin(speed*time*internalConsts[3] + startingPos[3]);
   float yVariation = abs(constants1[2]) + abs(constants1[3]);

   //baseY = ((baseY-yMin)/(yMax-yMin)) * (ranges[3] - ranges[2]) + ranges[2];
   baseY = mix(ranges[2], ranges[3], (baseY+yVariation)/(2.0*yVariation));

   // assume we wobble twice per second
   float wobbleFreq = insectParams[1];
   float wobbleAmount = insectParams[2]/2.0;
   float wobbleY = wobbleAmount*wobbleConsts[0]*sin(time*wobbleFreq*wobbleConsts[1] + startingPos[0] + startingPos[1]) +
                   wobbleAmount*wobbleConsts[2]*sin(time*wobbleFreq*wobbleConsts[3] + startingPos[2] + startingPos[3]);

   return vec4(x+gl_Vertex.x, baseY+wobbleY+gl_Vertex.y, gl_Vertex.z, 1.0);
}

// <0 is reversed direction
float getDirection(const in vec4 constants1, const in vec4 constants2, const in vec4 startingPos)
{
   float speed = insectParams[0];

   // essentially dx/dt
   float x = constants1[0]*speed*constants2[0]*cos(speed*time*constants2[0] + startingPos[0])
                 + constants1[1]*speed*constants2[1]*cos(speed*time*constants2[1] + startingPos[1]);

   return x;
}

void main()
{
   vec4 constants1, constants2, wobbleConsts, startingPos;

   generateConstants(constants1, constants2, wobbleConsts, startingPos);
   constants1 = vec4(1.0,1.0,1.0,1.0);
   vec4 newVertex = getPosition(constants1, constants2, wobbleConsts, startingPos);
   facing = getDirection(constants1, constants2, startingPos);

   //gl_FrontColor = constants2;///32768.0;
   gl_FrontColor = vec4(1.0,1.0,1.0,1.0);
   gl_PointSize = calculatePointSize();
   gl_Position = gl_ModelViewProjectionMatrix * newVertex;
}
