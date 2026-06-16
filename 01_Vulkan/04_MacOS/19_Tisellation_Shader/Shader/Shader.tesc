#version 450 core

#extension GL_ARB_separate_shader_objects : enable

layout (vertices = 4) out;

layout (binding = 0) uniform MyuniformData
{
    mat4 mvpMatrix;
    vec4 No_Of_LineSegments;
    vec4 No_Of_LineStripes;
    vec4 Line_Color;
} myuniformData;

void main(void)
{
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

    float segments = max(myuniformData.No_Of_LineSegments.x, 1.0);

    gl_TessLevelOuter[0] = 1.0;
    gl_TessLevelOuter[1] = segments;
    gl_TessLevelOuter[2] = 1.0;
    gl_TessLevelOuter[3] = segments;

    gl_TessLevelInner[0] = segments;
    gl_TessLevelInner[1] = 1.0;
}




