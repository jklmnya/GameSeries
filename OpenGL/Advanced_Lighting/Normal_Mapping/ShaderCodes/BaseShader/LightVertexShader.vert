#version 330 core
layout(location = 0) in vec3 aPos;

uniform mat4 MatModel;
uniform mat4 MatView;
uniform mat4 MatProjection;

void main() {
	gl_Position = MatProjection * MatView * MatModel * vec4(aPos, 1.0f);
}
