#pragma once

#include <Eigen/Core>

namespace tonyMesh {
    typedef float Real;
    typedef Eigen::Index Index;
    typedef int FaceType;

    typedef Eigen::Matrix<Real, Eigen::Dynamic, 3> VertexMatrix;
    typedef Eigen::Matrix<FaceType, Eigen::Dynamic, 3> FaceMatrix;
    typedef Eigen::Matrix<Real, Eigen::Dynamic, 3> NormalMatrix;

    typedef Eigen::Matrix<Real, 1, 3> Normal;
}