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
    //Cube cube;
    std::unique_ptr<ObjMesh> mesh; //pig mesh

    glm::mat4 rotationMatrix;

    GLuint Planet1BCTex, Planet1NMTex,
        Planet2BCTex, Planet2NMTex,
        PlaneTex, PlaneAlpha,
        CrystalBCTex, CrystalNMTex,
        CrystalBlendBCTex, CrystalBlendAlpha,
        SkyboxTex;

    SkyBox skybox;

    void setMatrices();
    void setAlphaMatrices();
    void setSkyboxMatrices();
    void setLightUniforms();
    void compile();

};

#endif // SCENEBASIC_UNIFORM_H
