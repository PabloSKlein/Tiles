#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class Texture
{
public:

	Texture(float uTexture, float vTexture);
	~Texture();

	float uTexture = 1.0f / 14.0f, 
		  vTexture = 1.0f / 10.0f;

};