#include <spdlog/spdlog.h>

#include <cctype>
#include <fstream>
#include <ios>
#include <limits>
#include <locale>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tonyMesh/IO.hpp>
#include <tonyMesh/common.hpp>
#include <tonyMesh/mesh.hpp>
#include <unordered_map>

namespace tonyMesh::IO {
namespace /* common to read and write*/ {
constexpr std::array<std::string, 3> dirs = {"./meshes/", "../meshes/",
                                             "../../meshes/"};

std::optional<std::fstream> getFile(const std::string& filePath,
                                    std::ios::openmode mode) {
    for (const auto& poss : dirs) {
        std::fstream file(poss + filePath, mode);
        if (file) return file;
    }

    return std::nullopt;
}

enum class FileType { OBJ, OFF };
const std::unordered_map<std::string, FileType> typeMap{{"obj", FileType::OBJ},
                                                        {"off", FileType::OFF}};

std::optional<FileType> getFileType(const std::string& filePath) {
    auto lastPoint{filePath.find_last_of('.')};
    if (lastPoint == std::string::npos) return std::nullopt;

    std::string ext{filePath.substr(lastPoint + 1)};
    auto& f = std::use_facet<std::ctype<char>>(std::locale());
    f.tolower(&ext[0], &ext[0] + ext.size());

    auto found{typeMap.find(ext)};
    if (found != typeMap.end()) return found->second;
    return std::nullopt;
}
}  // namespace

namespace /* read */ {
Mesh readObj(std::fstream& file, const std::string& fileName) {
    std::vector<Point> vertices;
    std::vector<Face> faces;
    std::string line;

    auto nextLine = [&]() -> bool {
        while (std::getline(file, line)) {
            if (!line.empty() && line.back() == '\r') line.pop_back();
            if (line.empty() || line[0] == '#') continue;
            return true;
        }
        return false;
    };

    Face constant{1, 1, 1};
    std::string c;
    std::istringstream ss;
    while (nextLine()) {
        if (line[0] == 'v') {
            Point point;
            ss = std::istringstream(line);
            ss >> c >> point(0) >> point(1) >> point(2);
            vertices.push_back(point);
        } else if (line[0] == 'f') {
            Face face;
            ss = std::istringstream(line);
            ss >> c >> face(0) >> face(1) >> face(2);
            face -= constant;
            faces.push_back(face);
        }
    }

    VertexMatrix vM(vertices.size(), 3);
    for (size_t i{}; i < vertices.size(); i++) vM.row(i) = vertices[i];

    FaceMatrix fM(faces.size(), 3);
    for (size_t i{}; i < faces.size(); i++) fM.row(i) = faces[i];

    return {vM, fM};
}

Mesh readOff(std::fstream& file, const std::string& fileName) {
    std::string line;

    // Reads the next non-empty, non-comment line into `line`. Returns false at
    // end of file.
    auto nextLine = [&]() -> bool {
        while (std::getline(file, line)) {
            if (!line.empty() && line.back() == '\r') line.pop_back();
            if (line.empty() || line[0] == '#') continue;
            return true;
        }
        return false;
    };

    auto fail = [&](const std::string& what) -> void {
        // return std::runtime_error(fileName + ": " + what);
        return spdlog::error("{} : {}", fileName, what);
    };

    if (!nextLine() || !line.starts_with("OFF")) fail("missing OFF header");

    if (!nextLine()) fail("missing counts line");
    unsigned numVertices{}, numFaces{};
    if (!(std::istringstream(line) >> numVertices >> numFaces))
        fail("invalid counts line");

    VertexMatrix vertices(numVertices, 3);
    for (unsigned i{}; i < numVertices; ++i) {
        if (!nextLine())
            fail("expected " + std::to_string(numVertices) + " vertices, got " +
                 std::to_string(i));
        std::istringstream ss(line);
        if (!(ss >> vertices(i, 0) >> vertices(i, 1) >> vertices(i, 2)))
            fail("invalid vertex at index " + std::to_string(i));
    }

    FaceMatrix faces(numFaces, 3);
    for (unsigned i{}; i < numFaces; ++i) {
        if (!nextLine())
            fail("expected " + std::to_string(numFaces) + " faces, got " +
                 std::to_string(i));
        std::istringstream ss(line);
        unsigned n{};
        if (!(ss >> n >> faces(i, 0) >> faces(i, 1) >> faces(i, 2)) || n != 3)
            fail("invalid or non-triangle face at index " + std::to_string(i));
    }

    return {vertices, faces};
}
}  // namespace

namespace /* write */ {
void writeObj(std::fstream& file, const Mesh& mesh) {
    file << std::setprecision(std::numeric_limits<Real>::max_digits10);
    const auto& vertices(mesh.vertices());
    const auto& faces(mesh.faces());

    file << "# " << vertices.rows() << " " << faces.rows() << " \n";
    for (Index i{}; i < vertices.rows(); i++)
        file << "v " << vertices(i, 0) << " " << vertices(i, 1) << " "
             << vertices(i, 2) << "\n ";

    for (Index i{}; i < faces.rows(); i++)
        file << "f v" << faces(i, 0) << " v" << faces(i, 1) << " v"
             << faces(i, 2) << " \n";
}

void writeOff(std::fstream& file, const Mesh& mesh) {
    file << std::setprecision(std::numeric_limits<Real>::max_digits10);

    file << "OFF\n";
    const auto& vertices{mesh.vertices()};
    const auto& faces{mesh.faces()};

    file << vertices.rows() << " " << faces.rows() << " 0\n";
    for (Index i{}; i < vertices.rows(); i++)
        file << vertices(i, 0) << " " << vertices(i, 1) << " " << vertices(i, 2)
             << " \n";

    // file << "\n";
    for (Index i{}; i < faces.rows(); i++)
        file << 3 << " " << faces(i, 0) << " " << faces(i, 1) << " "
             << faces(i, 2) << " \n";
    // file << '\n';
}
}  // namespace

Mesh readMesh(const std::string& filePath) {
    auto optFile{getFile(filePath, std::ios::in)};
    if (!optFile) {
        spdlog::error("cannot locate file {}", filePath);
        return {};
    }

    auto optFileType{getFileType(filePath)};
    if (!optFileType) {
        spdlog::error("cannot recognize extension of {}", filePath);
        return {};
    }

    switch (*optFileType) {
        case FileType::OBJ:
            return readObj(*optFile, filePath);
            break;
        case FileType::OFF:
            return readOff(*optFile, filePath);
            break;
    }
    spdlog::info("read mesh from {}", filePath);
    return {};
}

void writeMesh(const Mesh& mesh, const std::string& filePath) {
    auto optFile{getFile(filePath, std::ios::out)};
    auto optFileType{getFileType(filePath)};

    if (!optFileType) {
        spdlog::error("cannot recognize extension of {}", filePath);
        return;
    }

    switch (*optFileType) {
        case FileType::OBJ:
            writeObj(*optFile, mesh);
            break;
        case FileType::OFF:
            writeOff(*optFile, mesh);
            break;
    }

    spdlog::info("written mesh in {}", filePath);
}
}  // namespace tonyMesh::IO
