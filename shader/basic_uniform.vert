#version 460

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTexCoord;
layout (location = 3) in vec4 VertexTangent;

out vec4 Position;
//out vec3 Normal;
out vec2 TexCoord;
out vec3 LightsDir[3];
out vec3 SpotlightDir;
out vec3 ViewDir;

uniform struct LightInfo {
    vec4 Position;
    vec3 La; //Amb
    vec3 Ld; //Diff
    vec3 Ls; //Spec
} Lights[3];

uniform struct SpotLightInfo {
    vec3 Position;
    vec3 La;
    vec3 Ld;
    vec3 Ls;
    vec3 Direction;
    float Exponent;
    float Cutoff;
} Spotlight;

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 MVP;
uniform mat4 ProjectionMatrix;

void getCamSpaceValues ( out vec3 norm, out vec4 position );

void main()
{
    // Convert vertex normal and position to camera space
    Position = (ModelViewMatrix * vec4(VertexPosition,1.0));
    //getCamSpaceValues(Normal, Position);

    vec3 norm = normalize(NormalMatrix * VertexNormal);
    vec3 tang = normalize(NormalMatrix * vec3(VertexTangent));

    vec3 binormal = normalize(cross(norm,tang)) * VertexTangent.w;

    // Matrix for transformation to tangent space
    mat3 toObjectLocal = mat3(tang.x, binormal.x, norm.x, tang.y, binormal.y, norm.y, tang.z, binormal.z, norm.z );

    vec3 pos = vec3( ModelViewMatrix * vec4(VertexPosition,1.0) );

    for (int i = 0; i < 3; i++) LightsDir[i] = toObjectLocal * (Lights[i].Position.xyz - pos);
    SpotlightDir = toObjectLocal * (Spotlight.Position - pos);

    ViewDir = toObjectLocal * normalize(-pos);

    TexCoord = VertexTexCoord;
    gl_Position = MVP * vec4(VertexPosition, 1.0);
}

void getCamSpaceValues ( out vec3 norm, out vec4 position )
{
    norm = normalize( NormalMatrix * VertexNormal);
    position = (ModelViewMatrix * vec4(VertexPosition,1.0));

}


// ------------------ NOTE ---------------
//
//Lighting calculations are conducted in the fragment shader instead of the vertex shader as to avoid interpolation across vertices (gouraud shading)
//and instead perform lighting calculations on a per-pixel basis for a more accurate result.
//However the old implementation of vertex-based phong lighting can be viewed below as mentioned in the marking rubric:
//
//----------------------------------------
//
//vec3 phong(vec3 normCamS, vec4 vertexPosCamS){
//
//    vec3 ambient = Light.La * Material.Ka;
//
//    //normalize vertex pos to light pos
//    vec3 lightDir = normalize(vec3(Light.Position - vertexPosCamS));
//    float lightDirDot = max(dot(lightDir, normCamS), 0.0);
//
//    // Implement the diffuse shading calculation
//    vec3 diffuse = Light.Ld * Material.Kd * lightDirDot;
//
//    vec3 spec = vec3(0.0);
//    if (lightDirDot > 0.0){
//        vec3 v = normalize(-vertexPosCamS.xyz);
//        vec3 r = reflect(-lightDir, normCamS);
//        spec = Light.Ls * Material.Ks * pow(max(dot(r,v), 0.0), Material.Shininess);
//    }
//
//    return ambient + diffuse + spec;
//}
//
//void getCamSpaceValues ( out vec3 norm, out vec4 position )
//{
//    norm = normalize( NormalMatrix * VertexNormal);
//    position = (ModelViewMatrix * vec4(VertexPosition,1.0));
//}