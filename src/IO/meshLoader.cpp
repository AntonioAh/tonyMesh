#include "IO/enums.hpp"
#include <IO/meshLoader.hpp>
#include <algorithm>
#include <fstream>
#include <string>
#include <string_view>
#include <cctype>

IO_ResultType MeshLoader::loadMesh(const std::string &fileName, Eigen::MatrixXd &V, Eigen::MatrixXi &F){
  std::ifstream file;
  auto resultFile = getFile(fileName, file);
  if (resultFile != IO_ResultType::OK)
    return resultFile;

  std::string copyName{fileName};
  auto ext = getExtension(copyName);

  if (ext == IO_FileType::OFF){
    return MeshLoader::loadOFF(file, V, F);
  }

  return IO_ResultType::ExtensionNotFound;
}


IO_ResultType MeshLoader::getFile(const std::string& fileName, std::ifstream &file){
  constexpr std::array<std::string_view, 4> foldersToCheck = {"data/", "tests/data/" "../data/", "../tests/data/"};

  for (const auto& folder : foldersToCheck){
    file.open(std::string(folder) + fileName);

    if (file.is_open())
      break;
  }


  if (!file.is_open())
    return IO_ResultType::FileNotFound;
  file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

  return IO_ResultType::OK;
}

IO_FileType MeshLoader::getExtension(const std::string& fileName){
  std::string copyName(fileName);
  std::transform(copyName.begin(), copyName.end(), copyName.begin(), 
      [](auto c){
        return static_cast<char>(std::tolower(c));
      });
  std::string_view ext = std::string_view(copyName);
  ext = ext.substr(ext.rfind('.') + 1);

  if (ext == "off")
    return IO_FileType::OFF;
  if (ext == "obj")
    return IO_FileType::OBJ;

  return IO_FileType::NotFound;
}

IO_ResultType MeshLoader::loadOFF(std::ifstream &file, Eigen::MatrixXd &V, Eigen::MatrixXi &F){
  std::string str;
  file >> str;
  if (str != "OFF")
    return IO_ResultType::WrongStructure;

  unsigned int verts, faces, edges;
  try{
    file >> verts >> faces >> edges;
  }
  catch (const std::ios_base::failure& e ){
    return IO_ResultType::WrongStructure;
  }

  V.resize(verts, 3);
  F.resize(faces, 3);

  try{
    unsigned int three;

    for (unsigned int i = 0; i < verts; i++) 
      file >> V(i, 0) >> V(i, 1) >> V(i, 2);

    for (unsigned int i = 0; i < faces; i++){
      file >> three >> F(i, 0) >> F(i, 1) >> F(i, 2);
      if (three != 3)
        return IO_ResultType::WrongStructure;
    }

  }
  catch (const std::ios_base::failure& e){
    return IO_ResultType::WrongStructure;
  }

  return IO_ResultType::OK;
}
