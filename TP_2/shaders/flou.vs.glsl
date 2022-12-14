#version 300 es
precision mediump float;

layout(location = 0) in vec2 aVertexPosition;
layout(location = 1) in vec3 aVertexColor;

out vec3 vColor;
out vec2 vPosition;

mat3 translate(float tx, float ty) {
    return mat3(vec3(1, 0, 0), vec3(0, 1, 0), vec3(tx, ty, 1));
}

mat3 scale(float sx, float sy) {
    return mat3(vec3(sx, 0, 0), vec3(0, sy, 0), vec3(0, 0, 1));
}

mat3 rotate(float a) {
    return mat3(vec3(cos(a), sin(a), 0), vec3(-sin(a), cos(a), 0), vec3(0, 0, 1));
}

void main() {

    vec2 transformed = (vec3(aVertexPosition, 1) * scale(5., 5.)).xy;

    vColor = aVertexColor;
    vPosition = aVertexPosition;
    gl_Position = vec4(transformed, 0, 1);
}
