#version 300 es
precision mediump float;

layout(location = 0) in vec2 aVertexPosition;
//layout(location = 1) in vec3 aVertexColor;
layout(location = 2) in vec2 aVertexTexture;

//out vec3 vColor;
out vec2 vPosition;
out vec2 vTexture;

uniform mat3 uModelMatrix;
uniform vec3 uColor;

void main() {
    //vColor = uColor;
    vTexture = aVertexTexture;
    vec2 transformed = (uModelMatrix * vec3(aVertexPosition, 1)).xy;

    vPosition = aVertexPosition;
    gl_Position = vec4(transformed, 0, 1);
}

