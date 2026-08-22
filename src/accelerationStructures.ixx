module;
#include <Eigen/Dense>

export module accelerationStructures;

import definitions;
import mesh;

export namespace tonyMesh {



    template <typename Scalar>
    class UniformGrid {
    private:
    public:
        UniformGrid(const PointMatrix<Scalar>& _points);

        Mesh<Scalar>::Point getNearest(const Point<Scalar>& query);
        Mesh<Scalar>::PointMatrix getNeighbors(const  Point<Scalar>& query, size_t neighs);
    };
}