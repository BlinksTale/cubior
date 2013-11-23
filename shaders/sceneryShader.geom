#version 410
/*
precision highp float;

in VertexData
{
    vec3 color;
} vertex[];

out vec3 fragmentColor;

layout (triangles) in;
layout (triangle_strip) out;
layout (max_vertices = 3) out;

void main(void)
{
    for (int i = 0; i < gl_in.length(); i++)   //note the '>'
    {
        gl_Position = gl_in[i].gl_Position;
        fragmentColor = vertex[i].color;
        EmitVertex();
    }

    EndPrimitive();
}
*/

layout (points) in;
layout (points) out;
layout (max_vertices = 1) out;

struct vData
{
    vec3 normal;
    vec4 color;
};

in vData vertex[];
out vData frag;


void main()
{
    int i;
    for(i = 0;i < gl_in.length();i++)
    {
        frag.normal = vertex[i].normal;
        frag.color = vertex[i].color;
        gl_Position = gl_in[i].gl_Position;
        EmitVertex();
    }
    EndPrimitive();
}