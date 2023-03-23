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
SceneBasic_Uniform::SceneBasic_Uniform() : plane(60.0f, 60.0f, 1, 1), skybox(350.0f)
{
    Planet1Mesh = ObjMesh::load("media/Meshes/Planet.obj", true, true);
    CrystalMesh = ObjMesh::load("media/Meshes/Crystalline.obj", true, true);
    MoonMesh = ObjMesh::load("media/Meshes/MoonRock.obj", true, true);

    Light1Pos = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    Light2Pos = vec4(1.0f, 1.0f, 0.0f, 0.0f);
    Light3Pos = vec4(-1.0f, 0.15f, 0.0f, 0.0f);
    SpotLightPos = glm::vec3(0.0f, -14.0f, 0.0f);
    SpotLightDir = vec3(-glm::vec4(0.0f, -10.0f, 0.0f, 1.0f));

    setupCamera();
}

void SceneBasic_Uniform::initScene()
{
    compile();

    std::cout << std::endl;

    prog.printActiveUniforms();

    glEnable(GL_DEPTH_TEST);

    view = glm::lookAt(vec3(0.f, 0.f, 6.f), vec3(0.f, 0.f, 0.f), vec3(0.f, 1.f, 0.f));
    projection = mat4(1.f);

    // Center yellow point light
    prog.setUniform("Lights[0].Position", Light1Pos);
    prog.setUniform("Lights[0].Ld", vec3(2.0f, 1.0f, 0.3f));
    prog.setUniform("Lights[0].La", vec3(0.2f, 0.15f, 0.05f));
    prog.setUniform("Lights[0].Ls", vec3(1.0f, 0.5f, 0.2f));


    // Top light-blue directional light
    prog.setUniform("Lights[1].Position", Light2Pos);
    prog.setUniform("Lights[1].Ld", vec3(0.3f, 0.6f, 0.8f));
    prog.setUniform("Lights[1].La", vec3(0.0f, 0.1f, 0.0f));
    prog.setUniform("Lights[1].Ls", vec3(0.3f, 0.6f, 0.8f));

    // Side pink directional light
    prog.setUniform("Lights[2].Position", Light3Pos);
    prog.setUniform("Lights[2].Ld", vec3(0.4f, 0.1f, 0.1f));
    prog.setUniform("Lights[2].La", vec3(0.1f, 0.0f, 0.1f));
    prog.setUniform("Lights[2].Ls", vec3(0.4f, 0.1f, 0.1f));



    mat3 normalMatrix = mat3(vec3(view[0]), vec3(view[1]), vec3(view[2]));
    prog.setUniform("Spotlight.Position", vec3(view * glm::vec4(SpotLightPos, 1.0f)));
    prog.setUniform("Spotlight.Direction", normalMatrix * SpotLightDir);
    prog.setUniform("Spotlight.Ld", vec3(2.0f, 1.6f, 0.6f));
    prog.setUniform("Spotlight.Ls", vec3(0.2f, 0.15f, 0.05f));
    prog.setUniform("Spotlight.La", vec3(1.0f, 0.8f, 0.3f));
    prog.setUniform("Spotlight.Exponent", 50.f);
    prog.setUniform("Spotlight.Cutoff", glm::radians(15.f));

    prog.setUniform("Fog.MaxDist", 30.0f);
    prog.setUniform("Fog.MinDist", 10.0f);
    prog.setUniform("Fog.Colour", vec3(0.5f, 0.5f, 0.5f));

    CrystalBCTex =
        Texture::loadTexture("media/CrystalTextures/CrystalBase_initialShadingGroup_BaseColor.1001.png");
    CrystalNMTex =
        Texture::loadTexture("media/CrystalTextures/CrystalBase_initialShadingGroup_Normal.1001.png");

    Planet1BCTex =
        Texture::loadTexture("media/PlanetTextures/PlanetLower_1001_BaseColor.png");
    Planet1NMTex =
        Texture::loadTexture("media/PlanetTextures/PlanetLower_1001_Normal.png");

    Planet2BCTex = 
        Texture::loadTexture("media/PlanetTextures/PlanetLower_1002_BaseColor.png");
    Planet2NMTex = 
        Texture::loadTexture("media/PlanetTextures/PlanetLower_1002_Normal.png");

    MoonBCTex = 
        Texture::loadTexture("media/MoonTextures/MoonLow_1001_BaseColor.png");
    MoonNMTex =
        Texture::loadTexture("media/MoonTextures/MoonLow_1001_Normal.png");

    PlaneTex =
        Texture::loadTexture("media/PlaneTextures/PlaneTex.png");

    GLuint SkyboxTex = Texture::loadHdrCubeMap("media/Skybox/space");

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
        Skyboxprog.compileShader("shader/skybox.vert");
        Skyboxprog.compileShader("shader/skybox.frag");
        Skyboxprog.link();
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
    if (!m_animate) return;

    Planet1Angle = t * Planet1RotationSpeed;
    Planet2Angle = t * Planet2RotationSpeed;
    moonAngle = t * moonRotationSpeed;
    crystalOffset = sin(t * crystalLevSpeed) * crystalLevAmplitude;

}

void SceneBasic_Uniform::render()
{
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    prog.use();
    prog.setUniform("Material.Kd", 1.0f, 1.0f, 1.0f);
    prog.setUniform("Material.Ks", 1.05f, 1.05f, 1.05f);
    prog.setUniform("Material.Ka", 0.5f, 0.5f, 0.5f);
    prog.setUniform("Material.Shininess", 180.0f);

    setTextures(Planet1BCTex, Planet1NMTex);
    model = mat4(1.0f);
    model = glm::rotate(model, glm::radians(Planet1Angle), vec3(0.0f, 1.0f, 0.0f));
    model = glm::translate(model, vec3(Planet1Distance, -0.6f, 0.0f));
    model = glm::scale(model, vec3(0.6f, 0.6f, 0.6f));
    setMatrices();
    setLightUniforms();
    Planet1Mesh->render();

    setTextures(Planet2BCTex, Planet2NMTex);
    model = mat4(1.0f);
    model = glm::rotate(model, glm::radians(Planet2Angle), vec3(0.0f, 1.0f, 0.0f));
    model = glm::translate(model, vec3(Planet2Distance, 0.0f, 0.0f));
    setMatrices();
    Planet1Mesh->render();

    setTextures(MoonBCTex, MoonNMTex);
    model = glm::rotate(model, glm::radians(moonAngle), vec3(0.0f, 1.0f, 0.0f));
    model = glm::translate(model, vec3(moonDistance, 0.0f, 0.0f));
    model = glm::scale(model, vec3(0.4f, 0.4f, 0.4f));
    setMatrices();
    MoonMesh->render();

    prog.setUniform("Material.Ks", 1.0f, 1.0f, 1.0f);
    
    setTextures(CrystalBCTex, CrystalNMTex);
    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0f, crystalOffset, 0.0f));
    setMatrices();
    CrystalMesh->render();


    //Alpha shader pass
    Alphaprog.use();

    setTextures(PlaneTex, PlaneTex);
    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0f, -1.5f, 0.0f));
    setAlphaMatrices();
    plane.render();

    //Skybox shader pass
    Skyboxprog.use();

    model = mat4(1.0f);
    setSkyboxMatrices();
    skybox.render();

    
}

void SceneBasic_Uniform::resize(int w, int h)
{
    glViewport(0, 0, w, h);
    width = w;
    height = h;
    projection = glm::perspective(glm::radians(70.0f), (float)w / h, 0.3f, 500.f);
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

void SceneBasic_Uniform::setSkyboxMatrices()
{
    glm::mat4 mv = view * model;
    Skyboxprog.setUniform("MVP", projection * mv);
}

void SceneBasic_Uniform::setLightUniforms()
{

    prog.setUniform("Lights[0].Position", view * Light1Pos);
    prog.setUniform("Lights[1].Position", view * Light2Pos);
    prog.setUniform("Lights[2].Position", view * Light3Pos);
    mat3 normalMatrix = mat3(vec3(view[0]), vec3(view[1]), vec3(view[2]));
    prog.setUniform("Spotlight.Position", vec3(view * glm::vec4(SpotLightPos, 1.0f)));
    prog.setUniform("Spotlight.Direction", normalMatrix * SpotLightDir);
}

void SceneBasic_Uniform::setTextures(GLuint Tex1, GLuint Tex2)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Tex1);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, Tex2);
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
