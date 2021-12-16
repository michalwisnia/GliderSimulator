#pragma once


#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
private:
    glm::vec3 cameraPos;
    glm::quat cameraOrient;
    glm::vec3 cameraUp;
    glm::vec3 cameraTarget;
    float cameraSpeed = 10.f;;
    bool firstMouse;
    float lastX;
    float lastY;
    glm::mat4 viewMatrix;
    glm::vec3 directions;
    glm::vec2 rotation;

public:
    Camera(glm::vec3 Pos);
    void move(glm::vec3 directions, glm::vec2 rotations, float deltatime);
    void processInput(GLFWwindow* window, float deltatime, bool crashed);
    void mouseCallback(GLFWwindow* window, double xpos, double ypos);
    glm::mat4 getView();
    glm::vec3 getPos();
    glm::quat getOrient();
    glm::vec3 getUp();
    glm::vec3 getTarget();
};