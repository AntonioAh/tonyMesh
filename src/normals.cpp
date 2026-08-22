#include <normals.hpp>

#include <tuple>
#include <pca.hpp>
#include <Eigen/Core>
#include <dataStructures.hpp>
#include <queue>

using namespace tonyMesh;

//https://dl.acm.org/doi/10.1145/142920.134011
template <typename Scalar>
static bool makeOrient(PointMatrix<Scalar>& normals, UniformGrid<Scalar>& grid, size_t k, size_t maxZ) {
    std::vector<std::tuple<size_t, size_t, double>> edges;


    for (size_t i{}; i < normals.rows(); i++) {
        auto neighbors{grid.getNeighbors(i, k )};
        for (size_t neigh : neighbors) {
            edges.push_back({std::min(i, neigh), std::max(i, neigh),
                1.0 - std::abs(normals.row(i).dot(normals.row(neigh)))});
        }
    }
    edges.erase(std::unique(edges.begin(), edges.end()), edges.end());
    std::ranges::sort(edges, [](const auto& a, const auto &b){
        return std::get<2>(a) > std::get<2>(b);
    });

    std::vector<std::vector<size_t>> mst(normals.rows());

    UnionFind uf(normals.rows());
    size_t addedEdges{};
    while (addedEdges < normals.rows() - 1) {
        auto first{std::get<0>(edges.back())}, second{std::get<1>(edges.back())};

        if (uf.unite(first, second)) {
            mst[first].push_back(second);
            mst[second].push_back(first);
            addedEdges++;
        }
        edges.pop_back();
    }

    std::queue<std::pair<size_t, size_t>> coda;
    std::vector<bool> visited(normals.rows(), false);
    visited[maxZ] = true;

    for (auto child : mst[maxZ])
        coda.push({child, maxZ});

    constexpr Scalar minusOne{static_cast<Scalar>(1.0)};
    while (! coda.empty()) {
        auto node{coda.front()};
        coda.pop();

        if (visited[node.first])
            continue;
        visited[node.first] = true;

        if (normals.row(node.second).dot(normals.row(node.first)) < 0)
            normals.row(node.first) *= minusOne;

        for (auto child : mst[node.first]) {
            if (!visited[child])
                coda.push({child, node.first});
        }
    }

    return true;
}

//https://cgl.ethz.ch/Downloads/Publications/Papers/2002/p_Pau02.pdf
template <typename Scalar>
std::expected<PointMatrix<Scalar>, NormalsError> tonyMesh::computeNormals(const PointMatrix<Scalar>& points,
    std::optional<UniformGrid<Scalar>>& grid,
    size_t neighNumber,
    bool orient,
    size_t k)
{
    if (! grid)
        grid = UniformGrid<Scalar>(points);

    PointMatrix<Scalar> normals(points.rows(), 3);

    for (Eigen::Index i{}; i < points.rows(); i++) {
        auto neighbors(grid->getNeighbors(points.row(i), neighNumber));
        const PCA pca(points(neighbors, Eigen::placeholders::all));

        if (! pca.isValid())
            return std::unexpected(NormalsError::Generic);

        normals.row(i) = pca.getVector(0).transpose();
    }

    if (orient) {
        Eigen::Index idxZ;
        Scalar maxZ{points.col(2).maxCoeff(&idxZ)};
        if (normals(idxZ, 2) < 0)
            normals.row(idxZ) *= -1;

        if (!makeOrient(normals, grid.value(), k, maxZ))
            return std::unexpected(NormalsError::Orientation);
    }

    return normals;
}
