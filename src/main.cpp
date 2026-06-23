#include "stingray/particle.h"
#include "utils/mesh.h"
#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <iostream>
#include <stingray/pfgen.h>
#include <stingray/plinks.h>
#include <utils/cylinder.h>
#include <utils/shader.h>
#include <utils/sphere.h>

#include <glm/ext/matrix_float4x4.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

float deltaTime = 0.0f;
float lastFrame = 0.0f;

stingray::Vec3 GRAVITY(0.0, -9.81, 0.0);
stingray::ParticleGravity gravity(GRAVITY);

struct Bob {
  stingray::Particle particle;
  stingray::ParticleRod rod;

  void render(glm::mat4 model, int idxSize, int model_loc,
              utils::Sphere &sphere) {
    stingray::Vec3 position;
    particle.getPosition(&position);

    model =
        glm::translate(model, glm::vec3(position.x, position.y, position.z));
    stingray::Vec3 accel = particle.getAcceleration();
    // std::cout << position.x << ", " << position.y << ", " << position.z <<
    // "\n"; std::cout << particle.forceAccum.x << ", " << particle.forceAccum.y
    // << ", "
    //           << particle.forceAccum.z << "\n";
    // std::cout << "-----------------------\n";

    glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));
    sphere.RenderEBO(GL_TRIANGLES, idxSize);
  }
};

Bob b;

struct Rod {
  utils::Cylinder cylinder;
  stingray::Particle particle;
  stingray::ParticleRod rod;
};

const unsigned int SCR_WIDTH = 900;
const unsigned int SCR_HEIGHT = 900;

unsigned int vertPosLoc = 0;
unsigned int vertColorLoc = 1;

utils::Sphere sphere = utils::Sphere(100, 100);
utils::Cylinder cylinder = utils::Cylinder(100, 2);

unsigned int vertsSize = cylinder.numVertices();
unsigned int idxSize = sphere.numElements();

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 10.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float yaw = -90.0f;
float pitch = 0.0f;

float lastX = 400;
float lastY = 300;

bool firstMouse = true;
float fov = 45.0f;

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  if (!glfwWindowShouldClose(window)) {
    glViewport(0, 0, width, height);
  }
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  fov -= (float)yoffset;
  if (fov < 1.0f)
    fov = 1.0f;
  if (fov > 45.0f)
    fov = 45.0f;
}

void mouse_callback(GLFWwindow *window, double xPos, double yPos) {
  if (firstMouse) {
    lastX = xPos;
    lastY = yPos;
    firstMouse = false;
  }
  float xoffset = xPos - lastX;
  float yoffset = lastY - yPos;

  lastX = xPos;
  lastY = yPos;

  const float sensitivity = 0.08f;
  xoffset *= sensitivity;
  yoffset *= sensitivity;

  yaw += xoffset;
  pitch += yoffset;

  if (pitch > 89.0f)
    pitch = 89.0f;
  if (pitch < -89.0f)
    pitch = -89.9f;

  glm::vec3 direction;
  direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
  direction.y = sin(glm::radians(pitch));
  direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
  cameraFront = glm::normalize(direction);
}
void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }

  const float cameraSpeed = 3.5f * deltaTime;
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    cameraPos += cameraSpeed * cameraFront;
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    cameraPos -= cameraSpeed * cameraFront;
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    cameraPos -=
        glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    cameraPos +=
        glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
  }
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
    cameraPos += 2.0f * cameraSpeed * cameraUp;
  }
  if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) {
    cameraPos -= 2.0f * cameraSpeed * cameraUp;
  }
}
void renderScene(utils::Shader shader) {
  glm::mat4 model = glm::mat4(1.0f);
  // model = glm::translate(model, glm::vec3(0.0f, 5.0f, 0.0f));
  // model = glm::scale(model, glm::vec3(0.1f, 2.0f, 0.1f));

  glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

  glm::mat4 projection = glm::mat4(1.0f);
  projection = glm::perspective(
      (glm::radians(fov)), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

  unsigned int view_loc = glGetUniformLocation(shader.ID, "view");
  unsigned int projection_loc = glGetUniformLocation(shader.ID, "projection");
  unsigned int model_loc = glGetUniformLocation(shader.ID, "model");

  glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view));
  glUniformMatrix4fv(projection_loc, 1, GL_FALSE, glm::value_ptr(projection));

  // cylinder.RenderVBO(GL_TRIANGLE_STRIP, 0, vertsSize / 3);
  b.particle.integrate(deltaTime);
  b.render(GL_TRIANGLES, idxSize, model_loc, sphere);
}

int main(void) {
  GLFWwindow *window;
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "pendulum-sim", NULL, NULL);
  if (window == nullptr) {
    std::cout << "Failed to open glfw window\n";
    glfwTerminate();
  }

  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize glad\n";
  }

  glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glCullFace(GL_BACK);
  // glEnable(GL_CULL_FACE);

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetScrollCallback(window, scroll_callback);
  glfwSetCursorPosCallback(window, mouse_callback);

  cylinder.initializeAtrributeLocations(vertPosLoc);
  sphere.initializeAtrributeLocations(vertPosLoc);
  utils::Shader shader("../include/utils/shader.vs",
                       "../include/utils/shader.fs");
  b.particle.setPosition(0.0f, 0.0f, 0.0f);
  b.particle.setVelocity(0.0f, 0.0f, 0.0f);
  b.particle.addForce(GRAVITY);
  b.particle.setMass(1.0f);
  b.particle.damping = 0.99f;

  while (!glfwWindowShouldClose(window)) {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    shader.use();
    processInput(window);

    glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    renderScene(shader);

    glfwSwapBuffers(window);
    glfwWaitEventsTimeout(1.0 / 60.0);
  }

  glfwTerminate();
  return 0;
}
