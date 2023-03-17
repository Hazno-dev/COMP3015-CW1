#include "scenebasic_uniform.h"
#include <sstream>
#include <cstdio>
#include <cstdlib>

#include <string>
using std::string;

#include <iostream>
using std::cerr;
using std::endl;

#include <glm/gtc/matrix_transform.hpp>
using glm::vec3;
using glm::mat4;

#include "helper/glutils.h"


using glm::vec3;

//SceneBasic_Uniform::SceneBasic_Uniform() : TorusMesh(0.7f, 0.3f, 50, 50) {}
SceneBasic_Uniform::SceneBasic_Uniform() : TeapotMesh(50, glm::translate(mat4(1.f), vec3(0.0f,0.0f,0.0f))) {}

void SceneBasic_Uniform::initScene()
{
    compile();

    std::cout << std::endl;

    prog.printActiveUniforms();

    glEnable(GL_DEPTH_TEST);
    model = mat4(1.f);
    model = glm::rotate(model, glm::radians(-35.0f), vec3(1.0f, 0.0f, 0.0f));

    view = glm::lookAt(vec3(0.f, 0.f, 6.f), vec3(0.f, 0.f, 0.f), vec3(0.f, 1.f, 0.f));
    projection = mat4(1.f);
    float x, z;
    for (int i = 0; i < 3; i++) {
        std::stringstream name;
        name << "Lights[" << i << "].Position";
        x = 2.f * cosf((glm::two_pi<float>() / 3) * i);
        z = 2.f * sinf((glm::two_pi<float>() / 3) * i);
        prog.setUniform(name.str().c_str(), view * glm::vec4(x, 1.f, z + 1.f, 1.f));
    }

    prog.setUniform("Lights[0].Ld", vec3(0.0f, 0.0f, 1.8f));
    prog.setUniform("Lights[1].Ld", vec3(0.0f, 1.8f, 0.0f));
    prog.setUniform("Lights[2].Ld", vec3(1.8f, 0.0f, 0.0f));

    prog.setUniform("Lights[0].La", vec3(0.0f, 0.0f, 0.1f));
    prog.setUniform("Lights[1].La", vec3(0.0f, 0.1f, 0.0f));
    prog.setUniform("Lights[2].La", vec3(0.1f, 0.0f, 0.0f));

    prog.setUniform("Lights[0].Ls", vec3(0.0f, 0.0f, 0.8f));
    prog.setUniform("Lights[1].Ls", vec3(0.0f, 0.8f, 0.0f));
    prog.setUniform("Lights[2].Ls", vec3(0.8f, 0.0f, 0.0f));

    prog.setUniform("Material.Kd", 0.1f, 0.1f, 0.1f);
    prog.setUniform("Material.Ka", 0.9f, 0.9f, 0.9f);
    prog.setUniform("Material.Ks", 0.1f, 0.1f, 0.1f);
    prog.setUniform("Material.Shininess", 100.f);
    //prog.setUniform("Light.Ld", 1.0f, 1.0f, 1.0f);
    //prog.setUniform("Light.La", 0.4f, 0.4f, 0.4f);
    //prog.setUniform("Light.Ls", 1.0f, 1.0f, 1.0f);
    //prog.setUniform("Light.Position", view * glm::vec4(5.f, 5.f, 2.f, 1.f));
}

void SceneBasic_Uniform::compile()
{
	try {
		prog.compileShader("shader/basic_uniform.vert");
		prog.compileShader("shader/basic_uniform.frag");
		prog.link();
		prog.use();
	} catch (GLSLProgramException &e) {
		cerr << e.what() << endl;
		exit(EXIT_FAILURE);
	}
}

void SceneBasic_Uniform::update( float t )
{

}

void SceneBasic_Uniform::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    setMatrices();
    TeapotMesh.render();

    
}

void SceneBasic_Uniform::resize(int w, int h)
{

    glViewport(0, 0, w, h);
    width = w;
    height = h;
    projection = glm::perspective(glm::radians(70.0f), (float)w / h, 0.3f, 100.f);
}

void SceneBasic_Uniform::setMatrices()
{
    glm::mat4 mv = view * model;
    prog.setUniform("ModelViewMatrix", mv);
    prog.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    prog.setUniform("MVP", projection * mv);
    prog.setUniform("ProjectionMatrix", projection);
}