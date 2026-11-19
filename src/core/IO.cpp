#include <spdlog/spdlog.h>

#include <array>
#include <cctype>
#include <fstream>
#include <iomanip>
#include <ios>
#include <limits>
#include <locale>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <tonyMesh/IO.hpp>
#include <tonyMesh/common.hpp>
#include <tonyMesh/mesh.hpp>
#include <unordered_map>
#include <vector>

namespace tonyMesh::IO {
namespace /* common to read and write*/ {
constexpr std::array<std::string, 3> dirs = {"./meshes/", "../meshes/",
                                             "../../meshes/"};

std::optional<std::fstream> getFile(std::string_view filePath,
                                    std::ios::openmode mode) {
    for (const auto& poss : dirs) {
        std::string path{poss};
        path += filePath;
        std::fstream file(path, mode);
        if (file) return file;
    }

    return std::nullopt;
}

enum class FileType { OBJ, OFF };
const std::unordered_map<std::string, FileType> typeMap{{"obj", FileType::OBJ},
                                                        {"off", FileType::OFF}};

std::optional<FileType> getFileType(std::string_view filePath) {
    auto lastPoint{filePath.find_last_of('.')};
    if (lastPoint == std::string_view::npos) return std::nullopt;

    std::string ext{filePath.substr(lastPoint + 1)};
    auto& f = std::use_facet<std::ctype<char>>(std::locale());
    f.tolower(&ext[0], &ext[0] + ext.size());

    auto found{typeMap.find(ext)};
    if (found != typeMap.end()) return found->second;
    return std::nullopt;
}
}  // namespace

namespace /* read */ {
std::optional<Mesh> readObj(std::fstream& file,
                            [[maybe_unused]] std::string_view fileName) {
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

    VertexMatrix vM(static_cast<Index>(vertices.size()), 3);
    for (Index i{}; i < vM.rows(); i++)
        vM.row(i) = vertices[static_cast<std::size_t>(i)];

    FaceMatrix fM(static_cast<Index>(faces.size()), 3);
    for (Index i{}; i < fM.rows(); i++)
        fM.row(i) = faces[static_cast<std::size_t>(i)];

    return Mesh{vM, fM};
}

std::optional<Mesh> readOff(std::fstream& file, std::string_view fileName) {
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

    // Logs the problem and yields "no mesh"; callers do `return fail(...)`.
    auto fail = [&](const std::string& what) -> std::optional<Mesh> {
        spdlog::error("{}: {}", fileName, what);
        return std::nullopt;
    };

    if (!nextLine() || !line.starts_with("OFF"))
        return fail("missing OFF header");

    if (!nextLine()) return fail("missing counts line");
    unsigned numVertices{}, numFaces{};
    if (!(std::istringstream(line) >> numVertices >> numFaces))
        return fail("invalid counts line");

    VertexMatrix vertices(numVertices, 3);
    for (unsigned i{}; i < numVertices; ++i) {
        if (!nextLine())
            return fail("expected " + std::to_string(numVertices) +
                        " vertices, got " + std::to_string(i));
        std::istringstream ss(line);
        if (!(ss >> vertices(i, 0) >> vertices(i, 1) >> vertices(i, 2)))
            return fail("invalid vertex at index " + std::to_string(i));
    }

    FaceMatrix faces(numFaces, 3);
    for (unsigned i{}; i < numFaces; ++i) {
        if (!nextLine())
            return fail("expected " + std::to_string(numFaces) +
                        " faces, got " + std::to_string(i));
        std::istringstream ss(line);
        unsigned n{};
        if (!(ss >> n >> faces(i, 0) >> faces(i, 1) >> faces(i, 2)) || n != 3)
            return fail("invalid or non-triangle face at index " +
                        std::to_string(i));
    }

    return Mesh{vertices, faces};
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

std::optional<Mesh> readMesh(std::string_view filePath) {
    auto optFile{getFile(filePath, std::ios::in)};
    if (!optFile) {
        spdlog::error("cannot locate file {}", filePath);
        return std::nullopt;
    }

    auto optFileType{getFileType(filePath)};
    if (!optFileType) {
        spdlog::error("cannot recognize extension of {}", filePath);
        return std::nullopt;
    }

    std::optional<Mesh> mesh;
    switch (*optFileType) {
        case FileType::OBJ:
            mesh = readObj(*optFile, filePath);
            break;
        case FileType::OFF:
            mesh = readOff(*optFile, filePath);
            break;
    }
    if (mesh) spdlog::info("read mesh from {}", filePath);
    return mesh;
}

void writeMesh(const Mesh& mesh, std::string_view filePath) {
    auto optFile{getFile(filePath, std::ios::out)};
    if (!optFile) {
        spdlog::error("cannot open file {} for writing", filePath);
        return;
    }

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
