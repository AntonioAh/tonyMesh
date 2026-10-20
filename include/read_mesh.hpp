#ifndef TONYMESH_READ_MESH_HPP
#define TONYMESH_READ_MESH_HPP
#include <expected>

#include "mesh.hpp"


namespace tonyMesh {
    enum class IO_ERROR {
        GENERIC
    };
    enum class MESH_FILE_TYPE {
        OFF, STD, GENERIC
    };

    std::expected<Mesh, IO_ERROR> readMesh(std::string&& fileName, MESH_FILE_TYPE fileType=MESH_FILE_TYPE::GENERIC);
    std::expected<Mesh, IO_ERROR> readMesh(std::string& fileName, MESH_FILE_TYPE fileType=MESH_FILE_TYPE::GENERIC);
}

#endif //TONYMESH_READ_MESH_HPP
