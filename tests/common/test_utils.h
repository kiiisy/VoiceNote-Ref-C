#pragma once
#include "param.h"
#include <cstddef>
#include <filesystem>
#include <string>
#include <vector>

namespace testutil {

namespace fs = std::filesystem;

// 共通パラメータ
inline double sample_rate()
{
    return kFs;
}
inline double duration_sec()
{
    return 1.0;
}

// DcCutパラメータ
inline double dc_cut_cutoff_hz()
{
    return kFc;
}

inline std::size_t num_samples()
{
    return static_cast<std::size_t>(sample_rate() * duration_sec());
}

// プロジェクトルート
fs::path project_root();

// tests/<Module>/golden/foo.csv
fs::path golden_path(const std::string &module, const std::string &filename);

// tests/<Module>/output/foo_cpp.csv
fs::path output_path(const std::string &module, const std::string &filename);

// CSV I/O
std::vector<float> load_csv(const fs::path &path);
void               write_csv(const fs::path &path, const std::vector<float> &v);

// 波形ユーティリティ
void mono_to_stereo_interleaved(const std::vector<float> &mono, std::vector<float> &stereo);

// RMSE 計算
double rmse(const std::vector<float> &a, const std::vector<float> &b);

}  // namespace testutil
