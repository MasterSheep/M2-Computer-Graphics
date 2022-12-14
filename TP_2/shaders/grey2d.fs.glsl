#version 300 es
precision mediump float;

in vec3 vColor;

out vec3 fFragColor;

float M;

void main() {
    M = (vColor.r + vColor.g + vColor.b) / 3.0;
    fFragColor = vec3(M, M, M);
}
