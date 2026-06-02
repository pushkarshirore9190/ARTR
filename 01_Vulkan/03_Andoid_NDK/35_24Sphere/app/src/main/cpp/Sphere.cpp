#include "Sphere.h"
#include <math.h>
#include <string.h>

#define VDG_PI 3.14159265358979323846

static float sphere_vertices[1146];
static float sphere_normals[1146];
static float sphere_textures[764];
static unsigned short sphere_elements[2280];

static unsigned int numVertices = 382;
static unsigned int numElements = 2280;

extern "C" void getSphereVertexData
(
	float spherePositionCoords[1146],
	float sphereNormalCoords[1146],
	float sphereTexCoords[764],
	unsigned short sphereElements[2280]
)
{
	int i;

	memset(sphere_vertices, 0, sizeof(sphere_vertices));
	memset(sphere_normals, 0, sizeof(sphere_normals));
	memset(sphere_textures, 0, sizeof(sphere_textures));
	memset(sphere_elements, 0, sizeof(sphere_elements));

	int vertexIndex = 0;
	int textureIndex = 0;
	int elementIndex = 0;

	int stacks = 18;
	int slices = 18;

	for(int stack = 0; stack <= stacks; stack++)
	{
		float phi = VDG_PI * (float)stack / (float)stacks;

		for(int slice = 0; slice <= slices; slice++)
		{
			float theta = 2.0f * VDG_PI * (float)slice / (float)slices;

			float x = sinf(phi) * cosf(theta);
			float y = cosf(phi);
			float z = sinf(phi) * sinf(theta);

			sphere_vertices[vertexIndex] = x;
			sphere_normals[vertexIndex++] = x;

			sphere_vertices[vertexIndex] = y;
			sphere_normals[vertexIndex++] = y;

			sphere_vertices[vertexIndex] = z;
			sphere_normals[vertexIndex++] = z;

			sphere_textures[textureIndex++] = (float)slice / (float)slices;
			sphere_textures[textureIndex++] = (float)stack / (float)stacks;
		}
	}

	for(int stack = 0; stack < stacks; stack++)
	{
		for(int slice = 0; slice < slices; slice++)
		{
			unsigned short first = (stack * (slices + 1)) + slice;
			unsigned short second = first + slices + 1;

			sphere_elements[elementIndex++] = first;
			sphere_elements[elementIndex++] = second;
			sphere_elements[elementIndex++] = first + 1;

			sphere_elements[elementIndex++] = second;
			sphere_elements[elementIndex++] = second + 1;
			sphere_elements[elementIndex++] = first + 1;
		}
	}

	for(i = 0; i < 1146; i++)
	{
		spherePositionCoords[i] = sphere_vertices[i];
		sphereNormalCoords[i] = sphere_normals[i];
	}

	for(i = 0; i < 764; i++)
	{
		sphereTexCoords[i] = sphere_textures[i];
	}

	for(i = 0; i < 2280; i++)
	{
		sphereElements[i] = sphere_elements[i];
	}
}

extern "C" unsigned int getNumberOfSphereVertices(void)
{
	return(numVertices);
}

extern "C" unsigned int getNumberOfSphereElements(void)
{
	return(numElements);
}



