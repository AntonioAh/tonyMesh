#pragma once

#include <Eigen/Core>
#include <cmath>

namespace tonyMesh {
typedef float Real;
typedef double DoubleReal;

typedef Eigen::Index Index;
typedef int FaceType;

typedef Eigen::Matrix<Real, Eigen::Dynamic, 3> VertexMatrix;
typedef Eigen::Matrix<FaceType, Eigen::Dynamic, 3> FaceMatrix;
typedef Eigen::Matrix<Real, Eigen::Dynamic, 3> NormalMatrix;

typedef Eigen::Matrix<Real, 1, 3> Point;
typedef Eigen::Matrix<FaceType, 1, 3> Face;
typedef Eigen::Matrix<Real, 1, 3> Normal;

constexpr Real T_PI{Real(M_PI)};
}  // namespace tonyMesh
