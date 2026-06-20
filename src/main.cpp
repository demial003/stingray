#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <iostream>
#include <stingray/particle.h>
#include <utils/shader.h>
#include <utils/sphere.h>

unsigned int VAO;
unsigned int VBO;
unsigned int EBO;
unsigned int vertPosLoc = 0;
unsigned int vertsSize;
unsigned int idxSize;

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  if (!glfwWindowShouldClose(window)) {
    glViewport(0, 0, width, height);
  }
}

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
}

void setupGeometry() {
  glGenBuffers(1, &VBO);
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &EBO);
  glBindVertexArray(VAO);

  utils::Sphere sphere = utils::Sphere(100, 100);
  vertsSize = sphere.vboSize();
  idxSize = sphere.eboSize();
  float vertices[vertsSize];
  int indices[idxSize];
  sphere.genVertices(vertices, indices);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);
  glVertexAttribPointer(vertPosLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                        (void *)0);
  glEnableVertexAttribArray(vertPosLoc);
}

int main(void) {
  GLFWwindow *window;
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

  window = glfwCreateWindow(900, 900, "pendulum-sim", NULL, NULL);
  if (window == nullptr) {
    std::cout << "Failed to open glfw window\n";
    glfwTerminate();
  }

  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize glad\n";
  }

  glViewport(0, 0, 900, 900);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glCullFace(GL_BACK);
  glEnable(GL_CULL_FACE);

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  setupGeometry();
  utils::Shader shader("../include/utils/shader.vs",
                       "../include/utils/shader.fs");

  while (!glfwWindowShouldClose(window)) {
    shader.use();
    processInput(window);
    glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, idxSize, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
