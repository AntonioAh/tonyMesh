#pragma once

#include <core/common.hpp>
#include <core/mesh.hpp>

namespace tonyMesh::smoothing {

    //https://www.graphics.rwth-aachen.de/media/papers/sqrt31.pdf
    Mesh sqrt31(const Mesh& mesh);
}