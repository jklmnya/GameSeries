#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../Basic/Core.h"

const static std::string texture_ambient	= "texture_ambient";
const static std::string texture_diffuse	= "texture_diffuse";
const static std::string texture_specular	= "texture_specular";
const static std::string texture_noraml		= "texture_noraml";
const static std::string texture_height		= "texture_height";

struct Vertex {
	Vector3 Position;
	Vector3 Normal;
	Vector2 UV;
	Vector3 Tangent;
	Vector3 BiTangent;
};

struct Texture {
	unsigned int TextureID;
	E_Texture_Type Type;
};

class Mesh {

private:
	std::vector<Vertex> Vertices;
	std::vector<unsigned int> Indices;
	std::vector<Texture> Textures;

	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;

public:
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures) :
		Vertices(vertices), Indices(indices), Textures(textures) {
		SetupMesh();
	}

	void Draw(Shader& shader) {
		unsigned int DiffuseNum		= 0;
		unsigned int AmbientNum		= 0;
		unsigned int SpecularNum	= 0;
		unsigned int NormalNum		= 0;
		unsigned int HeightNum		= 0;
		unsigned int idx = 0;
		for (auto& texture : Textures) {
			std::string Name;
			switch (texture.Type) {
				case E_Texture_Type::TEXTURE_AMBIENT:
					Name = texture_ambient;
					Name += (char)(++AmbientNum + '0');
					break;
				case E_Texture_Type::TEXTURE_DIFFUSE:
					Name = texture_diffuse;
					Name += (char)(++DiffuseNum + '0');
					break;
				case E_Texture_Type::TEXTURE_SPECULAR:
					Name = texture_specular;
					Name += (char)(++SpecularNum + '0');
					break;
				case E_Texture_Type::TEXTURE_NORMAL:
					Name = texture_noraml;
					Name += (char)(++NormalNum + '0');
					break;
				case E_Texture_Type::TEXTURE_HEIGHT:
					Name = texture_height;
					Name += (char)(++HeightNum + '0');
					break;
				default:
					break;
			}
			glActiveTexture(GL_TEXTURE0 + idx);
			shader.useUniformUInt(Name.c_str(), idx);
			glBindTexture(GL_TEXTURE_2D, Textures[idx++].TextureID);
		}
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		glActiveTexture(GL_TEXTURE0);
	}

	inline unsigned int GetMeshVAO() {
		return VAO;
	}

	inline std::vector<unsigned int>& GetMeshIndices() {
		return Indices;
	}

	inline std::vector<Texture>& GetMeshTextures() {
		return Textures;
	}

private:
	void SetupMesh() {
		glGenVertexArrays(1, &VAO);
	
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * Vertices.size(), &Vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * Indices.size(), &Indices[0], GL_STATIC_DRAW);

		// Vertex Position
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		glEnableVertexAttribArray(0);

		// Normal
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
		glEnableVertexAttribArray(1);

		// UV
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, UV));
		glEnableVertexAttribArray(2);

		// Tangent
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
		glEnableVertexAttribArray(3);

		// BiTangent
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, BiTangent));
		glEnableVertexAttribArray(4);

		glBindVertexArray(0);
	}
};

#include <unordered_set>

class Model {

private:
	std::vector<Mesh> Meshes;
	std::string RootPath;
	std::unordered_set<std::string> LoadedTexturePath;

public:
	Model(const char* path) {
		Logger::GetLogger()->PrintMessage(path);
		LoadModel(path);
	}

	void Draw(Shader& shader) {
		for (auto& Mesh_ : Meshes)
			Mesh_.Draw(shader);
	}

	inline std::vector<Mesh>& GetModelMeshes() {
		return Meshes;
	}

private:
	void LoadModel(const std::string& path) {
		Assimp::Importer importer;
		// Load Mesh Data
		const aiScene* Scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs
			| aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals);
		if (!Scene || Scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !Scene->mRootNode) {
			Logger::GetLogger()->PrintError(importer.GetErrorString());
			return;
		}
		RootPath = TextureUtil::GetUpOneLevelPath(path);
		ProcessNode(Scene->mRootNode, Scene);
	}

	void ProcessNode(aiNode* Node, const aiScene* Scene) {
		for (unsigned int i = 0; i < Node->mNumMeshes; ++i) {
			aiMesh* mesh = Scene->mMeshes[Node->mMeshes[i]];
			Meshes.push_back(std::move(ProcessMesh(mesh, Scene)));
		}
		// Process Sub Node
		for (unsigned int i = 0; i < Node->mNumChildren; ++i)
			ProcessNode(Node->mChildren[i], Scene);
	}

	Mesh ProcessMesh(aiMesh* Mesh_, const aiScene* Scene) {
		std::vector<Vertex> Vertices;
		std::vector<unsigned int> Indices;
		std::vector<Texture> Textures;
		// Process Mesh's Vertices
		for (unsigned int i = 0; i < Mesh_->mNumVertices; ++i) {
			Vertex vertex;
			vertex.Position = Vector3(Mesh_->mVertices[i].x, Mesh_->mVertices[i].y, Mesh_->mVertices[i].z);
			if (Mesh_->HasNormals()) {
				vertex.Normal = Vector3(Mesh_->mNormals[i].x, Mesh_->mNormals[i].y, Mesh_->mNormals[i].z);
			}
			if (Mesh_->mTextureCoords[0]) {
				vertex.UV = Vector2(Mesh_->mTextureCoords[0][i].x, Mesh_->mTextureCoords[0][i].y);
				vertex.Tangent = Vector3(Mesh_->mTangents[i].x, Mesh_->mTangents[i].y, Mesh_->mTangents[i].z);
				vertex.BiTangent = Vector3(Mesh_->mBitangents[i].x, Mesh_->mBitangents[i].y, Mesh_->mBitangents[i].z);
			}
			else {
				vertex.UV = Vector2(0.0f, 0.0f);
			}
			Vertices.push_back(std::move(vertex));
		}
		// Process Mesh's Faces(Indices)
		for (unsigned int i = 0; i < Mesh_->mNumFaces; i++) {
			aiFace Face = Mesh_->mFaces[i];
			// retrieve all indices of the face and store them in the indices vector
			for (unsigned int j = 0; j < Face.mNumIndices; j++)
				Indices.push_back(Face.mIndices[j]);
		}
		// Process Material
		if (Mesh_->mMaterialIndex >= 0) {
			aiMaterial* Material = Scene->mMaterials[Mesh_->mMaterialIndex];
			std::vector<Texture> DiffuseTextures = LoadSpecificTexture(Material, aiTextureType_DIFFUSE,
				E_Texture_Type::TEXTURE_DIFFUSE, Scene);

			Textures.insert(Textures.end(), DiffuseTextures.begin(), DiffuseTextures.end());
			std::vector<Texture> SpecularTextures = LoadSpecificTexture(Material, aiTextureType_SPECULAR,
				E_Texture_Type::TEXTURE_SPECULAR, Scene);
			Textures.insert(Textures.end(), SpecularTextures.begin(), SpecularTextures.end());

			std::vector<Texture> NormalTextures = LoadSpecificTexture(Material, aiTextureType_HEIGHT,
				E_Texture_Type::TEXTURE_NORMAL, Scene);
			Textures.insert(Textures.end(), NormalTextures.begin(), NormalTextures.end());

			std::vector<Texture> HeightTextures = LoadSpecificTexture(Material, aiTextureType_AMBIENT,
				E_Texture_Type::TEXTURE_HEIGHT, Scene);
			Textures.insert(Textures.end(), HeightTextures.begin(), HeightTextures.end());
		}
		return Mesh(std::move(Vertices), std::move(Indices), std::move(Textures));
	}

	std::vector<Texture> LoadSpecificTexture(aiMaterial* Material, aiTextureType aiTextureType_, 
		E_Texture_Type TextureType, const aiScene* Scene) {
		std::vector<Texture> Result;
		Result.reserve(Material->GetTextureCount(aiTextureType_));
		for (unsigned int i = 0; i < Material->GetTextureCount(aiTextureType_); ++i) {
			aiString Path;
			Material->GetTexture(aiTextureType_, i, &Path);
			if (LoadedTexturePath.count(Path.C_Str()))
				continue;
			Texture texture;
			texture.Type = TextureType;
			std::string TexturePath = Path.C_Str();
			TextureUtil::GetTextureRelativePath(TexturePath);
			std::string FileName = RootPath + '\\' + TexturePath;

			Logger::GetLogger()->PrintMessage(FileName.c_str());

			texture.TextureID = TextureUtil::TextureFromFile(FileName.c_str());
			LoadedTexturePath.insert(TexturePath.c_str());
			Result.push_back(std::move(texture));
		}
		return Result;
	}

};