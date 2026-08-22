#ifndef TONYMESH_ACCELERATIONSTRUCTURES_HPP
#define TONYMESH_ACCELERATIONSTRUCTURES_HPP

#include <definitions.hpp>

namespace tonyMesh {
    template <typename Scalar>
    class UniformGrid {
    public:
        UniformGrid(const PointMatrix<Scalar>& points);

        size_t getNearest(const Point<Scalar>& query);

        VertexVector getNeighbors(Eigen::Index index, size_t number);
        VertexVector getNeighbors(const Point<Scalar>& query, size_t number);

    private:
        const PointMatrix<Scalar>& _points;
    };
}

#endif //TONYMESH_ACCELERATIONSTRUCTURES_HPP
