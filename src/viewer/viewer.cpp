#include "types.hpp"
#include <cstdlib>
#include <memory>
#include <vector>
#include <viewer/viewer.hpp>


Viewer::ResultType Viewer::loadViewer(){
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  window = glfwCreateWindow(800, 600, "tonyMesh", NULL, NULL);
  if (window == NULL){
    glfwTerminate();
    return ResultType::GLFW_ERROR;
  }
  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
   return ResultType::GLAD_ERROR;
  }

  glViewport(0, 0, 800, 600);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  return ResultType::GenericError;
}

void Viewer::loop(){
  while(!glfwWindowShouldClose(window))
  {
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
}

void Viewer::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
  glViewport(0, 0, width, height);
}

void Viewer::setPoints(std::shared_ptr<std::vector<double>> points){
  this->points = points;
}

std::shared_ptr<std::vector<double>> Viewer::setPoints(const std::vector<std::vector<double>>& points){
  this->points = std::make_shared<std::vector<double>>(3 * points.size());

  #pragma omp parallel for
  for (int i = 0; i < points.size(); i++){
    const int row = 3*i;
    this->points->at(row) = points[i][0];
    this->points->at(row + 1) = points[i][1];
    this->points->at(row + 2) = points[i][2];
  }

  return this->points;
}

std::shared_ptr<std::vector<double>> Viewer::setPoints(const Eigen::MatrixXd& points){
  this->points = std::make_shared<std::vector<double>>(3 * points.rows());

  #pragma omp parallel for 
  for (int i = 0; i < points.rows(); i++){
    const int row = i * 3;
    this->points->at(row) = points(i, 0);
    this->points->at(row + 1) = points(i, 0);
    this->points->at(row + 2) = points(i, 0);
  }

  return this->points;
}

void Viewer::setFaces(std::shared_ptr<std::vector<uint_m>> faces){
  this->faces = faces;
}

std::shared_ptr<std::vector<uint_m>> Viewer::setFaces(const std::vector<std::vector<uint_m>>& faces){
  this->points = std::make_shared<std::vector<double>>(3 * faces.size());

  #pragma omp parallel for
  for (int i = 0; i < faces.size(); i++){
    const int row = 3*i;
    this->faces->at(row) = faces[i][0];
    this->faces->at(row + 1) = faces[i][1];
    this->faces->at(row + 2) = faces[i][2];
  }

  return this->faces;
}

std::shared_ptr<std::vector<uint_m>> Viewer::setFaces(const Eigen::MatrixXi& faces){
  this->points = std::make_shared<std::vector<double>>(3 * faces.rows());

  #pragma omp parallel for 
  for (int i = 0; i < faces.rows(); i++){
    const int row = i * 3;
    this->faces->at(row) = faces(i, 0);
    this->faces->at(row + 1) = faces(i, 0);
    this->faces->at(row + 2) = faces(i, 0);
  }

  return this->faces;
}

