#ifndef VIEWER_HPP
#define VIEWER_HPP

#include <types.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <memory>
#include <vector>
#include <Eigen/Dense>

class Viewer{
public:
  static Viewer& instance(){
    static Viewer instance;
    return instance;
  }

  Viewer(const Viewer&) = delete;
  Viewer& operator=(const Viewer&) = delete;

  enum class ResultType{
    OK,
    GenericError,
    GLFW_ERROR,
    GLAD_ERROR
  };

  ResultType loadViewer();
  void loop();

   std::shared_ptr<std::vector<double>> setPoints(const std::vector<std::vector<double>>& points);
  std::shared_ptr<std::vector<double>> setPoints(const Eigen::MatrixXd& points);
  void setPoints(std::shared_ptr<std::vector<double>> points); 

  std::shared_ptr<std::vector<uint_m>> setFaces(const std::vector<std::vector<uint_m>>& points);
  std::shared_ptr<std::vector<uint_m>> setFaces(const Eigen::MatrixXi& points);
  void setFaces(std::shared_ptr<std::vector<uint_m>> points);


private:
  Viewer() = default;
  static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
  GLFWwindow* window; 

  std::shared_ptr<std::vector<double>> points;
  std::shared_ptr<std::vector<uint_m>> faces;

};

#endif // !VIEWER_HPP

