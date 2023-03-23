#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H

#include "helper/scene.h"

#include "helper/glslprogram.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "helper/torus.h"
#include "helper/teapot.h"
#include "helper/plane.h"
#include "helper/cube.h"
#include "helper/objmesh.h"
#include "helper/skybox.h"

class SceneBasic_Uniform : public Scene
{
public:
    SceneBasic_Uniform();

    void initScene();
    void update( float t );
    void render();
    void resize(int, int);
    void setupCamera();
    void updateCamera(GLFWwindow* window);

private:
    GLSLProgram prog;
    GLSLProgram Alphaprog;
    GLSLProgram Skyboxprog;
    //Torus TorusMesh;
    //Teapot TeapotMesh;

    Plane plane; //plane surface
    std::unique_ptr<ObjMesh> CrystalMesh;
    std::unique_ptr<ObjMesh> Planet1Mesh;
    std::unique_ptr<ObjMesh> Planet2Mesh;
    std::unique_ptr<ObjMesh> MoonMesh;
    std::unique_ptr<ObjMesh> RingMesh;

    glm::vec3 Planet1Location;
    glm::vec3 Planet2Location;
    glm::vec3 moonLocation;

    float Planet1RotationSpeed = 20.f;
    float Planet2RotationSpeed = 10.f;
    float moonRotationSpeed = 25.0f;
    float crystalLevSpeed = 2.0f;

    float Planet1Distance = 12.0f;
    float Planet2Distance = 26.5f;
    float moonDistance = 3.0f;
    float crystalLevAmplitude = 0.2f;

    float Planet1Angle;
    float Planet2Angle;
    float moonAngle;
    float crystalOffset;

    glm::vec4 Light1Pos;
    glm::vec4 Light2Pos;
    glm::vec4 Light3Pos;
    glm::vec3 SpotLightPos;
    glm::vec3 SpotLightDir;

    GLuint Planet1BCTex, Planet1NMTex,
        Planet2BCTex, Planet2NMTex,
        MoonBCTex, MoonNMTex,
        PlaneTex, PlaneAlpha,
        CrystalBCTex, CrystalNMTex,
        CrystalBlendBCTex, CrystalBlendAlpha,
        SkyboxTex;

    SkyBox skybox;

    void setMatrices();
    void setAlphaMatrices();
    void setSkyboxMatrices();
    void setLightUniforms();
    void setTextures(GLuint Tex1, GLuint Tex2);
    void compile();

};

#endif // SCENEBASIC_UNIFORM_H
