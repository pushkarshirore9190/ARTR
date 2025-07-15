#version 450 core
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec4 out_Color;
layout(location = 0) out vec4 FragColor;

layout(binding = 0) uniform UBO {
    mat4 modelMatrix;
    mat4 viewMatrix;
    mat4 projectionMatrix;
    float time;
    vec3 padding;
} ubo;

// Simple hash function for noise
float hash(vec2 p) {
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453123);
}

// 2D noise function
float noise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);

    float a = hash(i);
    float b = hash(i + vec2(1.0, 0.0));
    float c = hash(i + vec2(0.0, 1.0));
    float d = hash(i + vec2(1.0, 1.0));

    vec2 u = f * f * (3.0 - 2.0 * f);

    return mix(a, b, u.x) +
           (c - a) * u.y * (1.0 - u.x) +
           (d - b) * u.x * u.y;
}

void main(void)
{
    // Normalize fragment coords for UV (adjust for your resolution)
    vec2 uv = gl_FragCoord.xy / vec2(800.0, 600.0);

    // Animate noise with time
    float n = noise(uv * 10.0 + ubo.time * 0.5);

    // Distort the input color with noise, scaling between 0.5 and 1.0
    vec3 distortedColor = out_Color.rgb * (0.5 + 0.5 * n);

    FragColor = vec4(distortedColor, out_Color.a);
}

