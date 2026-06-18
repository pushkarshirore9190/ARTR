#version 450 core
#extension GL_ARB_separate_shader_objects : enable

// ===== Inputs from Vertex Shader =====
layout(location = 0) in vec2 out_TexCoord;
layout(location = 1) in vec3 out_transformedNormal;
layout(location = 2) in vec3 out_lightDirection;
layout(location = 3) in vec3 out_viewerVector;

// ===== Output =====
layout(location = 0) out vec4 FragColor;

// ===== Uniform Buffers =====
layout(binding = 0) uniform UniformData
{
    // Matrices
    mat4 modelMatrix;
    mat4 viewMatrix;
    mat4 projectionMatrix;

    // Light properties
    vec4 lightAmbient;
    vec4 lightDiffuse;
    vec4 lightSpecular;
    vec4 lightPosition;

    // Material properties
    vec4 materialAmbient;
    vec4 materialDiffuse;
    vec4 materialSpecular;
    float materialShininess;

    // Key toggle
    uint LKeyIsPressed;
} u_UniformData;

// ===== Texture Sampler =====
layout(binding = 1) uniform sampler2D uTextureSampler;

// ===== Main =====
void main(void)
{
    vec3 phong_ads_light;

    if (u_UniformData.LKeyIsPressed == 1)
    {
        // Normalize vectors
        vec3 N = normalize(out_transformedNormal);
        vec3 L = normalize(out_lightDirection);
        vec3 V = normalize(out_viewerVector);

        // Ambient
        vec3 ambient = vec3(u_UniformData.lightAmbient) * vec3(u_UniformData.materialAmbient);

        // Diffuse
        float diff = max(dot(N, L), 0.0);
        vec3 diffuse = vec3(u_UniformData.lightDiffuse) * vec3(u_UniformData.materialDiffuse) * diff;

        // Specular
        vec3 R = reflect(-L, N);
        float spec = pow(max(dot(R, V), 0.0), u_UniformData.materialShininess);
        vec3 specular = vec3(u_UniformData.lightSpecular) * vec3(u_UniformData.materialSpecular) * spec;

        // Combine Phong components
        phong_ads_light = ambient + diffuse + specular;
    }
    else
    {
        // Lighting disabled → full white
        phong_ads_light = vec3(1.0, 1.0, 1.0);
    }

    // Sample texture
    vec4 texColor = texture(uTextureSampler, out_TexCoord);

    // Combine lighting with texture
    FragColor = vec4(phong_ads_light, 1.0) * texColor;
}


