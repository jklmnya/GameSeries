#version 330 core
#define SH_NUM 9

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aLightTrans1;
layout (location = 2) in vec3 aLightTrans2;
layout (location = 3) in vec3 aLightTrans3;

uniform mat4 ModelMat;
uniform mat4 ViewMat;
uniform mat4 ProjectionMat;

uniform vec3 LightSH[SH_NUM];

out highp vec3 iFragColor;

void main() {
	gl_Position = ProjectionMat * ViewMat * ModelMat * vec4(aPos, 1.0f);
	vec3 Color;
	Color += LightSH[0] * aLightTrans1[0];
    Color += LightSH[1] * aLightTrans1[1];
    Color += LightSH[2] * aLightTrans1[2];
    Color += LightSH[3] * aLightTrans2[0];
    Color += LightSH[4] * aLightTrans2[1];
    Color += LightSH[5] * aLightTrans2[2];
    Color += LightSH[6] * aLightTrans3[0];
    Color += LightSH[7] * aLightTrans3[1];
    Color += LightSH[8] * aLightTrans3[2];
	iFragColor = Color;
}