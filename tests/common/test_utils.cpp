#include "test_utils.h"
#include <cmath>
#include <fstream>
#include <stdexcept>

#ifndef PROJECT_SOURCE_DIR
#define PROJECT_SOURCE_DIR "."
#endif

namespace testutil {

fs::path project_root()
{
    return fs::path(PROJECT_SOURCE_DIR);
}

fs::path golden_path(const std::string &module, const std::string &filename)
{
    return project_root() / "tests" / module / "golden" / filename;
}

fs::path output_path(const std::string &module, const std::string &filename)
{
    return project_root() / "tests" / module / "output" / filename;
}

std::vector<int16_t> load_csv(const fs::path &path)
{
    std::vector<int16_t> v;
    std::ifstream        ifs(path);
    if (!ifs) {
        throw std::runtime_error("Failed to open CSV: " + path.string());
    }
    int value;
    while (ifs >> value) {
        v.push_back(static_cast<int16_t>(value));
    }
    return v;
}

void write_csv(const fs::path &path, const std::vector<int16_t> &v)
{
    fs::create_directories(path.parent_path());
    std::ofstream ofs(path);
    if (!ofs) {
        throw std::runtime_error("Failed to write CSV: " + path.string());
    }
    for (auto s : v) {
        ofs << static_cast<int>(s) << "\n";
    }
}

void mono_to_stereo_interleaved(const std::vector<int16_t> &mono, std::vector<int16_t> &stereo)
{
    stereo.resize(mono.size() * 2);
    for (std::size_t n = 0; n < mono.size(); ++n) {
        stereo[2 * n + 0] = mono[n];
        stereo[2 * n + 1] = mono[n];
    }
}

double rmse(const std::vector<int16_t> &a, const std::vector<int16_t> &b)
{
    if (a.size() != b.size()) {
        throw std::runtime_error("rmse(int16): size mismatch");
    }
    if (a.empty()) {
        return 0.0;
    }
    double mse = 0.0;
    for (std::size_t i = 0; i < a.size(); ++i) {
        double af = static_cast<double>(a[i]) / 32768.0;
        double bf = static_cast<double>(b[i]) / 32768.0;
        double d  = af - bf;
        mse += d * d;
    }
    mse /= static_cast<double>(a.size());
    return std::sqrt(mse);
}

}  // namespace testutil
