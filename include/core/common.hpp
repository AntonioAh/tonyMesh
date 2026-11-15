#pragma once

#include <Eigen/Core>

namespace tonyMesh {
    typedef float Real;
    typedef Eigen::Index Index;

    typedef Eigen::Matrix<Real, 3, Eigen::Dynamic> VertexMatrix;
    typedef Eigen::Matrix<int, 3, Eigen::Dynamic> FaceMatrix;
    typedef Eigen::Matrix<Real, 3, Eigen::Dynamic> NormalMatrix;
}