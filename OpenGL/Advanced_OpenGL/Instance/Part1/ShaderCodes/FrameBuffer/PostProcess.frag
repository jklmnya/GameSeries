#version 330 core

in vec2 UV;
out vec4 FragColor;

uniform sampler2D texture1;

const float offset = 1.f / 300.f;
const vec2 offsets[9] = vec2[](
    vec2(-offset,  offset),
    vec2( 0.0f,    offset),
    vec2( offset,  offset),
    vec2(-offset,  0.0f),  
    vec2( 0.0f,    0.0f),  
    vec2( offset,  0.0f),  
    vec2(-offset, -offset),
    vec2( 0.0f,   -offset),
    vec2( offset, -offset) 
);

const float kernels[9] = float[](
    1.0f, 1.0f, 1.0f,
    1.0f, -8.0f, 1.0f,
    1.0f, 1.0f, 1.0f
);

void main() {
	vec3 Result = vec3(0.0f);
    for (int i = 0;i<9;++i)
        Result += texture(texture1, UV.st + offsets[i]).rgb * kernels[i];
    // Result /= 16.f;
	FragColor = vec4(Result, 1.0f);
}