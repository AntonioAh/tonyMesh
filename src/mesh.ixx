module;

#include <Eigen/Dense>
#include <type_traits>

export module mesh;
import definitions;

export namespace tonyMesh {


    template <typename Scalar>
    class Mesh {
    public:
        typedef Eigen::RowVector<Scalar, 3> Point;

        Mesh() = delete;
        Mesh(PointMatrix<Scalar> & points, FaceMatrix& faces) = delete;
        Mesh(PointMatrix<Scalar>&& points, FaceMatrix&& faces);
        Mesh(const Mesh& otherMesh);
        Mesh(Mesh&& otherMesh);

        const PointMatrix<Scalar>& getVertices() const;
        const FaceMatrix& getFaces() const;

    private:
        PointMatrix<Scalar> _vertices;
        FaceMatrix _faces;

    };

}

namespace tonyMesh {
    template <typename Scalar>
    Mesh<Scalar>::Mesh(PointMatrix<Scalar>&& points, FaceMatrix&& faces)
        : _vertices(std::move(points)), _faces(std::move(faces))
    {
    }

    template<typename Scalar>
    Mesh<Scalar>::Mesh(const Mesh &otherMesh)
        : _vertices(otherMesh.getVertices()), _faces(otherMesh.getFaces())
    {
    }

    template<typename Scalar>
    Mesh<Scalar>::Mesh(Mesh &&otherMesh)
        : _vertices(std::move(otherMesh._vertices)), _faces(std::move(otherMesh._faces))
    {
    }

    template<typename Scalar>
    const PointMatrix<Scalar>& Mesh<Scalar>::getVertices() const{
        return _vertices;
    }

    template<typename Scalar>
    const FaceMatrix& Mesh<Scalar>::getFaces() const {
        return _faces;
    }
}

export template class tonyMesh::Mesh<float>;
export template class tonyMesh::Mesh<double>;
export template class tonyMesh::Mesh<long double>;

