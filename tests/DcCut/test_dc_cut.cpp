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

// 入力生成
static void gen_input_case1(std::vector<float> &x)
{
    x.resize(kN);
    const double dc   = 0.2;
    const double freq = 1000.0;

    for (std::size_t n = 0; n < kN; ++n) {
        double t = static_cast<double>(n) / sample_rate();
        x[n]     = static_cast<float>(std::sin(2.0 * M_PI * freq * t) + dc);
    }
}

static void gen_input_case2(std::vector<float> &x)
{
    x.resize(kN);
    const double dc     = 0.1;
    const double f_low  = 50.0;
    const double f_high = 3000.0;

    for (std::size_t n = 0; n < kN; ++n) {
        double t = static_cast<double>(n) / sample_rate();
        double v = 0.6 * std::sin(2.0 * M_PI * f_low * t) + 0.3 * std::sin(2.0 * M_PI * f_high * t) + dc;
        x[n]     = static_cast<float>(v);
    }
}

static void gen_input_case3(std::vector<float> &x)
{
    x.assign(kN, 1.0f);  // ステップ入力
}

static void gen_input_case4(std::vector<float> &x)
{
    // Pythonで生成した乱数波形を読む
    auto csv = golden_path("DcCut", "dc_cut_case4_input.csv");
    x        = load_csv(csv);

    if (x.size() != kN) {
        throw std::runtime_error("Case4 input CSV size mismatch");
    }
}

static void gen_input_case5(std::vector<float> &x)
{
    // sin(1000Hz) + 0.1*sin(0.3Hz)
    x.resize(kN);
    const double f_sig   = 1000.0;
    const double f_drift = 0.3;

    for (std::size_t n = 0; n < kN; ++n) {
        double t     = static_cast<double>(n) / sample_rate();
        double drift = 0.1 * std::sin(2.0 * M_PI * f_drift * t);
        double sig   = std::sin(2.0 * M_PI * f_sig * t);
        x[n]         = static_cast<float>(sig + drift);
    }
}

// 1ケース分を実行・比較する共通関数
static void run_case(int case_index, const std::string &case_name)
{
    // 入力生成 (mono)
    std::vector<float> x_mono;
    switch (case_index) {
    case 1:
        gen_input_case1(x_mono);
        break;
    case 2:
        gen_input_case2(x_mono);
        break;
    case 3:
        gen_input_case3(x_mono);
        break;
    case 4:
        gen_input_case4(x_mono);
        break;
    case 5:
        gen_input_case5(x_mono);
        break;
    default:
        throw std::runtime_error("Invalid case index");
    }

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
