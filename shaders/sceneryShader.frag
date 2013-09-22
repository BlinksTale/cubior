// External variables
uniform float time;

// Received variables
varying vec4 point;

// Local variables
float tinyAmount = 0.000001;

float convertToCake(float value)
{
    return mod(value/100.0+0.5-tinyAmount, 1.0);
}

bool getAlternatingSpot(float x1, float y1, float z1) { // return if in a checker or not
  float delta = 50.0 - tinyAmount;
  float x=x1/1.0+delta;
  float y=y1/1.0+delta;
  float z=z1/1.0+delta; // 49 worked

  float altSize = 400.0; // how wide the checker patterns are
  bool alternatingSpot =( // calculate if given a dark checker spot or not
      (x<0.0) != ((mod(abs(x+tinyAmount),altSize*2.0)<altSize))
    ) != (
      (y<0.0) != ((mod(abs(y+tinyAmount),altSize*2.0)<altSize))
    ) != (
      (z<0.0) != ((mod(abs(z+tinyAmount),altSize*2.0)<altSize))
    );
  return alternatingSpot;
}

void main ()
{
    //gl_FragColor = gl_Color;//vec4(1.0, 0.0, 0.0, 1.0);
    //vec4 point = gl_Vertex;
    //mod(point.y+500.0,1.0)
    vec4 color = gl_Color;
    
    float shadowStrength = 0.5;//0.25;

    // For Checker Pattern
    float altShadow = getAlternatingSpot(point.x, point.y, point.z) ? 0.125 : 0.0;

    // For Black
    float shadow = (shadowStrength - convertToCake(point.y) * (shadowStrength)) + altShadow;

    // Fade to Black
    gl_FragColor = vec4(color.x - shadow, color.y - shadow, color.z - shadow, color.q);
    
    // Self Coloring
    //gl_FragColor = vec4(color.x, color.y, color.z, color.q);
    // Fade to Awesome
    //gl_FragColor = vec4(convertToCake(point.x), convertToCake(point.y), convertToCake(point.z), 1.0);
    
}
