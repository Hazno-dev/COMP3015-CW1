#version 460

layout (location = 0) out vec4 FragColor;

in vec4 Position;
in vec3 Normal;

//
//Uniforms
//

uniform struct LightInfo {
    vec4 Position;
    vec3 La; //Amb
    vec3 Ld; //Diff
    vec3 Ls; //Spec
} Lights[3];

uniform struct MaterialInfo{
    vec3 Ka; //Amb
    vec3 Kd; //Diff
    vec3 Ks; //Spec
    float Shininess; //Shininess
} Material;

//
//Functions
//

vec3 phong(int light, vec3 NormalCam, vec4 PositionCam);

//
//Main
//

void main() {

    vec3 Colour = vec3(0.0);
    for (int i = 0; i <3; i++) Colour += phong(i, Normal, Position);

    FragColor = vec4(Colour, 1.0);
}

vec3 phong(int light, vec3 NormalCam, vec4 PositionCam){

    vec3 ambient = Lights[light].La * Material.Ka;

    //normalize vertex pos to light pos - Directional just mult vertexcampos by W 
    vec3 lightDir = normalize(vec3(Lights[light].Position - (PositionCam * Lights[light].Position.w)));
    float lightDirDot = max(dot(lightDir, NormalCam), 0.0);

    // Implement the diffuse shading calculation
    vec3 diffuse = Lights[light].Ld * Material.Kd * lightDirDot;

    vec3 spec = vec3(0.0);
    if (lightDirDot > 0.0){
        vec3 v = normalize(-PositionCam.xyz);
        vec3 h = normalize(lightDir + v); // Calculate the halfway vector
        spec = Lights[light].Ls * Material.Ks * pow(max(dot(NormalCam, h), 0.0), Material.Shininess);
    }

    return ambient + diffuse + spec;

}