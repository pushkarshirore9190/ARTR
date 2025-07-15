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
    gl_Position = ubo.projectionMatrix * ubo.viewMatrix * ubo.modelMatrix * vPosition;

    // 🎨 Animate per-vertex color using position and time
    float r = abs(sin(ubo.time + vPosition.x * 5.0));
    float g = abs(sin(ubo.time + vPosition.y * 5.0));
    float b = abs(sin(ubo.time + vPosition.x * 5.0 + vPosition.y * 5.0));

    out_Color = vec4(r, g, b, 1.0);
}

