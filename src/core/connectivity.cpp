#include <tbb/parallel_for.h>

#include <algorithm>
#include <chrono>
#include <tonyMesh/connectivity.hpp>
#include <tonyMesh/dataStructures.hpp>
#include <iterator>
#include <utility>

namespace tonyMesh::connectivity {
std::vector<Index> vertex_vertex(Index vert, const FaceMatrix& faces) {
    std::vector<Index> result;

    for (Index f{}; f < faces.rows(); f++) {
        if (faces(f, 0) == vert) {
            result.push_back(faces(f, 1));
            result.push_back(faces(f, 2));
        } else if (faces(f, 1) == vert) {
            result.push_back(faces(f, 0));
            result.push_back(faces(f, 2));
        } else if (faces(f, 2) == vert) {
            result.push_back(faces(f, 0));
            result.push_back(faces(f, 1));
        }
    }

    std::ranges::sort(result);
    result.erase(std::ranges::unique(result).begin(), result.end());
    return result;
}

std::vector<std::vector<Index>> vertex_vertex_lists(size_t numVertices,
                                                    const FaceMatrix& faces) {
    std::vector<std::vector<Index>> result(numVertices);
    const Index numFaces{faces.rows()};

    for (Index f{}; f < numFaces; f++) {
        result[faces(f, 0)].push_back(faces(f, 1));
        result[faces(f, 0)].push_back(faces(f, 2));

        result[faces(f, 1)].push_back(faces(f, 0));
        result[faces(f, 1)].push_back(faces(f, 2));

        result[faces(f, 2)].push_back(faces(f, 0));
        result[faces(f, 2)].push_back(faces(f, 1));
    }

    tbb::parallel_for(
        size_t{}, static_cast<size_t>(numVertices), [&](size_t v) {
            std::ranges::sort(result[v]);
            result[v].erase(std::ranges::unique(result[v]).begin(),
                            result[v].end());
        });
    return result;
}

int degree(Index v, const FaceMatrix& faces) {
    return static_cast<int>(vertex_vertex(v, faces).size());
}

std::vector<int> degrees(size_t numVertices, const FaceMatrix& faces) {
    std::vector<int> result(numVertices);
    const auto vertLists{vertex_vertex_lists(numVertices, faces)};

    for (size_t i{}; i < numVertices; i++)
        result[i] = static_cast<int>(vertLists[i].size());

    return result;
}

std::vector<Index> vertex_face(Index vert, const FaceMatrix& faces) {
    std::vector<Index> result;
    const Index numFaces{faces.rows()};

    for (Index f{}; f < numFaces; f++) {
        if (faces(f, 0) == vert || faces(f, 1) == vert || (faces(f, 2)) == vert)
            result.push_back(f);
    }
    return result;
}

std::vector<std::vector<Index>> vertex_faces_lists(Index numVertices,
                                                   const FaceMatrix& faces) {
    std::vector<std::vector<Index>> result(static_cast<size_t>(numVertices));
    const Index numFaces{faces.rows()};

    for (Index f{}; f < numFaces; f++) {
        for (int k{}; k < 3; k++) result[faces(f, k)].push_back(f);
    }

    return result;
}

std::vector<Edge> edges(const FaceMatrix& faces) {
    std::vector<Edge> result;
    const auto numFaces{faces.rows()};
    result.reserve(static_cast<size_t>(3 * numFaces));

    for (Index f{}; f < numFaces; f++) {
        result.emplace_back(faces(f, 0), faces(f, 1));
        result.emplace_back(faces(f, 1), faces(f, 2));
        result.emplace_back(faces(f, 2), faces(f, 0));
    }
    std::ranges::sort(result);
    result.erase(std::ranges::unique(result).begin(), result.end());
    return result;
}

std::vector<bool> isEdgeManifold(const FaceMatrix& faces) {
    const auto allEdges{edges(faces)};
    const auto numEdges{allEdges.size()};

    std::vector<int> counts(numEdges);
    const auto numFaces{faces.rows()};

    for (Index f{}; f < numFaces; f++) {
        for (int k{}; k < 3; k++) {
            Edge edge{faces(f, k), faces(f, (k+1)%3)};
            auto found{std::ranges::lower_bound(allEdges, edge)};
            counts[std::distance(allEdges.begin(), found)] += 1;
        }
    }

    std::vector<bool> result(allEdges.size());
    for (size_t i{}; i < numEdges; i++)
        result[i] = (counts[i] <= 2);
    return result;
}
std::vector<bool> isVertexManifold(size_t numVertices,
                                   const FaceMatrix& faces) {
    // Link of v: for every face (v, a, b), the opposite edge (a, b).
    std::vector<std::vector<std::pair<int, int>>> links(numVertices);
    for (Index f{}; f < faces.rows(); f++) {
        for (int k{}; k < 3; k++) {
            const int v{faces(f, k)};
            links[static_cast<size_t>(v)].emplace_back(faces(f, (k + 1) % 3),
                                                       faces(f, (k + 2) % 3));
        }
    }

    // A vertex is manifold iff its link is one connected path or one cycle,
    // i.e. it is connected and every link vertex has degree <= 2.
    std::vector<bool> ok(numVertices, true);
    for (size_t v{}; v < numVertices; v++) {
        const auto& link{links[v]};
        if (link.empty()) continue;  // isolated vertex: nothing to violate

        std::vector<int> ids;  // sorted unique link vertices -> local index
        ids.reserve(2 * link.size());
        for (const auto& [a, b] : link) {
            ids.push_back(a);
            ids.push_back(b);
        }
        std::ranges::sort(ids);
        ids.erase(std::ranges::unique(ids).begin(), ids.end());

        const auto local{[&](int id) {
            return static_cast<size_t>(std::ranges::lower_bound(ids, id) -
                                       ids.begin());
        }};

        std::vector<int> deg(ids.size());
        dataStructures::UnionFind sets(ids.size());

        size_t components{ids.size()};
        for (const auto& [a, b] : link) {
            const size_t la{local(a)}, lb{local(b)};
            if (++deg[la] > 2 || ++deg[lb] > 2) {
                ok[v] = false;
                break;
            }
            if (sets.unite(la, lb)) --components;
        }
        if (ok[v] && components != 1) ok[v] = false;
    }

    return ok;
}

bool isManifold(size_t numVertices, const FaceMatrix& faces) {
    const auto isVert{isVertexManifold(numVertices, faces)};
    const auto isEdges{isEdgeManifold(faces)};

    return std::ranges::all_of(isVert, [](const bool& p) { return p; }) &&
           std::ranges::all_of(isEdges, [](const bool& p) { return p; });
}
}  // namespace tonyMesh::connectivity
