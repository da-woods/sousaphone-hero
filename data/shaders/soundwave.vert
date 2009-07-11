uniform vec4 centre;// = vec4(0.0, 0.0, 0.0, 1.0);

//uniform float time;

varying vec2 distanceFromCentre; // distance in each direction

void main()
{
   gl_Position = ftransform();
   gl_FrontColor = gl_Color;
   gl_TexCoord[0] = gl_MultiTexCoord0;

   vec4 centreOnScreen = gl_ModelViewProjectionMatrix*centre;

   distanceFromCentre = gl_Position.xy - centre.xy;
} 
