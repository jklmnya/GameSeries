#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUv;

uniform mat4 MatModel;
uniform mat4 MatView;
uniform mat4 MatProjection;

out vec3 Normal;
out vec3 FragPos;
out vec2 uv;

void main()
{
    gl_Position = MatProjection * MatView * MatModel * vec4(aPos, 1.0f);
    Normal = mat3(transpose(inverse(MatModel))) * aNormal;  
    FragPos = vec3(MatModel * vec4(aPos, 1.0f));
    uv = aUv;
}