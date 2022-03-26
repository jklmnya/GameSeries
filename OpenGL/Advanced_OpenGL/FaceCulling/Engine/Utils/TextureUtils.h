#pragma once

#include <assimp/scene.h>

#include "stb_image.h"
#include "../Basic/Core.h"

enum E_Texture_Type {
	TEXTURE_AMBIENT,
	TEXTURE_DIFFUSE,
	TEXTURE_SPECULAR,
	TEXTURE_NORMAL,
	TEXTURE_HEIGHT
};

namespace TextureUtil {

	unsigned int LoadTexture(unsigned char* data, int width, int height, int nrComponents) {
		unsigned int textureId;
		glGenTextures(1, &textureId);

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
			Logger::GetLogger()->PrintError("Failed To Load Texture");
			stbi_image_free(data);
		}
		return textureId;
	}

	void GetTextureRelativePath(std::string& Path) {
		if (Path.find_last_of('/') != Path.npos)
			Path = Path.substr(Path.find_last_of('/') + 1);
		else if (Path.find_last_of('\\') != Path.npos)
			Path = Path.substr(Path.find_last_of('\\') + 1);
	}

	unsigned int TextureFromFile(const char* path) {
		int width, height, nrComponents;
		unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
		return LoadTexture(data, width, height, nrComponents);
	}

	unsigned int TextureFromFbx(const aiTexture* _aiTexture) {
		int width, height, nrComponents;
		unsigned char* data = nullptr;
		if (_aiTexture->mHeight == 0)
			data = stbi_load_from_memory(reinterpret_cast<unsigned char*>(_aiTexture->pcData), _aiTexture->mWidth, &width, &height, &nrComponents, 0);
		else
			data = stbi_load_from_memory(reinterpret_cast<unsigned char*>(_aiTexture->pcData), _aiTexture->mWidth * _aiTexture->mHeight, &width, &height, &nrComponents, 0);
		return LoadTexture(data, width, height, nrComponents);
	}	
};

