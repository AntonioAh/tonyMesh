#ifndef TONYMESH_MESH_HPP
#define TONYMESH_MESH_HPP

#include <definitions.hpp>

namespace tonyMesh {

    template <typename Scalar>
    class Mesh {
    public:
        Mesh(PointMatrix<Scalar>& vertices, FaceMatrix& faces) = delete;
        Mesh(PointMatrix<Scalar>&& vertices, FaceMatrix&& faces);


        Mesh() = delete;
        Mesh(const Mesh& other);
        Mesh(Mesh&& other);
        Mesh operator=(const Mesh& other);
        Mesh operator=(Mesh&& other);

        const PointMatrix<Scalar>& getVertices() const;
        const FaceMatrix& getFaces() const;

        size_t degree(size_t vertex) const;
        VertexVector getNeighbors(size_t vertex) const;
        FaceVector getIncidentFaces(size_t vertex) const;

    private:
        PointMatrix<Scalar> _vertices;
        FaceMatrix _faces;
    };
}

#endif //TONYMESH_MESH_HPP
