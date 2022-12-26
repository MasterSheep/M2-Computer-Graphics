#version 300 es
precision mediump float;

// Entrée du shader
in vec3 vPosition_vs; // Position du sommet transformé dans l'espace View
in vec3 vNormal_vs;  // Normale du sommet transformé dans l'espace View
in vec2 vTexCoords; // Coordonnées de texture du sommet

// Sorties du shader
out vec3 fFragColor;

uniform sampler2D uTexture;
uniform sampler2D uTexture2;

uniform vec3 uKd;
uniform vec3 uKs;
uniform float uShininess;
uniform vec3 uLightPos_vs;
uniform vec3 uLightIntensity;

vec3 blinnPhong(){
    vec3 w_0 = normalize(-vPosition_vs);
    vec3 w_i = normalize(uLightPos_vs - vPosition_vs);
    vec3 halfVector = (w_0 + w_i) / 2.f;
    float d = distance(uLightPos_vs, vPosition_vs);
    return (uLightIntensity / (d * d)) * 
    ( uKd * ( dot(w_i, normalize(vNormal_vs) ) ) + 
      uKs * ( pow( dot(halfVector, normalize(vNormal_vs)), uShininess ) ) );
}


void main() {
    //fFragColor = normalize(vNormal_vs);
    //fFragColor = texture(uTexture, vTexCoords).xyz + texture(uTexture2, vTexCoords).xyz;

    fFragColor = blinnPhong() * (texture(uTexture, vTexCoords).xyz + texture(uTexture2, vTexCoords).xyz);
}
