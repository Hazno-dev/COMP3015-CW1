#version 460

layout (binding = 0) uniform samplerCube SkyboxTex;

layout (location = 0) out vec4 FragColor;

in vec3 Vec;

void main() {

    vec3 texColour = texture(SkyboxTex, normalize(Vec)).rgb;
    texColour = pow(texColour, vec3(1.0/2.2));
    FragColor = vec4(texColour, 1.0);

}