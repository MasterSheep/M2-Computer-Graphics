#version 300 es
precision mediump float;

in vec3 vColor;
in vec2 vPosition;

out vec3 fFragColor;

float dst;

float attenuation(float alpha, float beta) {
    dst = distance(vPosition, vec2(0., 0.));
    return alpha * exp(-beta * dst * dst);
}

void main() {
    fFragColor = vColor * attenuation(1., 100.);
}
