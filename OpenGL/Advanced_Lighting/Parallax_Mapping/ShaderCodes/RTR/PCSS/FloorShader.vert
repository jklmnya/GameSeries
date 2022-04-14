#version 330 core

layout(location = 0) in vec3 aPos;

uniform mat4 ModelMat;
uniform mat4 ViewMat;
uniform mat4 ProjectionMat;

uniform mat4 LightVP;

out vec4 PositionFromLight;

void main() {
	gl_Position = ProjectionMat * ViewMat * ModelMat * vec4(aPos, 1.0f);
	PositionFromLight = LightVP * ModelMat * vec4(aPos, 1.0f);
}