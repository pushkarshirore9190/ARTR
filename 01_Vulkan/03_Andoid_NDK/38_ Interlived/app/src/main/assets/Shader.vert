#version 450 core
#extension GL_ARB_separate_shader_objects : enable
layout(location=0) in vec4 vPosition;
layout(location=1) in vec4 vColor;
layout(location=2) in vec3 vNormal;
layout(location=3) in vec2 vTexcoord;

layout(binding=0) uniform UniformData
{
    //matrices related uniform
    mat4 modelMatrix;
    mat4 viewMatrix;
    mat4 projectionMatrix;

	//Light related Uniforms
	vec4 lightAmbient;
	vec4 lightDiffused;
    vec4 lightSpecular;
    vec4 lightPosition;

    // matrical unforom
    vec4 materialAmbient;
    vec4 materialDiffuse;
    vec4 materialSpecular;
    float materialShininess;

} u_UniformData;

layout (location=0) out vec4 out_color;

layout (location=1) out vec2 out_Texcoord;

layout(location=2) out vec3 out_transformedNormal;

layout(location=3) out vec3 out_lightDirection;

layout(location=4) out vec3 out_viwerVector;

void main(void)
{
    // Transform vertex position using MVP matrices
    gl_Position = u_UniformData.projectionMatrix * u_UniformData.viewMatrix * u_UniformData.modelMatrix * vPosition;

    out_color = vColor;
    out_Texcoord = vTexcoord;

    vec4 eyeCoordinates = u_UniformData.viewMatrix *u_UniformData.modelMatrix * vPosition;
    mat3 normalMatrix = mat3(transpose(inverse(u_UniformData.viewMatrix * u_UniformData.modelMatrix)));
    out_transformedNormal = normalMatrix * vNormal;
    out_lightDirection = vec3(u_UniformData.lightPosition) - eyeCoordinates.xyz;
    out_viwerVector = -eyeCoordinates.xyz;


    
}


