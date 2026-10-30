#include <core/readMesh.hpp>

#include <filesystem>
#include <array>
namespace  fs = std::filesystem;

constexpr static std::array<std::string, 1> folders = {
    "data/"
};

std::optional<std::string> tonyMesh::findDataFile(const std::string& filename) {
    fs::path projectRoot(PROJECT_SOURCE_DIR);

    for (const auto& folder : folders) {
        fs::path dirPath(folder);
        fs::path resolvedDir = dirPath.is_absolute() ? dirPath : (projectRoot / dirPath);
        fs::path filePath = resolvedDir / filename;

        std::error_code ec;
        // Using error_code prevents unexpected exceptions if a directory lacks permissions
        if (fs::exists(filePath, ec) && !ec && fs::is_regular_file(filePath, ec) && !ec) {
            return filePath.string();
        }
    }

    return std::nullopt;
}