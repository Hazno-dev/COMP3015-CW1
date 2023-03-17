#version 460

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;

out vec3 LightIntensity;

uniform struct LightInfo {
    vec4 Position;
    vec3 La; //Amb
    vec3 Ld; //Diff
    vec3 Ls; //Spec
} Light;

uniform struct MaterialInfo{
    vec3 Ka; //Amb
    vec3 Kd; //Diff
    vec3 Ks; //Spec
    float Shininess; //Shininess
} Material;


uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 MVP;

void getCamSpaceValues ( out vec3 norm, out vec4 position );
vec3 phong(vec3 normCamS, vec4 vertexPosCamS);

void main()
{
    // Convert vertex normal and position to camera space
    vec3 normCamS;
    vec4 vertexPosCamS;
    getCamSpaceValues(normCamS, vertexPosCamS);

    LightIntensity = phong(normCamS, vertexPosCamS);

    gl_Position = MVP * vec4(VertexPosition, 1.0);
}

vec3 phong(vec3 normCamS, vec4 vertexPosCamS){

    vec3 ambient = Light.La * Material.Ka;

    //normalize vertex pos to light pos
    vec3 lightDir = normalize(vec3(Light.Position - vertexPosCamS));
    float lightDirDot = max(dot(lightDir, normCamS), 0.0);

    // Implement the diffuse shading calculation
    vec3 diffuse = Light.Ld * Material.Kd * lightDirDot;

    vec3 spec = vec3(0.0);
    if (lightDirDot > 0.0){
        vec3 v = normalize(-vertexPosCamS.xyz);
        vec3 r = reflect(-lightDir, normCamS);
        spec = Light.Ls * Material.Ks * pow(max(dot(r,v), 0.0), Material.Shininess);
    }

    return ambient + diffuse + spec;

}

void getCamSpaceValues ( out vec3 norm, out vec4 position )
{
    norm = normalize( NormalMatrix * VertexNormal);
    position = (ModelViewMatrix * vec4(VertexPosition,1.0));
}