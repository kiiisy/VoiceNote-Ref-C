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

std::vector<float> load_csv(const fs::path &path)
{
    std::vector<float> v;
    std::ifstream      ifs(path);
    if (!ifs) {
        throw std::runtime_error("Failed to open CSV: " + path.string());
    }
    float x;
    while (ifs >> x) {
        v.push_back(x);
    }
    return v;
}

void write_csv(const fs::path &path, const std::vector<float> &v)
{
    fs::create_directories(path.parent_path());
    std::ofstream ofs(path);
    if (!ofs) {
        throw std::runtime_error("Failed to write CSV: " + path.string());
    }
    ofs.setf(std::ios::fixed);
    ofs.precision(10);
    for (float x : v) {
        ofs << x << "\n";
    }
}

void mono_to_stereo_interleaved(const std::vector<float> &mono, std::vector<float> &stereo)
{
    stereo.resize(mono.size() * 2);
    for (std::size_t n = 0; n < mono.size(); ++n) {
        stereo[2 * n + 0] = mono[n];
        stereo[2 * n + 1] = mono[n];
    }
}

double rmse(const std::vector<float> &a, const std::vector<float> &b)
{
    if (a.size() != b.size()) {
        throw std::runtime_error("rmse: size mismatch");
    }
    if (a.empty()) {
        return 0.0;
    }
    double mse = 0.0;
    for (std::size_t i = 0; i < a.size(); ++i) {
        double d = static_cast<double>(a[i]) - static_cast<double>(b[i]);
        mse += d * d;
    }
    mse /= static_cast<double>(a.size());
    return std::sqrt(mse);
}

}  // namespace testutil
