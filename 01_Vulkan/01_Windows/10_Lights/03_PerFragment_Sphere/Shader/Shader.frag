#version 450 core
#extension GL_ARB_separate_shader_objects : enable

layout(location=0) in vec3 out_transformedNormal;

layout(location=1) in vec3 out_lightDirection;

layout(location=2) in vec3 out_viwerVector;

layout(location = 0) out vec4 FragColor;

layout(binding=0) uniform UniformData
{
    //matrices related uniform
    mat4 modelMatrix;
    mat4 viewMatrix;
    mat4 projectionMatrix;

	//Light related Uniforms
	vec4 lightAmbient;
	vec4 lightDiffuse;
    vec4 lightSpecular;
    vec4 lightPosition;

    // matrical unforom
    vec4 materialAmbient;
    vec4 materialDiffuse;
    vec4 materialSpecular;
    vec4 materialShininess;


	//key Press related uniform
	uint LKeyIsPressed;

} u_UniformData;

void main(void)
{
	// code
	vec3 phong_ads_light;

	if(u_UniformData.LKeyIsPressed == 1)
	{
		vec3 normalisedTransformedNormal = normalize(out_transformedNormal);
		vec3 normalisedLightDirection = normalize(out_lightDirection);
		vec3 normalisedViewerVector = normalize(out_viwerVector);

		vec3 ambientLight = vec3(u_UniformData.lightAmbient) * vec3(u_UniformData.materialAmbient);
		vec3 diffuseLight = vec3(u_UniformData.lightDiffuse) * vec3(u_UniformData.materialDiffuse) * max(dot(normalisedLightDirection, normalisedTransformedNormal),0.0);
		vec3 reflectionVector = reflect(-normalisedLightDirection,normalisedTransformedNormal);
		vec3 specularLight = vec3(u_UniformData.lightSpecular) 
                   * vec3(u_UniformData.materialSpecular) 
                   * pow(max(dot(reflectionVector, normalisedViewerVector), 0.0), 
                         u_UniformData.materialShininess.x);

		phong_ads_light = ambientLight + diffuseLight + specularLight;
	}

	else
	{
		phong_ads_light = vec3(1.0,1.0,1.0);
	}

	FragColor = vec4(phong_ads_light,1.0);
}


