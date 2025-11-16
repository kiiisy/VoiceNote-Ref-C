#include "NoiseGate/noise_gate.h"
#include "test_utils.h"
#include <gtest/gtest.h>

#include <stdexcept>
#include <string>
#include <vector>

using namespace testutil;

static const std::size_t kN = testutil::num_samples();

dsp::NoiseGateParams default_params()
{
    dsp::NoiseGateParams p{};
    p.th_open      = 0.05;
    p.th_close     = 0.04;
    p.attack_time  = 0.005;  // 5 ms
    p.release_time = 0.050;  // 50 ms
    return p;
}

// caseN_input.csv を読み込む
static void load_input_case(int case_index, std::vector<float> &x)
{
    std::string file;
    switch (case_index) {
    case 1:
        file = "noise_gate_case1_input.csv";
        break;
    case 2:
        file = "noise_gate_case2_input.csv";
        break;
    case 3:
        file = "noise_gate_case3_input.csv";
        break;
    case 4:
        file = "noise_gate_case4_input.csv";
        break;
    case 5:
        file = "noise_gate_case5_input.csv";
        break;
    default:
        throw std::runtime_error("Invalid case index");
    }

    auto path = golden_path("NoiseGate", file);
    x         = load_csv(path);

    if (x.size() != kN) {
        throw std::runtime_error("NoiseGate input size mismatch");
    }
}

// 1ケース分の共通実行
static void run_case(int case_index, const std::string &case_name)
{
    // 入力
    std::vector<float> x_mono;
    load_input_case(case_index, x_mono);

    // ステレオ化
    std::vector<float> in_stereo;
    std::vector<float> out_stereo;
    mono_to_stereo_interleaved(x_mono, in_stereo);
    out_stereo.resize(in_stereo.size());

    // ノイズゲート実行
    const auto params = default_params();
    dsp::noise_gate(in_stereo.data(), out_stereo.data(), kN, sample_rate(), params);

    // 左チャンネルだけ取り出し
    std::vector<float> out_left(kN);
    for (std::size_t n = 0; n < kN; ++n) {
        out_left[n] = out_stereo[2 * n + 0];
    }

    // C++結果を CSV に保存（デバッグ用）
    auto out_path = output_path("NoiseGate", case_name + "_cpp.csv");
    write_csv(out_path, out_left);

    // Python golden 読み込み
    auto golden_file = golden_path("NoiseGate", case_name + ".csv");
    auto golden      = load_csv(golden_file);

    ASSERT_EQ(golden.size(), out_left.size()) << "Size mismatch: " << case_name;

    // RMSE 比較
    double e = rmse(out_left, golden);
    EXPECT_LT(e, 1e-6) << "RMSE too large in " << case_name;
}

// gtest
TEST(NoiseGateGolden, Case1_NoisePlusBurst)
{
    run_case(1, "noise_gate_case1");
}

TEST(NoiseGateGolden, Case2_FadeInSine)
{
    run_case(2, "noise_gate_case2");
}

TEST(NoiseGateGolden, Case3_SlowNoise)
{
    run_case(3, "noise_gate_case3");
}

TEST(NoiseGateGolden, Case4_ThresholdSquare)
{
    run_case(4, "noise_gate_case4");
}

TEST(NoiseGateGolden, Case5_PhraseLike)
{
    run_case(5, "noise_gate_case5");
}
