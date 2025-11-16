#include "DcCut/dc_cut.h"
#include "test_utils.h"
#include <gtest/gtest.h>

#include <cmath>
#include <stdexcept>
#include <vector>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace testutil;

static const std::size_t kN = testutil::num_samples();

// caseN_input.csv を読み込む
static void load_input_case(int case_index, std::vector<float> &x)
{
    std::string file;
    switch (case_index) {
    case 1:
        file = "dc_cut_case1_input.csv";
        break;
    case 2:
        file = "dc_cut_case2_input.csv";
        break;
    case 3:
        file = "dc_cut_case3_input.csv";
        break;
    case 4:
        file = "dc_cut_case4_input.csv";
        break;
    case 5:
        file = "dc_cut_case5_input.csv";
        break;
    default:
        throw std::runtime_error("Invalid case index");
    }

    auto path = golden_path("DcCut", file);
    x         = load_csv(path);

    if (x.size() != kN) {
        throw std::runtime_error("DcCut input size mismatch");
    }
}

// 1ケース分を実行・比較する共通関数
static void run_case(int case_index, const std::string &case_name)
{
    // 入力（mono）
    std::vector<float> x_mono;
    load_input_case(case_index, x_mono);

    // ステレオ化
    std::vector<float> in_stereo;
    std::vector<float> out_stereo;
    mono_to_stereo_interleaved(x_mono, in_stereo);
    out_stereo.resize(in_stereo.size());

    // DCカット実行
    dsp::dc_cut(in_stereo.data(), out_stereo.data(), kN, sample_rate(), dc_cut_cutoff_hz());

    // 左チャンネルだけ抽出
    std::vector<float> out_left(kN);
    for (std::size_t n = 0; n < kN; ++n) {
        out_left[n] = out_stereo[2 * n + 0];
    }

    // C++結果を CSV に出力
    auto out_path = output_path("DcCut", case_name + "_cpp.csv");
    write_csv(out_path, out_left);

    // Python golden 読み込み
    auto golden_file = golden_path("DcCut", case_name + ".csv");
    auto golden      = load_csv(golden_file);

    ASSERT_EQ(golden.size(), out_left.size()) << "Size mismatch: " << case_name;

    // RMSE 比較
    double e = rmse(out_left, golden);
    EXPECT_LT(e, 1e-6) << "RMSE too large in " << case_name;
}

// gtest
TEST(DcCutGolden, Case1_SinePlusDc)
{
    run_case(1, "dc_cut_case1");
}

TEST(DcCutGolden, Case2_LowHighPlusDc)
{
    run_case(2, "dc_cut_case2");
}

TEST(DcCutGolden, Case3_Step)
{
    run_case(3, "dc_cut_case3");
}

TEST(DcCutGolden, Case4_TrianglePlusDc)
{
    run_case(4, "dc_cut_case4");
}

TEST(DcCutGolden, Case5_DriftDc)
{
    run_case(5, "dc_cut_case5");
}
