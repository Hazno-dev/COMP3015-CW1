#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H

#include "helper/scene.h"

#include "helper/glslprogram.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "helper/torus.h"
#include "helper/teapot.h"
#include "helper/plane.h"
#include "helper/objmesh.h"

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
    //Torus TorusMesh;
    //Teapot TeapotMesh;
    Plane plane; //plane surface
    std::unique_ptr<ObjMesh> mesh; //pig mesh

    glm::mat4 rotationMatrix;

    void setMatrices();
    void compile();

};

#endif // SCENEBASIC_UNIFORM_H
