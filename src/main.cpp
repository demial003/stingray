#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <iostream>
#include <string>

#include <stingray/particle.h>
#include <stingray/pworld.h>

#include <utils/cylinder.h>
#include <utils/mesh.h>
#include <utils/shader.h>
#include <utils/sphere.h>

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>

#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

extern "C" const char *__lsan_default_suppressions() {
  return "leak:libnvidia-glcore\n"
         "leak:libGLX\n"
         "leak:libglfw\n";
}

float deltaTime = 0.0f;
float lastFrame = 0.0f;
bool show = true;
float initialTheta = glm::radians(100.0f);

utils::Sphere sphere = utils::Sphere(100, 100);
utils::Cylinder cylinder = utils::Cylinder(100, 5);
utils::Sphere lightSphere(10, 10);

stingray::Vec3 GRAVITY(0.0, -9.81, 0.0);

struct Bob {
  stingray::Particle particle;
};

struct Rod {
  stingray::Particle particle;
  stingray::ParticleRod rod;
};

Bob b;
Rod r;

const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 900;

unsigned int vertPosLoc = 0;
unsigned int vertNormLoc = 1;
unsigned int vertTexLoc = 2;

std::vector<std::string> vec;
unsigned int vertsSize = cylinder.numVertices();
unsigned int idxSize = sphere.numElements();

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 20.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float yaw = -90.0f;
float pitch = 0.0f;

float lastX = 400;
float lastY = 300;

bool firstMouse = true;
float fov = 45.0f;

void drawBob(stingray::Vec3 position, utils::Shader &shader, GLenum drawMode) {
  glm::mat4 model(1.0f);
  model = glm::translate(model, glm::vec3(position.x, position.y, position.z));
  model = glm::scale(model, glm::vec3(0.8f));

  shader.setMat4("model", model);
  sphere.RenderEBO(drawMode, idxSize);
}

void drawRod(stingray::Vec3 rPos, stingray::Vec3 bPos, utils::Shader &shader,
             GLenum drawMode) {
  stingray::Vec3 dir = rPos - bPos;
  dir.normalize();
  float theta = dir.dotProduct(stingray::Vec3(0, 1, 0));
  theta = acosf(theta);

  glm::vec3 axis =
      -glm::cross(glm::vec3(dir.x, dir.y, dir.z), glm::vec3(0, 1, 0));

  glm::mat4 model(1.0f);
  model = glm::translate(model, glm::vec3(rPos.x, rPos.y, rPos.z));
  if (glm::length(axis) > 1e-6f) {
    model = glm::rotate(model, theta, axis);
  }
  model = glm::scale(model, glm::vec3(0.1f, 1.0f, 0.1f));

  shader.setMat4("model", model);
  cylinder.RenderVBO(drawMode, 0, vertsSize);
}

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
    pitch = -89.0f;

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

  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
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
void renderScene(utils::Shader &shader, int fbWidth, int fbHeight) {
  glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

  glm::mat4 projection = glm::mat4(1.0f);
  projection = glm::perspective((glm::radians(fov)),
                                (float)fbWidth / (float)fbHeight, 0.1f, 100.0f);
  shader.setMat4("view", view);
  shader.setMat4("projection", projection);

  drawRod(r.particle.getPosition(), b.particle.getPosition(), shader,
          GL_TRIANGLE_STRIP);
  drawBob(b.particle.getPosition(), shader, GL_TRIANGLES);
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
    return -1;
  }

  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize glad\n";
    return -1;
  }

  glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glDepthFunc(GL_LEQUAL);
  glCullFace(GL_BACK);
  glEnable(GL_CULL_FACE);

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetScrollCallback(window, scroll_callback);
  glfwSetCursorPosCallback(window, mouse_callback);

  cylinder.initializeAtrributeLocations(vertPosLoc, vertNormLoc, vertTexLoc);
  sphere.initializeAtrributeLocations(vertPosLoc, vertNormLoc, vertTexLoc);
  lightSphere.initializeAtrributeLocations(vertPosLoc, vertNormLoc, vertTexLoc);
  utils::Shader shader("../shaders/shader.vert", "../shaders/shader.frag");
  utils::Shader lightShader("../shaders/lightShader.vert",
                            "../shaders/lightShader.frag");

  stingray::ParticleWorld world(2, 4);
  r.rod.particle[0] = &r.particle;
  r.rod.particle[1] = &b.particle;

  r.rod.length = cylinder.getHeight();
  world.getParticles().push_back(&b.particle);
  world.getParticles().push_back(&r.particle);
  world.getContactGenerators().push_back(&r.rod);

  b.particle.setPosition(
      cylinder.getHeight() * sinf(initialTheta),
      cylinder.getHeight() + -cylinder.getHeight() * cosf(initialTheta), 0.0f);
  b.particle.setMass(1.0f);
  b.particle.damping = 0.99f;
  b.particle.setAcceleration(GRAVITY);

  r.particle.setPosition(0.0, cylinder.getHeight(), 0.0);
  r.particle.setInverseMass(0);
  r.particle.damping = 0.99f;

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

  ImGui_ImplGlfw_InitForOpenGL(window, true);

  ImGui_ImplOpenGL3_Init();

  while (!glfwWindowShouldClose(window)) {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    processInput(window);

    world.startFrame();
    world.runPhysics(1.0 / 60.0);

    // ImGui::ShowDemoWindow(&show);

    int fbWidth, fbHeight;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    glViewport(0, 0, fbWidth, fbHeight);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // ImGui_ImplOpenGL3_NewFrame();
    // ImGui_ImplGlfw_NewFrame();
    // ImGui::NewFrame();
    // ImGui::Begin("Controls");
    // ImGui::Text("viewport=%d,%d,%d,%d\n", vp[0], vp[1], vp[2], vp[3]);
    // ImGui::Text(
    //     "dist=%.3f",
    //     (b.particle.getPosition() - r.particle.getPosition()).magnitude());
    // ImGui::End();

    shader.use();
    shader.setVec3("viewPos", cameraPos);
    shader.setFloat("material.shininess", 32.0f);

    glm::vec3 lightPos(5.0f, 5.0f, 7.0f);

    shader.setVec3("dirLight.direction", -lightPos);
    shader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
    shader.setVec3("dirLight.diffuse", 0.5f, 0.5f, 0.5f);
    shader.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);

    renderScene(shader, fbWidth, fbHeight);
    glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(
        (glm::radians(fov)), (float)fbWidth / (float)fbHeight, 0.1f, 100.0f);
    lightShader.use();

    glm::mat4 model(1.0f);
    model = glm::translate(model, lightPos);
    model = glm::scale(model, glm::vec3(0.2f));

    lightShader.setMat4("view", view);
    lightShader.setMat4("projection", projection);
    lightShader.setMat4("model", model);
    lightSphere.RenderEBO(GL_TRIANGLES, lightSphere.numElements());

    // ImGui::Render();
    // ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
    glfwWaitEventsTimeout(1.0 / 60.0);
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  glfwTerminate();

  return 0;
}
