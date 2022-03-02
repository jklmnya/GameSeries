#pragma once

#include "stb_image.h"
#include "../Log/Logger.h"

class Texture {

private:
	unsigned int textureId;

public:
	static unsigned int loadTexture(const char* path);
};

unsigned int Texture::loadTexture(const char* path) {
	unsigned int textureId;
	glGenTextures(1, &textureId);
	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data) {
		GLenum textureFormat;
		switch (nrComponents)
		{
		case 1:
			textureFormat = GL_RED;
			break;
		case 3:
			textureFormat = GL_RGB;
			break;
		case 4:
			textureFormat = GL_RGBA;
			break;
		default:
			break;
		}

		glBindTexture(GL_TEXTURE_2D, textureId);
		glTexImage2D(GL_TEXTURE_2D, 0, textureFormat, width, height, 0, textureFormat, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else {
		Logger::getLogger()->PrintError("Failed to load Texture");
		stbi_image_free(data);
	}
	return textureId;
}