varying vec4 point;

void main()
{
	gl_FrontColor = gl_Color;
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    point = gl_Vertex; // gl_Vertex is the 3d world position
    // the flat rendered coord is gl_Position
}