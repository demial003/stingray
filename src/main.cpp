#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <iostream>
#include <string>

#include <stb/stb_image.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/version.h>

#include <physics/particle.h>
#include <physics/pworld.h>

#include <renderer/camera.h>
#include <renderer/cylinder.h>
#include <renderer/mesh.h>
#include <renderer/model.h>
#include <renderer/shader.h>
#include <renderer/sphere.h>

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>

#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

extern "C" const char *__lsan_default_suppressions() {
  return "leak:libnvidia-glcore\n"
         "leak:libGLX\n"
         "leak:libglfw\n";
}

float initialTheta = glm::radians(100.0f);

bool showWireframe = false;

stingray::renderer::Sphere sphere = stingray::renderer::Sphere(100, 100);
stingray::renderer::Cylinder cylinder = stingray::renderer::Cylinder(100, 5);
stingray::renderer::Sphere lightSphere(10, 10);

stingray::physics::Vec3 GRAVITY(0.0, -9.81, 0.0);

unsigned int gridVBO;
unsigned int gridVAO;

struct Bob {
  stingray::physics::Particle particle;
};

struct Rod {
  stingray::physics::Particle particle;
  stingray::physics::ParticleRod rod;
};

Bob b;
Rod r;

const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 900;

unsigned int vertPosLoc = 0;
unsigned int vertNormLoc = 1;
unsigned int vertTexLoc = 2;

stingray::renderer::Camera camera(glm::vec3(0.0f, 0.0f, 20.0f));

float lastX = SCR_WIDTH / 2.0;
float lastY = SCR_HEIGHT / 2.0;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

unsigned int whiteTex;

void setupGrid() {
  float vertices[] = {
      -1.0f, 0.0f, -1.0f, -1.0f, 0.0f, 1.0f,  1.0f, 0.0f, 1.0f,
      1.0f,  0.0f, -1.0f, -1.0f, 0.0f, -1.0f, 1.0f, 0.0f, 1.0f,
  };
  glGenBuffers(1, &gridVBO);
  glGenVertexArrays(1, &gridVAO);
  glBindVertexArray(gridVAO);

  glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(vertPosLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                        (void *)0);

  glEnableVertexAttribArray(vertPosLoc);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}
void drawBob(stingray::physics::Vec3 position,
             stingray::renderer::Shader &shader) {
  glm::mat4 model(1.0f);
  model = glm::translate(model, glm::vec3(position.x, position.y, position.z));
  model = glm::scale(model, glm::vec3(0.8f));

  shader.setMat4("model", model);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, whiteTex);
  shader.setInt("material.texture_diffuse1", 0);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, whiteTex);

  sphere.Draw(shader);
}

void drawRod(stingray::physics::Vec3 rPos, stingray::physics::Vec3 bPos,
             stingray::renderer::Shader &shader) {
  stingray::physics::Vec3 dir = rPos - bPos;
  dir.normalize();
  float theta = dir.dotProduct(stingray::physics::Vec3(0, 1, 0));
  theta = acosf(theta);

  glm::vec3 axis =
      -glm::cross(glm::vec3(dir.x, dir.y, dir.z), glm::vec3(0, 1, 0));

  glm::mat4 model(1.0f);
  model = glm::translate(model, glm::vec3(rPos.x, rPos.y, rPos.z));
  if (glm::length(axis) > 1e-6f) {
    model = glm::rotate(model, theta, axis);
  }
  model = glm::scale(model, glm::vec3(0.1f, 1.0f, 0.1f));

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, whiteTex);
  shader.setInt("material.texture_diffuse1", 0);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, whiteTex);
  shader.setInt("material.texture_specular1", 1);

  shader.setMat4("model", model);
  cylinder.Draw(shader);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  if (!glfwWindowShouldClose(window)) {
    glViewport(0, 0, width, height);
  }
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  camera.processMouseScroll(static_cast<float>(yoffset));
}

void mouse_callback(GLFWwindow *window, double xPosIn, double yPosIn) {
  float xpos = static_cast<float>(xPosIn);
  float ypos = static_cast<float>(yPosIn);

  if (firstMouse) {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }

  float xoffset = xpos - lastX;
  float yoffset = lastY - ypos;

  lastX = xpos;
  lastY = ypos;

  camera.processMouseMovement(xoffset, yoffset);
}
void processInput(GLFWwindow *window) {
  using namespace stingray::renderer;
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }

  if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  }

  if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) {
    if (!showWireframe) {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      showWireframe = true;
    } else {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      showWireframe = false;
    }
  }

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    camera.processKeyboard(FORWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    camera.processKeyboard(BACKWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    camera.processKeyboard(LEFT, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    camera.processKeyboard(RIGHT, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    camera.processKeyboard(DOWN, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    camera.processKeyboard(UP, deltaTime);

  if (camera.position.y <= -2.0f) {
    camera.position.y = -2.0f;
  }
}
void renderScene(stingray::renderer::Shader &shader, int fbWidth,
                 int fbHeight) {
  glm::mat4 view = camera.getViewMatrix();

  glm::mat4 projection = glm::mat4(1.0f);
  projection = glm::perspective((glm::radians(camera.zoom)),
                                (float)fbWidth / (float)fbHeight, 0.1f, 100.0f);
  shader.setMat4("view", view);
  shader.setMat4("projection", projection);

  drawRod(r.particle.getPosition(), b.particle.getPosition(), shader);
  drawBob(b.particle.getPosition(), shader);
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

  int fbWidth, fbHeight;
  glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
  glViewport(0, 0, fbWidth, fbHeight);
  glDepthFunc(GL_LEQUAL);
  glCullFace(GL_BACK);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetScrollCallback(window, scroll_callback);
  glfwSetCursorPosCallback(window, mouse_callback);

  stingray::renderer::Shader shader("../shaders/shader.vert",
                                    "../shaders/shader.frag");
  stingray::renderer::Shader lightShader("../shaders/lightShader.vert",
                                         "../shaders/lightShader.frag");
  stingray::renderer::Shader gridShader("../shaders/grid.vert",
                                        "../shaders/grid.frag");

  stingray::physics::ParticleWorld world(2, 4);

  sphere.genVertices();
  cylinder.genVertices();
  lightSphere.genVertices();

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

  setupGrid();

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

  ImGui_ImplGlfw_InitForOpenGL(window, true);

  ImGui_ImplOpenGL3_Init();

  stbi_set_flip_vertically_on_load(true);
  stingray::renderer::Model bag("../assets/backpack.obj");

  glGenTextures(1, &whiteTex);
  glBindTexture(GL_TEXTURE_2D, whiteTex);
  unsigned char whitePixel[] = {255, 255, 255, 255};
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE,
               whitePixel);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  while (!glfwWindowShouldClose(window)) {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    processInput(window);

    world.startFrame();
    world.runPhysics(1.0 / 60.0);

    // ImGui::ShowDemoWindow(&show);

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

    glm::mat4 view = camera.getViewMatrix();

    glm::mat4 projection = glm::mat4(1.0f);
    projection =
        glm::perspective((glm::radians(camera.zoom)),
                         (float)fbWidth / (float)fbHeight, 0.1f, 100.0f);

    glBindVertexArray(gridVAO);
    gridShader.use();
    gridShader.setMat4("view", view);
    gridShader.setMat4("projection", projection);
    glm::mat4 gridModel = glm::mat4(1.0);
    gridModel = glm::translate(gridModel, glm::vec3(0.0f, -3.0f, 0.0f));
    gridModel = glm::scale(gridModel, glm::vec3(50.0f, 1.0f, 50.0f));
    gridShader.setMat4("model", gridModel);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    shader.use();
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);
    // shader.setVec3("diffuseColor", glm::vec3(0.8f, 0.5f, 0.2f));
    shader.setVec3("viewPos", camera.position);
    shader.setFloat("material.shininess", 32.0f);

    glm::vec3 lightPos(5.0f, 5.0f, 7.0f);

    shader.setVec3("dirLight.direction", -lightPos);
    shader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
    shader.setVec3("dirLight.diffuse", 0.5f, 0.5f, 0.5f);
    shader.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);

    glm::mat4 bagModel(1.0f);
    bagModel = glm::translate(bagModel, glm::vec3(20.0f, 0.0f, 4.0f));
    shader.setMat4("model", bagModel);
    shader.setVec3("material.diffuseColor", glm::vec3(0.8f, 0.5f, 0.2f));
    bag.Draw(shader);

    shader.setVec3("material.diffuseColor", glm::vec3(1.0f));
    renderScene(shader, fbWidth, fbHeight);

    lightShader.use();

    glm::mat4 model(1.0f);
    model = glm::translate(model, lightPos);
    model = glm::scale(model, glm::vec3(0.2f));

    lightShader.setMat4("view", view);
    lightShader.setMat4("projection", projection);
    lightShader.setMat4("model", model);
    lightSphere.Draw(lightShader);

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
