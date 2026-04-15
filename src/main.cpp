#include "IO/enums.hpp"
#include "IO/meshLoader.hpp"
#include "viewer/viewer.hpp"
#include <iostream>

int main(int argc, char *argv[]){
  // if (argc == 1){
  //   std::cout << "Specify as argument what mesh to open\n";
  //   return 1;
  // }
  //
  // std::string fileName(argv[1]);
  //
  // Eigen::MatrixXd V;
  // Eigen::MatrixXi F;
  //
  // auto result = MeshLoader::loadMesh(fileName, V, F);
  // IO_printError(result);
  
  Viewer::instance().loadViewer();
  Viewer::instance().loop();
  return 0;
}
