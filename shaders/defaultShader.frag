void main ()
{
  vec4 color = gl_Color;
    // Self Coloring
    gl_FragColor = vec4(color.x, color.y, color.z, color.q);

}
