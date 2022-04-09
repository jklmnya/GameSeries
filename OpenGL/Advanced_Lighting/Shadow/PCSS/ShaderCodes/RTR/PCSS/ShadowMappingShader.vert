#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 LightVP;
uniform mat4 ModelMat;

void main() {
	gl_Position = LightVP * ModelMat * vec4(aPos, 1.0f);
}