#version 300 es
layout(location = 0) in vec4 a_position;
layout(location = 1) in vec4 a_texCoord;
uniform mat4 u_MVPMatrix;
uniform mat4 u_MVPMatrix_Next;
out vec4 v_texCoord;
void main()
{
    gl_Position = u_MVPMatrix * a_position;
    v_texCoord = a_texCoord * u_MVPMatrix_Next;
}