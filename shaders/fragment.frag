varying vec4 point;

float convertToCake(float value)
{
    return mod(value/100.0+0.499, 1.0);
}

void main ()
{
    //gl_FragColor = gl_Color;//vec4(1.0, 0.0, 0.0, 1.0);
    //vec4 point = gl_Vertex;
    //mod(point.y+500.0,1.0)
    vec4 color = gl_Color;
    
    float shadowStrength = 0.5;//0.25;

    // For Black
    float shadow = (shadowStrength - convertToCake(point.y) * (shadowStrength));

    // Fade to Black
    gl_FragColor = vec4(color.x - shadow, color.y - shadow, color.z - shadow, color.q);
    
    // Self Coloring
    //gl_FragColor = vec4(color.x, color.y, color.z, color.q);
    // Fade to Awesome
    //gl_FragColor = vec4(convertToCake(point.x), convertToCake(point.y), convertToCake(point.z), 1.0);
    
}
