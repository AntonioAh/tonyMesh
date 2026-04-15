#ifndef MESH_LOADER_HPP
#define MESH_LOADER_HPP

#include <fstream>
#include <string>
#include <Eigen/Dense>
#include <IO/enums.hpp>

class MeshLoader{
public:
  static IO_ResultType loadMesh(const std::string& fileName, Eigen::MatrixXd &V, Eigen::MatrixXi &F);
  // static ResultType loadMesh(const std::string& fileName, Eigen::MatrixXd &V, Eigen::MatrixXi &F, Eigen::MatrixXd &N);

private:
  static IO_ResultType getFile(const std::string& fileName, std::ifstream &file);
  static IO_FileType getExtension(const std::string& fileName); 

  static IO_ResultType loadOFF(std::ifstream &file, Eigen::MatrixXd &V, Eigen::MatrixXi &F); 
  // static ResultType loadOFF(std::ifstream &file, Eigen::MatrixXd &V, Eigen::MatrixXi &F, Eigen::MatrixXd &N); 
};

#endif
