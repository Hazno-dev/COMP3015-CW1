#version 460

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;

out vec3 LightIntensity;

uniform vec4 LightPosition;
uniform vec3 Kd;
uniform vec3 Ld;
uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 MVP;

void getCamSpaceValues ( out vec3 norm, out vec3 position )
{
    norm = normalize( NormalMatrix * VertexNormal);
    position = (ModelViewMatrix * vec4(VertexPosition,1.0)).xyz;
}

void main()
{
    // Convert vertex normal and position to camera space
    vec3 normCamS, vertexPosCamS;
    getCamSpaceValues(normCamS, vertexPosCamS);

    vec3 lightDir = normalize(vec3(LightPosition.xyz - vertexPosCamS));

    // Implement the diffuse shading calculation
    float diff = max(dot(normCamS, lightDir), 0.0);
    LightIntensity = Ld * Kd * diff;

    gl_Position = MVP * vec4(VertexPosition, 1.0);
}
