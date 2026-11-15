#include <tonyMesh/connectivity.hpp>
#include <algorithm>
#include <chrono>
#include <tbb/parallel_for.h>

namespace tonyMesh::connectivity {
    std::vector<Index> vertex_vertex(Index vert, const FaceMatrix& faces){
        std::vector<Index> result;

        for (Index f{}; f < faces.cols(); f++) {
            if (faces(0, f) == vert) {
                result.push_back(faces(1, f));
                result.push_back(faces(2, f));
            }
            else if (faces(1, f) == vert) {
                result.push_back(faces(0, f));
                result.push_back(faces(2, f));
            }
            else if (faces(2, f) == vert) {
                result.push_back(faces(0, f));
                result.push_back(faces(1, f));
            }
        }

        std::ranges::sort(result);
        result.erase(std::ranges::unique(result).begin(), result.end());
        return result;
    }

    std::vector<std::vector<Index>> vertex_vertex_lists(size_t numVertices, const FaceMatrix& faces) {
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

        tbb::parallel_for(size_t{},  static_cast<size_t>(numVertices), [&](size_t v) {
            std::ranges::sort(result[v]);
            result[v].erase(std::ranges::unique(result[v]).begin(), result[v].end());
        });
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

    std::vector<std::vector<Index>> vertex_faces_lists(Index numVertices, const FaceMatrix& faces) {
        std::vector<std::vector<Index>> result(static_cast<size_t>(numVertices));
        const Index numFaces{faces.rows()};

        for (Index f{}; f < numFaces; f++) {
            for (int k{}; k < 3; k++)
                result[faces(f, k)].push_back(f);
        }

        return result;
    }
}