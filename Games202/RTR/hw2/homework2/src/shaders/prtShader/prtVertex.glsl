#define SH_NUM 9

attribute vec3 aVertexPosition;
attribute mat3 aPrecomputeLT;

uniform vec3 uSH[SH_NUM];
uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;

varying highp vec3 vColor;

void main() {
    gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * vec4(aVertexPosition, 1.0);

    vec3 Color;
    Color += uSH[0] * aPrecomputeLT[0][0];
    Color += uSH[1] * aPrecomputeLT[0][1];
    Color += uSH[2] * aPrecomputeLT[0][2];
    Color += uSH[3] * aPrecomputeLT[1][0];
    Color += uSH[4] * aPrecomputeLT[1][1];
    Color += uSH[5] * aPrecomputeLT[1][2];
    Color += uSH[6] * aPrecomputeLT[2][0];
    Color += uSH[7] * aPrecomputeLT[2][1];
    Color += uSH[8] * aPrecomputeLT[2][2];
    vColor = Color;
}
