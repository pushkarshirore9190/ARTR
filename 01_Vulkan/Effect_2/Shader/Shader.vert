#version 450 core
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec4 vPosition;
layout(location = 1) in vec4 vColor;

layout(location = 0) out vec4 out_Color;

layout(binding = 0) uniform UBO {
    mat4 modelMatrix;
    mat4 viewMatrix;
    mat4 projectionMatrix;
    float time;
    vec3 padding;
} ubo;

void main(void)
{
    // Add sine wave to y-position (wavy up/down)
    vec4 jitteredPos = vPosition;

    // Adjust strength and speed here
    float waveSpeed = 5.0;
    float waveAmplitude = 0.05;

    // Use position.x to offset each vertex differently
    jitteredPos.y += sin(ubo.time * waveSpeed + vPosition.x * 10.0) * waveAmplitude;

    gl_Position = ubo.projectionMatrix * ubo.viewMatrix * ubo.modelMatrix * jitteredPos;
    out_Color = vColor;
}

