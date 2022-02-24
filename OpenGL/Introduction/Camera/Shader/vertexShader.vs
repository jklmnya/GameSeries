#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 uv;

out vec2 textureCoord;

uniform mat4 MatModel;
uniform mat4 MatView;
uniform mat4 MatProjection;

void main()
{
    gl_Position = MatProjection * MatView * MatModel * vec4(aPos, 1.0f);
    textureCoord = uv;
}