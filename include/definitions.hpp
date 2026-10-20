#ifndef TONYMESH_DEFINITIONS_HPP
#define TONYMESH_DEFINITIONS_HPP

#include <Eigen/Core>

namespace tonyMesh {

typedef float Scalar;
typedef double DoubleScalar;

typedef unsigned FaceType;
typedef Eigen::Index Index;
struct Edge;

typedef Eigen::Matrix<Scalar, Eigen::Dynamic, 3> VertexMatrix;
typedef Eigen::Matrix<FaceType, Eigen::Dynamic, 3> FaceMatrix;

typedef Eigen::RowVector<Scalar, 3> Vertex;
typedef Eigen::RowVector<FaceType, 3> Face;

typedef Eigen::Matrix<FaceType, Eigen::Dynamic, 3> FaceAdjacencyMatrix;
}  // namespace tonyMesh

#endif  // TONYMESH_DEFINITIONS_HPP
