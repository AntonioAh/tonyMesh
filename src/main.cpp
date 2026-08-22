#include <iostream>

import mesh;
import reconstruction;

using Scalar = float;

int main() {
    tonyMesh::ReconstructionInfo<Scalar> info{tonyMesh::ReconstructionMethod::MLS, {100, 100, 100},
    std::nullopt};
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
