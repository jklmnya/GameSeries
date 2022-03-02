#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 MatModel;
uniform mat4 MatView;
uniform mat4 MatProjection;

out vec3 Normal;
out vec3 FragPos;

void main()
{
    gl_Position = MatProjection * MatView * MatModel * vec4(aPos, 1.0f);
    Normal = mat3(transpose(inverse(MatModel))) * aNormal;  
    FragPos = vec3(MatModel * vec4(aPos, 1.0f));
}