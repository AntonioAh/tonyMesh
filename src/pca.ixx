module;
#include <Eigen/Dense>

export module pca;

import definitions;
import mesh;

export namespace tonyMesh {

    template <typename Scalar>
    class PCA {
    public:
        PCA(const PointMatrix<Scalar>& points, bool selfAdjacent=false);
        bool isValid() const;

        const Eigen::Vector<Scalar, Eigen::Dynamic>& getValues() const;
        Scalar getValue(size_t index) const;

        const Mesh<Scalar>::PointMatrix& getVectors() const;
        const Eigen::Vector<Scalar, Eigen::Dynamic> getVector(size_t idx) const;

    private:
        bool _isValid;
    };
}

namespace tonyMesh {
    template<typename Scalar>
    PCA<Scalar>::PCA(const PointMatrix<Scalar>& points, bool selfAdjacent) {


    }
}
