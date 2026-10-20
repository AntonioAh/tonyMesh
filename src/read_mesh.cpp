#include <read_mesh.hpp>
#include <cctype>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <utility>

namespace tonyMesh {

    namespace {
        std::filesystem::path resolveDataPath(const std::string& fileName) {
            std::filesystem::path p(fileName);
            if (std::filesystem::exists(p)) {
                return p;
            }
#ifdef TONYMESH_DATA_DIR
            std::filesystem::path dataPath = std::filesystem::path(TONYMESH_DATA_DIR) / p;
            if (std::filesystem::exists(dataPath)) {
                return dataPath;
            }
#endif
            for (const auto* prefix : {"data", "../data", "../../data"}) {
                std::filesystem::path relPath = std::filesystem::path(prefix) / p;
                if (std::filesystem::exists(relPath)) {
                    return relPath;
                }
            }
            return p;
        }

        bool nextValidLine(std::ifstream& file, std::string& line) {
            while (std::getline(file, line)) {
                const auto first = line.find_first_not_of(" \t\r\n");
                if (first == std::string::npos || line[first] == '#') {
                    continue;
                }
                return true;
            }
            return false;
        }

        std::expected<Mesh, IO_ERROR> readOFF(const std::filesystem::path& filePath) {
            std::ifstream file(filePath);
            if (!file.is_open()) {
                return std::unexpected(IO_ERROR::GENERIC);
            }

            std::string line;
            if (!nextValidLine(file, line)) {
                return std::unexpected(IO_ERROR::GENERIC);
            }

            std::istringstream headerStream(line);
            std::string magic;
            headerStream >> magic;

            if (magic != "OFF" && magic != "NOFF" && magic != "COFF" && magic != "STOFF") {
                return std::unexpected(IO_ERROR::GENERIC);
            }

            Index nV = 0;
            Index nF = 0;
            Index nE = 0;

            if (!(headerStream >> nV >> nF >> nE)) {
                if (!nextValidLine(file, line)) {
                    return std::unexpected(IO_ERROR::GENERIC);
                }
                std::istringstream countStream(line);
                if (!(countStream >> nV >> nF >> nE)) {
                    return std::unexpected(IO_ERROR::GENERIC);
                }
            }

            if (nV < 0 || nF < 0) {
                return std::unexpected(IO_ERROR::GENERIC);
            }

            VertexMatrix vertices(nV, 3);
            FaceMatrix faces(nF, 3);

            for (Index v = 0; v < nV; ++v) {
                if (!nextValidLine(file, line)) {
                    return std::unexpected(IO_ERROR::GENERIC);
                }
                double x{}, y{}, z{};
                if (std::sscanf(line.c_str(), "%lf %lf %lf", &x, &y, &z) < 3) {
                    return std::unexpected(IO_ERROR::GENERIC);
                }
                vertices(v, 0) = static_cast<Scalar>(x);
                vertices(v, 1) = static_cast<Scalar>(y);
                vertices(v, 2) = static_cast<Scalar>(z);
            }

            for (Index f = 0; f < nF; ++f) {
                if (!nextValidLine(file, line)) {
                    return std::unexpected(IO_ERROR::GENERIC);
                }
                unsigned int valence{}, i0{}, i1{}, i2{};
                if (std::sscanf(line.c_str(), "%u %u %u %u", &valence, &i0, &i1, &i2) < 4 || valence != 3) {
                    return std::unexpected(IO_ERROR::GENERIC);
                }
                faces(f, 0) = static_cast<FaceType>(i0);
                faces(f, 1) = static_cast<FaceType>(i1);
                faces(f, 2) = static_cast<FaceType>(i2);
            }

            return Mesh(std::move(vertices), std::move(faces));
        }
    }

    std::expected<Mesh, IO_ERROR> readMesh(std::string& fileName, MESH_FILE_TYPE fileType) {
        const auto filePath = resolveDataPath(fileName);
        fileName = filePath.string();

        if (fileType == MESH_FILE_TYPE::GENERIC) {
            std::string ext = filePath.extension().string();
            if (!ext.empty() && ext.front() == '.') {
                ext.erase(0, 1);
            }
            for (auto& c : ext) {
                c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
            }
            if (ext == "off") {
                fileType = MESH_FILE_TYPE::OFF;
            }
        }

        switch (fileType) {
            case MESH_FILE_TYPE::OFF:
                return readOFF(filePath);
            case MESH_FILE_TYPE::STD:
            case MESH_FILE_TYPE::GENERIC:
            default:
                return std::unexpected(IO_ERROR::GENERIC);
        }
    }

    std::expected<Mesh, IO_ERROR> readMesh(std::string&& fileName, MESH_FILE_TYPE fileType) {
        return readMesh(fileName, fileType);
    }

}
