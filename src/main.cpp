#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <iostream>
#include <string>

#include <random>
#include <stb/stb_image.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/version.h>

#include <physics/particle.h>
#include <physics/pworld.h>

#include <renderer/camera.h>
#include <renderer/cube.h>
#include <renderer/cylinder.h>
#include <renderer/mesh.h>
#include <renderer/model.h>
#include <renderer/quad.h>
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
bool toggleCursor = false;

stingray::renderer::Sphere sphere = stingray::renderer::Sphere(100, 100);
stingray::renderer::Cylinder cylinder = stingray::renderer::Cylinder(100, 5);
stingray::renderer::Sphere lightSphere(10, 10);
stingray::renderer::Quad grid(100, 100);
stingray::renderer::Quad ocean(256, 256);
stingray::renderer::Cube skybox;

stingray::physics::Vec3 GRAVITY(0.0, -9.81, 0.0);

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

stingray::renderer::Camera camera(glm::vec3(0.0f, 0.05f, -20.0f));

float lastX = SCR_WIDTH / 2.0;
float lastY = SCR_HEIGHT / 2.0;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

unsigned int whiteTex;
unsigned int cubeMap;

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
};

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  if (!glfwWindowShouldClose(window)) {
    glViewport(0, 0, width, height);
  }
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  camera.processMouseScroll(static_cast<float>(yoffset));
}

void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mods) {
  using namespace stingray::renderer;
  if (action != GLFW_PRESS)
    return;
  if (key == GLFW_KEY_ESCAPE) {
    glfwSetWindowShouldClose(window, true);
  }

  if (key == GLFW_KEY_LEFT_CONTROL) {
    camera.toggle();
    if (!toggleCursor) {
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
      toggleCursor = true;
    } else {
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
      toggleCursor = false;
    }
  }

  if (key == GLFW_KEY_TAB) {
    if (!showWireframe) {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      showWireframe = true;
    } else {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      showWireframe = false;
    }
  }
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

unsigned int loadCubemap(std::vector<std::string> faces) {
  unsigned int textureID;
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

  int width, height, nrChannels;
  for (unsigned int i = 0; i < faces.size(); i++) {
    unsigned char *data =
        stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
    if (data) {
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height,
                   0, GL_RGB, GL_UNSIGNED_BYTE, data);
      stbi_image_free(data);
    } else {
      std::cout << "Cubemap tex failed to load at path: " << faces[i]
                << std::endl;
      stbi_image_free(data);
    }
  }
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  return textureID;
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
  glCullFace(GL_BACK);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glEnable(GL_CULL_FACE);

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetScrollCallback(window, scroll_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetKeyCallback(window, key_callback);

  stingray::renderer::Shader shader("../shaders/shader.vert",
                                    "../shaders/shader.frag");
  stingray::renderer::Shader lightShader("../shaders/lightShader.vert",
                                         "../shaders/lightShader.frag");
  stingray::renderer::Shader gridShader("../shaders/grid.vert",
                                        "../shaders/grid.frag");
  stingray::renderer::Shader oceanShader("../shaders/ocean.vert",
                                         "../shaders/ocean.frag");
  stingray::renderer::Shader skyboxShader("../shaders/skybox.vert",
                                          "../shaders/skybox.frag");

  stingray::physics::ParticleWorld world(2, 4);

  sphere.genVertices();
  cylinder.genVertices();
  lightSphere.genVertices();
  grid.genVertices();
  ocean.genVertices();
  skybox.genVertices();

  std::vector<std::string> faces{
      "../assets/qwantani-cubemap/px.png", "../assets/qwantani-cubemap/nx.png",
      "../assets/qwantani-cubemap/py.png", "../assets/qwantani-cubemap/ny.png",
      "../assets/qwantani-cubemap/pz.png", "../assets/qwantani-cubemap/nz.png"};
  cubeMap = loadCubemap(faces);

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

  stbi_set_flip_vertically_on_load(true);
  stingray::renderer::Model bag("../assets/backpack.obj");

  glGenTextures(1, &whiteTex);
  glBindTexture(GL_TEXTURE_2D, whiteTex);
  unsigned char whitePixel[] = {255, 255, 255, 255};
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE,
               whitePixel);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  float scale = 200.0;
  float shininess = 64.0f;
  float wavelength = 4.0;
  float amp = 0.1;
  float speed = 1.0;
  float steepness = 0.5;
  float freq = 0;
  float windAngle = glm::radians(225.0);

  std::mt19937 rng(12345);
  std::uniform_real_distribution<float> logDist(std::log(0.5), std::log(2.0f));
  std::uniform_real_distribution<float> angleDist(-0.6f, 0.6f);

  struct WaveCpu {
    float lenRatio;
    float angleOffset;
  };
  WaveCpu waves[8];
  WaveCpu textureWaves[8];

  for (auto &w : waves) {
    w.lenRatio = std::exp(logDist(rng));
    w.angleOffset = angleDist(rng);
  }

  for (auto &w : textureWaves) {
    w.lenRatio = std::exp(logDist(rng));
    w.angleOffset = angleDist(rng);
  }

  while (!glfwWindowShouldClose(window)) {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    processInput(window);

    b.particle.setAcceleration(GRAVITY);
    world.startFrame();
    world.runPhysics(1.0 / 60.0);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("params");
    // ImGui::SliderFloat("gravity", (GRAVITY.getY()), -20.0f, 0.0f, "%.4f");
    ImGui::SliderFloat("shininess", &shininess, 0.0f, 128.0f, "%.2f");
    ImGui::SliderFloat("wavelength", &wavelength, 1.0f, 10.0f, "%.2f");
    ImGui::SliderFloat("amplitude", &amp, 0.0f, 1.0f, "%.4f");
    ImGui::SliderFloat("speed", &speed, 0.0f, 3.0f, "%.2f");
    ImGui::SliderAngle("wind angle", &windAngle, 0.0f, 360.0f);
    ImGui::SliderFloat("steepness", &steepness, 0.0f, 1.0f, "%.4f");
    ImGui::End();

    glm::vec3 lightPos(6.0f, 2.0f, 50.0f);
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 projection = glm::mat4(1.0f);
    projection =
        glm::perspective((glm::radians(camera.zoom)),
                         (float)fbWidth / (float)fbHeight, 0.1f, 100.0f);

    // glBindVertexArray(gridVAO);
    // gridShader.use();
    // gridShader.setMat4("view", view);
    // gridShader.setMat4("projection", projection);
    // glm::mat4 gridModel = glm::mat4(1.0);
    // gridModel = glm::translate(gridModel, glm::vec3(0.0f, -3.0f, 0.0f));
    // gridModel = glm::scale(gridModel, glm::vec3(50.0f, 1.0f, 50.0f));
    // gridShader.setMat4("model", gridModel);
    // grid.Draw(gridShader);
    // glDrawArrays(GL_TRIANGLES, 0, 6);

    oceanShader.use();
    oceanShader.setMat4("view", view);
    oceanShader.setMat4("projection", projection);
    glm::mat4 oceanModel = glm::mat4(1.0);
    oceanModel = glm::translate(oceanModel, glm::vec3(0.0, -3.0, 0.0));
    oceanModel = glm::scale(oceanModel, glm::vec3(scale));
    oceanShader.setMat4("model", oceanModel);
    oceanShader.setVec3("material.diffuseColor",
                        glm::vec3(0.05f, 0.20f, 0.30f));
    oceanShader.setFloat("material.shininess", shininess);
    oceanShader.setVec3("dirLight.direction", -lightPos);
    oceanShader.setVec3("dirLight.ambient", 0.35f, 0.35f, 0.35f);
    oceanShader.setVec3("dirLight.diffuse", 0.5f, 0.5f, 0.5f);
    oceanShader.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);
    oceanShader.setFloat("uTime", (float)glfwGetTime());
    oceanShader.setFloat("steepness", steepness);
    oceanShader.setVec3("viewPos", camera.position);
    oceanShader.setFloat("gravity", GRAVITY.y);
    oceanShader.setInt("numWaves", 8);
    oceanShader.setInt("numTextureWaves", 8);
    oceanShader.setInt("k", 1);

    for (int i = 0; i < 8; i++) {
      float w1 = wavelength * waves[i].lenRatio;
      freq = 2.0 * M_PI / w1;
      float a = amp * waves[i].lenRatio;
      float angle = windAngle + waves[i].angleOffset;
      glm::vec2 d(cos(angle), sin(angle));
      float phaseOffset = speed * sqrt(-GRAVITY.y * freq);
      std::string p = "geometricWaves[" + std::to_string(i) + "].";
      oceanShader.setFloat(p + "freq", freq);
      oceanShader.setFloat(p + "amp", a);
      oceanShader.setFloat(p + "phaseOffset", phaseOffset);
      oceanShader.setVec2(p + "dir", d);
    }

    for (int i = 0; i < 8; i++) {
      float w1 = wavelength * textureWaves[i].lenRatio;
      freq = 2.0 * M_PI / w1;
      float a = amp * textureWaves[i].lenRatio;
      float angle = windAngle + textureWaves[i].angleOffset;
      glm::vec2 d(cos(angle), sin(angle));
      float phaseOffset = speed * sqrt(-GRAVITY.y * freq);
      std::string p = "textureWaves[" + std::to_string(i) + "].";
      oceanShader.setFloat(p + "freq", freq);
      oceanShader.setFloat(p + "amp", a);
      oceanShader.setFloat(p + "phaseOffset", phaseOffset);
      oceanShader.setVec2(p + "dir", d);
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);
    ocean.Draw(oceanShader);

    shader.use();
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);
    shader.setVec3("viewPos", camera.position);
    shader.setFloat("material.shininess", shininess);

    shader.setVec3("dirLight.direction", -lightPos);
    shader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
    shader.setVec3("dirLight.diffuse", 0.5f, 0.5f, 0.5f);
    shader.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);

    // glm::mat4 bagModel(1.0f);
    // bagModel = glm::translate(bagModel, glm::vec3(20.0f, 0.0f, 4.0f));
    // shader.setMat4("model", bagModel);
    // shader.setVec3("material.diffuseColor", glm::vec3(0.8f, 0.5f, 0.2f));
    // bag.Draw(shader);

    // shader.setVec3("material.diffuseColor", glm::vec3(1.0f));
    // renderScene(shader, fbWidth, fbHeight);

    lightShader.use();

    glm::mat4 model(1.0f);
    model = glm::translate(model, lightPos);
    model = glm::scale(model, glm::vec3(0.2f));

    lightShader.setMat4("view", view);
    lightShader.setMat4("projection", projection);
    lightShader.setMat4("model", model);
    lightSphere.Draw(lightShader);

    glDepthMask(GL_FALSE);
    skyboxShader.use();
    glm::mat4 skyboxView = glm::mat4(glm::mat3(camera.getViewMatrix()));
    skyboxShader.setMat4("view", skyboxView);
    skyboxShader.setMat4("projection", projection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);
    skybox.Draw(skyboxShader);
    glDepthMask(GL_TRUE);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
    glfwWaitEventsTimeout(1.0 / 60.0);
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  glfwTerminate();

  return 0;
}
