#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <iostream>
#include <stingray/particle.h>
#include <utils/shader.h>
#include <utils/sphere.h>

unsigned int VAO;
unsigned int VBO;
unsigned int vertPosLoc = 0;

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
  glBindVertexArray(VAO);

  float vertices[96];
  utils::Sphere sphere = utils::Sphere();
  sphere.genVertices(vertices);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(vertPosLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                        (void *)0);
  glEnableVertexAttribArray(vertPosLoc);
}

int main(void) {
  GLFWwindow *window;
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

  window = glfwCreateWindow(900, 600, "pendulum-sim", NULL, NULL);
  if (window == nullptr) {
    std::cout << "Failed to open glfw window\n";
    glfwTerminate();
  }

  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize glad\n";
  }

  glViewport(0, 0, 900, 600);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

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
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 180);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
