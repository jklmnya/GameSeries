#pragma once

#include <assimp/scene.h>

#include "stb_image.h"
#include "../Basic/Core.h"

enum E_Texture_Type {
	TEXTURE_AMBIENT = 1,
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


	std::string GetUpOneLevelPath(const std::string& Path) {
		if (Path.find_last_of('/') != Path.npos)
			return Path.substr(0, Path.find_last_of('/'));
		else if (Path.find_last_of('\\') != Path.npos)
			return Path.substr(0, Path.find_last_of('\\'));
		else
			return Path;
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

	unsigned int LoadCubeMap(std::string BasePath) {
		std::vector<std::string> CubeMapPaths{
			BasePath + "right.jpg",
			BasePath + "left.jpg",
			BasePath + "top.jpg",
			BasePath + "bottom.jpg",
			BasePath + "front.jpg",
			BasePath + "back.jpg"
		};
		unsigned int CubeMapId;
		glGenTextures(1, &CubeMapId);
		glBindTexture(GL_TEXTURE_CUBE_MAP, CubeMapId);
		int Width, Height, NrChannels;
		for (unsigned int i = 0; i < CubeMapPaths.size(); ++i) {
			unsigned char* Data = stbi_load(CubeMapPaths[i].c_str(), &Width, &Height, &NrChannels, 0);
			if (Data) {
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, Width, Height, 0,
					GL_RGB, GL_UNSIGNED_BYTE, Data);
			}
			else {
				Logger::GetLogger()->PrintError("Failed To Load Texture");
				stbi_image_free(Data);
			}
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		return CubeMapId;
	}

};

