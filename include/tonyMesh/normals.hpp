#pragma once
#include <tonyMesh/common.hpp>
#include <Eigen/Core>
#include <memory>
#include <tonyMesh/accelerationStructures/accelerationStructure.hpp>

namespace tonyMesh::normals {
    NormalMatrix faceNormals(const VertexMatrix& vertices, const FaceMatrix& faces);

    enum class Normalization {
        NO_NORMALIZATION, AREA, ANGLE
    };
    NormalMatrix vertexNormals(const VertexMatrix& vertices, const FaceMatrix& faces, Normalization type =Normalization::ANGLE);

    NormalMatrix pointCloud(const VertexMatrix& vertices, size_t numNeighbors, std::shared_ptr<AccelerationStructure> acc);
    void orientNormals(NormalMatrix& normals, const VertexMatrix& vertices, size_t numNeighbors, std::shared_ptr<AccelerationStructure> acc);
}