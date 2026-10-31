#pragma once

#include <core/common.hpp>

namespace tonyMesh {
    struct Edge {
        VertexIndex first, second;

        explicit Edge(VertexIndex first, VertexIndex second) :
            first{std::min(first, second)},
            second{std::max(first, second)} {
        }

        explicit Edge(VertexIndex first, VertexIndex second, bool sorted) {
            if (!sorted) {
                this->first = first;
                this->second = second;
            }
            else {
                this->first = std::min(first, second);
                this->second = std::max(first, second);
            }
        }

        bool operator==(const Edge &other) const {
            return first==other.first && second==other.second;
        }

        bool operator<(const Edge& other) const {
            if (first != other.first)
                return first < other.first;
            return second < other.second;
        }

        struct EdgeHash {
            std::size_t operator()(const Edge& e) const {
                auto h1{std::hash<VertexIndex>{}(e.first)};
                auto h2{std::hash<VertexIndex>{}(e.second)};
                return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
            }
        };
    };
}