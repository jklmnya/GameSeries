#pragma once

/*
	1. GLSL's structure

	#version version_number
	in type in_name;

	out type out_name;

	uniform type uniform_name;

	int main() {
		// process in val
		out_name = ...
	}

	2. data: int float double uint bool
	container: vector, matrix

	vector:
	vecn  -> vector has n float
	bvecn -> vector has n bool
	ivecn
	uvecn
	dvecn

	3. how to send data from vertex shader to fragment shader
	in vertex shader has a out val and in fragment shader has a in val that has same type and name

	eg. 
	vertex shader:

	#version 330 core
	layout (location = 0) in vec3 aPos;	// 位置变量的属性位置为0

	out vec4 vertexColor;

	void main() {
		gl_Position = vec4(aPos, 1.0);
		vertexColor = vec4(0.5, 0.0, 0.0, 1.0);
	}
		
	fragment shader:

	#version 330 core
	out vec4 FragColor;

	in vec4 vertexCOlor;

	void main() {
		FragColor = vertexColor;
	}

	4. uniform: a data in shader that can receive data from application.

	eg.
	#version 330 core
	out vec4 FragColor;

	uniform vec4 ourColor;

	void main() {
		FragColor = ourColor;
	}
*/

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "Log/Logger.h"

enum E_Shader_Check_Type {
	VERTEX_SHADER_COMPILE,
	FRAGMENT_SHADER_COMPILE,
	LINK
};

enum E_Shader_Type {
	VERTEX_SHADER,
	FRAGMENT_SHADER
};

class Shader {

	unsigned int shaderProgramID;

public:

	// Constructor
	Shader(const char* vertexShaderSourcePath, const char* fragmentShaderSourcePath) {
		std::string vertexShaderCode, fragmentShaderCode;
		std::ifstream vertexShaderFile, fragmentShaderFile;
		// ensure ifstream objects can throw exceptions:
		vertexShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fragmentShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try {
			vertexShaderFile.open(vertexShaderSourcePath);
			fragmentShaderFile.open(fragmentShaderSourcePath);
			std::stringstream vertexShaderss, fragmentShaderss;
			vertexShaderss << vertexShaderFile.rdbuf();
			vertexShaderCode = vertexShaderss.str();
			fragmentShaderss << fragmentShaderFile.rdbuf();
			fragmentShaderCode = fragmentShaderss.str();
			// close file
			vertexShaderFile.close();
			fragmentShaderFile.close();
		}
		catch (std::ifstream::failure& e) {
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what() << std::endl;
		}
		// Create vertex shader and fragment shader
		unsigned int vertexShader, fragmentShader;
		createShader(vertexShader, vertexShaderCode.c_str(), E_Shader_Type::VERTEX_SHADER);
		createShader(fragmentShader, fragmentShaderCode.c_str(), E_Shader_Type::FRAGMENT_SHADER);
		// Create shader program and link
		shaderProgramID = linkShader(vertexShader, fragmentShader);
	}

private:

	static void checkShaderErrors(unsigned int id, E_Shader_Check_Type type);
	static void createShader(unsigned int& id, const char* code, E_Shader_Type type);
	static unsigned int linkShader(unsigned int vertexShaderID, unsigned int fragmentShaderID);

public:

	inline void useShaderProgram() const {
		glUseProgram(shaderProgramID);
	}

	inline void deleteShaderProgram() const {
		glDeleteProgram(shaderProgramID);
	}

	inline void useUniformInt(const char* valName, int val) const {
		glUniform1i(glGetUniformLocation(shaderProgramID, valName), val);
	}

	inline void useUniformFloat(const char* valName, float val) const {
		glUniform1f(glGetUniformLocation(shaderProgramID, valName), val);
	}
};

void Shader::checkShaderErrors(unsigned int id, E_Shader_Check_Type type) {
	static int success;
	static char log[1024];
	if (type == E_Shader_Check_Type::LINK) {
		glGetProgramiv(id, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(id, 512, NULL, log);
			Logger::getLogger()->PrintError(log);
		}
	}
	else {
		glGetShaderiv(id, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(id, 512, NULL, log);
			if (type == E_Shader_Check_Type::VERTEX_SHADER_COMPILE)
				Logger::getLogger()->PrintError(log);
			else
				Logger::getLogger()->PrintError(log);
		}
	}
}

void Shader::createShader(unsigned int& id, const char* code, E_Shader_Type type) {
	if (type == E_Shader_Type::VERTEX_SHADER)
		id = glCreateShader(GL_VERTEX_SHADER);
	else
		id = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(id, 1, &code, NULL);
	glCompileShader(id);
	if (type == E_Shader_Type::VERTEX_SHADER)
		checkShaderErrors(id, E_Shader_Check_Type::VERTEX_SHADER_COMPILE);
	else
		checkShaderErrors(id, E_Shader_Check_Type::FRAGMENT_SHADER_COMPILE);
}

unsigned int Shader::linkShader(unsigned int vertexShaderID, unsigned int fragmentShaderID) {
	unsigned int id = glCreateProgram();
	glAttachShader(id, vertexShaderID);
	glAttachShader(id, fragmentShaderID);
	glLinkProgram(id);
	checkShaderErrors(id, E_Shader_Check_Type::LINK);
	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);
	return id;
}