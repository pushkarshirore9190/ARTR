#version 450 core
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec4 vPosition;
layout(location = 1) in vec4 vColor;

layout(location = 0) out vec4 out_Color;

layout(binding = 0) uniform UBO
{
    mat4 modelMatrix;
    mat4 viewMatrix;
    mat4 projectionMatrix;
    float time;
    vec3 padding; // padding for 16-byte alignment
} ubo;

void main(void)
{
    gl_Position = ubo.projectionMatrix * ubo.viewMatrix * ubo.modelMatrix * vPosition;
    out_Color = vColor;
}




