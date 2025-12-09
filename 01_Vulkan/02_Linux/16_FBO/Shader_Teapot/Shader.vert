#version 450 core
#extension GL_ARB_separate_shader_objects : enable

// ===== Vertex Inputs =====
layout(location = 0) in vec3 vPosition;   // Position (binding 0)
layout(location = 1) in vec3 vNormal;     // Normal (binding 1)
layout(location = 2) in vec2 vTexCoord;   // Texture (binding 2)

// ===== Vertex Outputs =====
layout(location = 0) out vec2 out_TexCoord;
layout(location = 1) out vec3 out_transformedNormal;
layout(location = 2) out vec3 out_lightDirection;
layout(location = 3) out vec3 out_viewerVector;

// ===== Uniform Buffer (binding = 0) =====
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

    // Key press toggle
    uint LKeyIsPressed;
} u_UniformData;

// ===== Main =====
void main(void)
{
    // MVP transformation
    gl_Position = u_UniformData.projectionMatrix *
                  u_UniformData.viewMatrix *
                  u_UniformData.modelMatrix *
                  vec4(vPosition, 1.0);

    // Pass texture coordinates
    out_TexCoord = vTexCoord;

    // If lighting is enabled
    if (u_UniformData.LKeyIsPressed == 1)
    {
        // Compute vectors in eye coordinates
        vec4 eyeCoordinates = u_UniformData.viewMatrix * u_UniformData.modelMatrix * vec4(vPosition, 1.0);
        mat3 normalMatrix = mat3(transpose(inverse(u_UniformData.viewMatrix * u_UniformData.modelMatrix)));

        out_transformedNormal = normalize(normalMatrix * vNormal);
        out_lightDirection = vec3(u_UniformData.lightPosition) - eyeCoordinates.xyz;
        out_viewerVector = -eyeCoordinates.xyz;
    }
    else
    {
        // Default zero vectors when lighting is off
        out_transformedNormal = vec3(0.0);
        out_lightDirection = vec3(0.0);
        out_viewerVector = vec3(0.0);
    }
}


