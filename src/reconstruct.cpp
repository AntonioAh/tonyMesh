#include <reconstruct.hpp>
#include <accelerationStructures.hpp>
#include <normals.hpp>

using namespace tonyMesh;
template <typename Scalar>
static bool buildNormals(const PointMatrix<Scalar>& points, ReconstructionInfo<Scalar>& info) {
    if (info.normals)
       return true;

    auto result = computeNormals(points, info.grid);
    if (! result.has_value())
        return false;

    info.normals = std::move(result.value());
    return true;
}


template <typename Scalar>
std::expected<Mesh<Scalar>, ReconstructionError> tonyMesh::reconstruct(const PointMatrix<Scalar>& points, ReconstructionInfo<Scalar>& info) {
    if (! buildNormals(points, info))
        return  std::unexpected(ReconstructionError::Normals);
}