#include <iostream>
#include <tonyMesh/app/application.hpp>

int main(int argc, char** argv) {
    std::cout << "Hello World\n";
    tonyMesh::app::Application app;
    app.start();
    app.run();

    return 0;
}
