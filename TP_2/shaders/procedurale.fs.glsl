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
    fFragColor = vColor * (smoothstep(0.4, 0.5, max(abs(fract(8.0 * vPosition.x - 0.5 * mod(floor(8.0 * vPosition.y), 2.0)) - 0.5), abs(fract(8.0 * vPosition.y) - 0.5))));
}
