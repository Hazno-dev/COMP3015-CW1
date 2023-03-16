#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H

#include "helper/scene.h"

#include <glad/glad.h>
#include "helper/glslprogram.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "helper/torus.h"

class SceneBasic_Uniform : public Scene
{
public:
    SceneBasic_Uniform();

    void initScene();
    void update( float t );
    void render();
    void resize(int, int);

private:
    GLSLProgram prog;
    Torus TorusMesh;
    glm::mat4 rotationMatrix;

    void setMatrices();
    void compile();

};

#endif // SCENEBASIC_UNIFORM_H
