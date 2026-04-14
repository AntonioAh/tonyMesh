#ifndef MESH_LOADER_HPP
#define MESH_LOADER_HPP

#include <fstream>
#include <string>
#include <Eigen/Dense>
#include <IO/enums.hpp>

class MeshLoader{
public:
  static ResultType loadMesh(const std::string& fileName, Eigen::MatrixXd &V, Eigen::MatrixXi &F);
  // static ResultType loadMesh(const std::string& fileName, Eigen::MatrixXd &V, Eigen::MatrixXi &F, Eigen::MatrixXd &N);

private:
  static ResultType getFile(const std::string& fileName, std::ifstream &file);
  static FileType getExtension(const std::string& fileName); 

  static ResultType loadOFF(std::ifstream &file, Eigen::MatrixXd &V, Eigen::MatrixXi &F); 
  // static ResultType loadOFF(std::ifstream &file, Eigen::MatrixXd &V, Eigen::MatrixXi &F, Eigen::MatrixXd &N); 
};

#endif
