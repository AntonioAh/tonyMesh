#include <mesh.hpp>
#include <unordered_set>
#include <vector>

using namespace tonyMesh;

template<typename Scalar>
Mesh<Scalar>::Mesh(PointMatrix<Scalar> &&vertices, FaceMatrix &&faces)
    : _vertices(std::move(vertices)), _faces(std::move(faces))
{

}

template<typename Scalar>
Mesh<Scalar>::Mesh(const Mesh &other)
    : _vertices(other._vertices), _faces(other._faces)
{
}

template<typename Scalar>
Mesh<Scalar>::Mesh(Mesh &&other)
    : _vertices(std::move(other._vertices)), _faces(std::move(other._faces))
{
    other._vertices.resize(0, 3);
    other._faces.resize(0, 3);
}

template<typename Scalar>
Mesh<Scalar> Mesh<Scalar>::operator=(const Mesh &other) {
    if (this == &other)
        return *this;

    _vertices = other._vertices;
    _faces = other._faces;
}

template<typename Scalar>
Mesh<Scalar> Mesh<Scalar>::operator=(Mesh &&other) {
    if (this == &other)
        return *this;

    _vertices = std::move(other._vertices);
    _vertices = std::move(other._faces);

    other._vertices.resize(0, 3);
    other._faces.resize(0, 3);
}

template<typename Scalar>
const PointMatrix<Scalar> & Mesh<Scalar>::getVertices() const {
    return _vertices;
}

template<typename Scalar>
const FaceMatrix & Mesh<Scalar>::getFaces() const {
    return _faces;
}

template<typename Scalar>
size_t Mesh<Scalar>::degree(size_t vertex) const {
    std::unordered_set<size_t> visited;

    for (Eigen::Index i{}; i < _faces.rows(); ++i) {
        if (_faces(i, 0) == vertex) {
            visited.insert(_faces(i, 1));
            visited.insert(_faces(i, 2));
        }
        else if (_faces(i, 1) == vertex) {
            visited.insert(_faces(i, 0));
            visited.insert(_faces(i, 2));
        } else if (_faces(i, 2) == vertex) {
            visited.insert(_faces(i, 0));
            visited.insert(_faces(i, 1));
        }
    }

    return visited.size();
}

template<typename Scalar>
VertexVector Mesh<Scalar>::getNeighbors(size_t vertex) const {
    std::unordered_set<size_t> visited;

    for (Eigen::Index i{}; i < _faces.rows(); ++i) {
        if (_faces(i, 0) == vertex) {
            visited.insert(_faces(i, 1));
            visited.insert(_faces(i, 2));
        }
        else if (_faces(i, 1) == vertex) {
            visited.insert(_faces(i, 0));
            visited.insert(_faces(i, 2));
        } else if (_faces(i, 2) == vertex) {
            visited.insert(_faces(i, 0));
            visited.insert(_faces(i, 1));
        }
    }

    VertexVector result(visited.size());
    Eigen::Index count{};
    for (auto& neighbor : visited)
        result(count++) = neighbor;
    return result;
}

template<typename Scalar>
FaceVector Mesh<Scalar>::getIncidentFaces(size_t vertex) const {
    std::vector<size_t> faces;

    Eigen::Array<bool, Eigen::Dynamic, 1> mask = (_faces.array() == vertex).rowwise().any();

    for (Eigen::Index i{}; i < mask.size(); i++)
        if (mask(i))
            faces.push_back(static_cast<size_t>(i));

    return FaceVector{std::move(faces)};
}
