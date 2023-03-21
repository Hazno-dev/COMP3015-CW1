#version 460

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTexCoord;
layout (location = 3) in vec2 VertexTangent;

out vec4 Position;
out vec3 Normal;
out vec2 TexCoord;

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 MVP;
uniform mat4 ProjectionMatrix;

void getCamSpaceValues ( out vec3 norm, out vec4 position );

void main()
{
    // Convert vertex normal and position to camera space
    getCamSpaceValues(Normal, Position);
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