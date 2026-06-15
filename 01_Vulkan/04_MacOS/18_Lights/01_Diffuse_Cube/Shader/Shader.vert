#version 450 core
#extension GL_ARB_separate_shader_objects : enable
layout(location=0) in vec4 vPosition;
layout(location=1) in vec3 vNormal;
layout(binding=0) uniform UniformData
{
    //matrices related uniform
    mat4 modelMatrix;
    mat4 viewMatrix;
    mat4 projectionMatrix;

	//Light related Uniforms
	vec4 lightDiffused;
	vec4 lightPosition;
	vec4 materialDiffused;

	//key Press related uniform
	uint LKeyIsPressed;

} u_UniformData;

layout(location=0) out vec4 out_vDiffusedLight;

void main(void)
{
    // Transform vertex position using MVP matrices
    gl_Position = u_UniformData.projectionMatrix * u_UniformData.viewMatrix * u_UniformData.modelMatrix * vPosition;
    if(u_UniformData.LKeyIsPressed == 1)
    {
        vec4 eyeCoordinates = u_UniformData.viewMatrix *u_UniformData.modelMatrix * vPosition;
        mat3 normalMatrix = mat3(transpose(inverse(u_UniformData.viewMatrix * u_UniformData.modelMatrix)));
        vec3 transformedNormals = normalize(normalMatrix*vNormal);
        vec3 lightSource = normalize(vec3(u_UniformData.lightPosition - eyeCoordinates));
        out_vDiffusedLight = vec4(vec3(u_UniformData.lightDiffused)*vec3(u_UniformData.materialDiffused*max(dot(lightSource,transformedNormals),0.0)), 1.0);
    }
    else
    {
        out_vDiffusedLight = vec4(1.0,1.0,1.0,1.0);
    }

}


