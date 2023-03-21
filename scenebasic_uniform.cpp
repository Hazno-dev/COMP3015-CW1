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
#include "helper/glutils.h"
#include "helper/texture.h"


using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;

//SceneBasic_Uniform::SceneBasic_Uniform() : TorusMesh(0.7f, 0.3f, 50, 50) {}
SceneBasic_Uniform::SceneBasic_Uniform() : plane(50.0f, 50.0f, 1, 1)
{
    mesh = ObjMesh::load("media/Planet.obj", true, true);
}

void SceneBasic_Uniform::initScene()
{
    compile();

    std::cout << std::endl;

    prog.printActiveUniforms();

    glEnable(GL_DEPTH_TEST);
    
    setupCamera();

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

    prog.setUniform("Lights[0].Ld", vec3(1.0f, 1.0f, 1.0f));
    prog.setUniform("Lights[1].Ld", vec3(1.0f, 1.0f, 1.0f));
    prog.setUniform("Lights[2].Ld", vec3(1.0f, 1.0f, 1.0f));

    prog.setUniform("Lights[0].La", vec3(0.0f, 0.0f, 0.1f));
    prog.setUniform("Lights[1].La", vec3(0.0f, 0.1f, 0.0f));
    prog.setUniform("Lights[2].La", vec3(0.1f, 0.0f, 0.0f));

    prog.setUniform("Lights[0].Ls", vec3(1.0f, 1.0f, 1.0f));
    prog.setUniform("Lights[1].Ls", vec3(1.0f, 1.0f, 1.0f));
    prog.setUniform("Lights[2].Ls", vec3(1.0f, 1.0f, 1.0f));

    mat3 normalMatrix = mat3(vec3(view[0]), vec3(view[1]), vec3(view[2]));
    prog.setUniform("Spotlight.Position", normalMatrix * glm::vec3(0.0f, 1.0f, -2.0f));
    prog.setUniform("Spotlight.Direction", normalMatrix * vec3(-glm::vec4(0.0f, 10.0f, 0.0f, 1.0f)));
    prog.setUniform("Spotlight.Ld", glm::vec3(1.0f));
    prog.setUniform("Spotlight.Ls", glm::vec3(1.0f));
    prog.setUniform("Spotlight.La", glm::vec3(0.5f));
    prog.setUniform("Spotlight.Exponent", 50.f);
    prog.setUniform("Spotlight.Cutoff", glm::radians(15.f));

    prog.setUniform("Fog.MaxDist", 30.0f);
    prog.setUniform("Fog.MinDist", 10.0f);
    prog.setUniform("Fog.Colour", vec3(0.5f, 0.5f, 0.5f));

    Planet1BCTex =
        Texture::loadTexture("media/PlanetTextures/PlanetLower_1001_BaseColor.png");
    Planet1NMTex =
        Texture::loadTexture("media/PlanetTextures/PlanetLower_1001_Normal.png");

    PlaneTex =
        Texture::loadTexture("media/PlaneTextures/PlaneTex.png");


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Planet1BCTex);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, Planet1NMTex);
}

void SceneBasic_Uniform::setupCamera()
{
    cameraPos = vec3(0.0f, 0.0f, 6.0f);
    cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    cameraSpeed = 0.01f;
    mouseSensitivity = 0.1f;
    lastX = width / 2;
    lastY = height / 2;
    yaw = -90.0f;
    pitch = 0.0f;
}

void SceneBasic_Uniform::compile()
{
	try {
        Alphaprog.compileShader("shader/basic_alpha.vert");
        Alphaprog.compileShader("shader/basic_alpha.frag");
        Alphaprog.link();
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

    prog.use();
    prog.setUniform("Material.Kd", 1.0f, 1.0f, 1.0f);
    prog.setUniform("Material.Ks", 1.05f, 1.05f, 1.05f);
    prog.setUniform("Material.Ka", 0.5f, 0.5f, 0.5f);
    prog.setUniform("Material.Shininess", 180.0f);
    model = mat4(1.0f);
    model = glm::rotate(model, glm::radians(90.0f), vec3(0.0f, 1.0f, 0.0f));
    setMatrices();
    setLightUniforms();
    mesh->render();


    //Alpha shader pass
    Alphaprog.use();
    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0f, -0.45f, 0.0f));
    setAlphaMatrices();

    plane.render();

    
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

void SceneBasic_Uniform::setAlphaMatrices()
{
    glm::mat4 mv = view * model;
    Alphaprog.setUniform("ModelViewMatrix", mv);
    Alphaprog.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    Alphaprog.setUniform("MVP", projection * mv);
    Alphaprog.setUniform("ProjectionMatrix", projection);
}

void SceneBasic_Uniform::setLightUniforms()
{
    float x, z;
    for (int i = 0; i < 3; i++) {
        std::stringstream name;
        name << "Lights[" << i << "].Position";
        x = 2.f * cosf((glm::two_pi<float>() / 3) * i);
        z = 2.f * sinf((glm::two_pi<float>() / 3) * i);
        prog.setUniform(name.str().c_str(), view * glm::vec4(x, 1.f, z + 1.f, 1.f));
    }


    mat3 normalMatrix = mat3(vec3(view[0]), vec3(view[1]), vec3(view[2]));
    prog.setUniform("Spotlight.Position", vec3(view * glm::vec4(0.0f, 1.0f, -2.0f, 1.0f)));
    prog.setUniform("Spotlight.Direction", normalMatrix * vec3(-glm::vec4(0.0f, 10.0f, 0.0f, 1.0f)));
}

//
// updateCamera - is called from scenerunner and will handle WASD + mouse movement to modify the view.
// I was going to use a callback instead inside scenerunner.h, however that requires a static function that cant take additional parameters.
// There may be a minor jolt with quick right clicks 
//
void SceneBasic_Uniform::updateCamera(GLFWwindow* window)
{
    // Get the current cursor position
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    // Calculate the offset between the current and previous cursor positions
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    // Update the last cursor position
    lastX = xpos;
    lastY = ypos;

    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT)) {

        // Hide and lock the cursor
        if (glfwGetInputMode(window, GLFW_CURSOR) != GLFW_CURSOR_DISABLED) 
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        // Update the yaw and pitch based on the offsets
        yaw += xoffset;
        pitch += yoffset;

        // Clamp the pitch between -89 and 89 degrees to stop camera doing 360s
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        // Calculate the new camera front vector based on yaw and pitch
        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFront = glm::normalize(front);

        // Move the camera based on keyboard inputs
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            cameraPos += cameraSpeed * cameraFront;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            cameraPos -= cameraSpeed * cameraFront;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            cameraPos -= glm::normalize(glm::cross(cameraFront, glm::vec3(0.f, 1.f, 0.f))) * cameraSpeed;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            cameraPos += glm::normalize(glm::cross(cameraFront, glm::vec3(0.f, 1.f, 0.f))) * cameraSpeed;
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
            cameraPos += cameraSpeed * glm::vec3(0.f, 1.f, 0.f);
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
            cameraPos -= cameraSpeed * glm::vec3(0.f, 1.f, 0.f);
    }

    // If the right mouse button is not pressed, make the cursor visible and unlocked
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE) glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    

    // Update the view matrix for usage in setMatrices();
    view = glm::lookAt(cameraPos, cameraPos + cameraFront, glm::vec3(0.f, 1.f, 0.f));
}
