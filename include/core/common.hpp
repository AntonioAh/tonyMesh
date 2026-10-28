#pragma once

#include <Eigen/Core>

namespace tonyMesh {
    typedef double Real;
    typedef int FaceType;
    typedef Eigen::Index Index;
    typedef Eigen::Index VertexIndex;
    typedef Eigen::Index FaceIndex;

    typedef Eigen::Matrix<Real, Eigen::Dynamic, 3> VertexMatrix;
    typedef Eigen::Matrix<FaceType, Eigen::Dynamic, 3> FaceMatrix;

    typedef Eigen::RowVector<Real, Eigen::Dynamic> Vertex;
    typedef Eigen::Matrix<FaceType, 1, 3> Face;
}