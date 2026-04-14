#include "IO/enums.hpp"
#include <IO/meshLoader.hpp>
#include <algorithm>
#include <fstream>
#include <string>
#include <string_view>
#include <cctype>

ResultType MeshLoader::loadMesh(const std::string &fileName, Eigen::MatrixXd &V, Eigen::MatrixXi &F){
  std::ifstream file;
  auto resultFile = getFile(fileName, file);
  if (resultFile != ResultType::OK)
    return resultFile;

  std::string copyName{fileName};
  auto ext = getExtension(copyName);

  if (ext == FileType::OFF){
    return MeshLoader::loadOFF(file, V, F);
  }

  return ResultType::ExtensionNotFound;
}


ResultType MeshLoader::getFile(const std::string& fileName, std::ifstream &file){
  constexpr std::array<std::string_view, 2> foldersToCheck = {"../data/", "../tests/data/"};

  for (const auto& folder : foldersToCheck){
    file.open(std::string(folder) + fileName);

    if (file.is_open())
      break;
  }


  if (!file.is_open())
    return ResultType::FileNotFound;
  file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

  return ResultType::OK;
}

FileType MeshLoader::getExtension(const std::string& fileName){
  std::string copyName(fileName);
  std::transform(copyName.begin(), copyName.end(), copyName.begin(), 
      [](auto c){
        return static_cast<char>(std::tolower(c));
      });
  std::string_view ext = std::string_view(copyName);
  ext = ext.substr(ext.rfind('.'));

  if (ext == "off")
    return FileType::OFF;
  if (ext == "obj")
    return FileType::OBJ;

  return FileType::NotFound;
}

ResultType MeshLoader::loadOFF(std::ifstream &file, Eigen::MatrixXd &V, Eigen::MatrixXi &F){
  std::string str;
  file >> str;
  if (str != "OFF")
    return ResultType::WrongStructure;

  unsigned int verts, faces, edges;
  try{
    file >> verts >> faces >> edges;
  }
  catch (const std::ios_base::failure& e ){
    return ResultType::WrongStructure;
  }

  V.resize(verts, 3);
  F.resize(faces, 3);

  try{
    double a, b, c;
    unsigned int e, f, g;
    for (unsigned int i = 0; i < verts; i++) 
      file >> V(i, 0) >> V(i, 1) >> V(i, 2);

    for (unsigned int i = 0; i < faces; i++)
      file >> F(i, 0) >> F(i, 1) >> F(i, 2);
  }
  catch (const std::ios_base::failure& e){
    return ResultType::WrongStructure;
  }

  return ResultType::GenericError;
}
