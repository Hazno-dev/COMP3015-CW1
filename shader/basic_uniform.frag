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

uniform struct SpotLightInfo {
    vec3 Position;
    vec3 La;
    vec3 Ld;
    vec3 Ls;
    vec3 Direction;
    float Exponent;
    float Cutoff;
} Spotlight;

uniform struct MaterialInfo{
    vec3 Ka; //Amb
    vec3 Kd; //Diff
    vec3 Ks; //Spec
    float Shininess; //Shininess
} Material;

//
//Consts
//

const bool ToonShading = true;
const bool SpecToonShading = false;
const int level = 5;
const float scaleFactor = 1.0/level;

//
//Functions
//

vec3 Phong(int light, vec3 NormalCam, vec4 PositionCam);
vec3 BlinnPhong(int light, vec3 NormalCam, vec4 PositionCam);
vec3 SpotBlinnPhong(vec3 NormalCam, vec4 PositionCam);

//
//Main
//

void main() {

    vec3 Colour = vec3(0.0);
    for (int i = 0; i <3; i++) Colour += BlinnPhong(i, Normal, Position);
    Colour += SpotBlinnPhong(Normal, Position);

    FragColor = vec4(Colour, 1.0);
}

// PHONG - LESS PERFORMANT
//
// Phong lighting in fragment shader is conducted on a per-pixel basis > per fragment
// Phong shading uses the reflection vector to calculate specular highlights, which can be more computationally expensive
// Only supports point lights at the moment
//
vec3 Phong(int light, vec3 NormalCam, vec4 PositionCam){

    // Calculate ambient lighting component
    vec3 ambient = Lights[light].La * Material.Ka;

    // Calculate and normalize light direction vector
    vec3 lightDir = normalize(vec3(Lights[light].Position - (PositionCam * Lights[light].Position.w)));
    float lightDirDot = max(dot(lightDir, NormalCam), 0.0);

    // Implement the diffuse shading calculation
    vec3 diffuse = vec3(0.0);
    if (ToonShading) diffuse = Lights[light].Ld * Material.Kd * (floor(lightDirDot*level)*scaleFactor);
    else diffuse = Lights[light].Ld * Material.Kd * lightDirDot;

    // Initialize specular component
    vec3 spec = vec3(0.0);
    if (lightDirDot > 0.0){
         vec3 v = normalize(-PositionCam.xyz);
        vec3 r = reflect(-lightDir, NormalCam); // Calculate the reflection vector
        float specIntensity = pow(max(dot(r,v), 0.0), Material.Shininess); // Calculate the specular intensity

        // Apply ToonShading to the specular component
        if (SpecToonShading) specIntensity = floor(specIntensity * level) * scaleFactor;

        spec = Lights[light].Ls * Material.Ks * specIntensity; // Calculate the specular component using the reflection vector
    }

    // Return the final shading color (ambient + diffuse + specular)
    return ambient + diffuse + spec;

}

// Blinn-Phong - MORE PERFORMANT
//
// Blinn-Phong shading improves performance by replacing the reflection vector calculation with the halfway vector
// This results in slightly different visual appearance but with lower computational cost
// For a directional light simply set the w component of a given light to 0.0 
//
vec3 BlinnPhong(int light, vec3 NormalCam, vec4 PositionCam){

    // Calculate ambient lighting component
    vec3 ambient = Lights[light].La * Material.Ka;

    // Calculate and normalize light direction vector
    vec3 lightDir = normalize(vec3(Lights[light].Position - (PositionCam * Lights[light].Position.w)));
    float lightDirDot = max(dot(lightDir, NormalCam), 0.0);

    // Implement the diffuse shading calculation based on ToonShading being true/false
    vec3 diffuse = vec3(0.0);

    if (ToonShading) diffuse = Lights[light].Ld * Material.Kd * (floor(lightDirDot*level)*scaleFactor);
    else diffuse = Lights[light].Ld * Material.Kd * lightDirDot;

    // Initialize specular component
    vec3 spec = vec3(0.0);
    if (lightDirDot > 0.0){
        vec3 v = normalize(-PositionCam.xyz);
        vec3 h = normalize(lightDir + v); // Calculate the halfway vector
        float specIntensity = pow(max(dot(h, NormalCam), 0.0), Material.Shininess); // Calculate the specular intensity using halfway vector

        // Apply ToonShading to the specular component
        if (SpecToonShading) specIntensity = floor(specIntensity * level) * scaleFactor;

        spec = Lights[light].Ls * Material.Ks * specIntensity; // Calculate the specular component using the specular intensity
    }

    // Return the final shading color (ambient + diffuse + specular)
    return ambient + diffuse + spec;
}

// Adapted Blinn-Phong shading for Spotlight
//
vec3 SpotBlinnPhong(vec3 NormalCam, vec4 PositionCam){

    // Calculate ambient lighting component and initialize diffuse and specular components
    vec3 ambient = Spotlight.La * Material.Ka;
    vec3 diffuse = vec3(0);
    vec3 spec = vec3(0.0);

    // Calculate and normalize light direction vector
    vec3 lightDir = normalize(vec3(Spotlight.Position - PositionCam.xyz));
    float cosAng = dot(-lightDir, normalize(Spotlight.Direction));
    float angle = acos(cosAng);
    float spotScale = 0.0;

    // Check if the point is within the spotlight's cutoff angle
    if (angle < Spotlight.Cutoff){
        // Calculate the spotlight attenuation using the exponent term
        spotScale = pow(cosAng, Spotlight.Exponent);

        // Calculate the diffuse shading component + implement toon shading if enabled
        float lightDirDot = max(dot(lightDir, NormalCam), 0.0);

        if (ToonShading) diffuse = Spotlight.Ld * Material.Kd * (floor((lightDirDot * spotScale) * level) * scaleFactor);
        else diffuse = Spotlight.Ld * Material.Kd * lightDirDot * spotScale;


        // Calculate the specular shading component if the point is lit
        if (lightDirDot > 0.0){
            vec3 v = normalize(-PositionCam.xyz);
            vec3 h = normalize(lightDir + v); // Calculate the halfway vector
            float specIntensity = pow(max(dot(h, NormalCam), 0.0), Material.Shininess); // Calculate the specular intensity

            // Apply ToonShading to the specular component
            if (SpecToonShading) specIntensity = floor(specIntensity * level) * scaleFactor;

            spec = Spotlight.Ls * Material.Ks * specIntensity * spotScale; // Calculate the specular component using the halfway vector and spotlight attenuation
        }
    }

    // Return the final shading color (ambient + diffuse + specular)
    return ambient + diffuse + spec;
}
