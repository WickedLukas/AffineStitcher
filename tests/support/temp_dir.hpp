#pragma once

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>

/// Creates a unique directory under the system temp directory and removes it on destruction.
/// Unique names matter because CTest runs test cases in parallel processes.
class TempDir {
  public:
    TempDir() {
        std::string pattern =
            (std::filesystem::temp_directory_path() / "affine_stitcher-XXXXXX").string();
        if (mkdtemp(pattern.data()) == nullptr) {
            throw std::runtime_error("mkdtemp failed");
        }
        path_ = pattern;
    }
    ~TempDir() {
        std::error_code ignored;
        std::filesystem::remove_all(path_, ignored);
    }
    TempDir(const TempDir&) = delete;
    TempDir& operator=(const TempDir&) = delete;
    TempDir(TempDir&&) = delete;
    TempDir& operator=(TempDir&&) = delete;

    [[nodiscard]] const std::filesystem::path& path() const { return path_; }

    /// Writes `content` to `name` inside the directory and returns the full path.
    [[nodiscard]] std::filesystem::path write(const std::string& name,
                                              std::string_view content) const {
        const std::filesystem::path file = path_ / name;
        std::ofstream{file} << content;
        return file;
    }

  private:
    std::filesystem::path path_;
};

/// Returns the whole content of a text file.
inline std::string read_file(const std::filesystem::path& path) {
    // Not via istreambuf_iterator: GCC 12 reports a false -Wnull-dereference for it at -O2.
    const std::ifstream file{path};
    std::ostringstream content;
    content << file.rdbuf();
    return content.str();
}
