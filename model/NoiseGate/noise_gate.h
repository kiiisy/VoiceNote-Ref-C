#pragma once
#include <cstddef>

namespace dsp {

// ノイズゲートのパラメータ
struct NoiseGateParams
{
    double th_open;       // ゲートが開くしきい値
    double th_close;      // ゲートが閉じるしきい値（ヒステリシス）
    double attack_time;   // アタック時間 [秒]
    double release_time;  // リリース時間 [秒]
};

// ステレオ用ステート
struct NoiseGateState
{
    // ゲイン
    double gainL;
    double gainR;

    // ゲート状態
    bool gate_openL;
    bool gate_openR;

    // スムージング係数
    double a_attack;
    double a_release;

    // しきい値
    double th_open;
    double th_close;
};

void noise_gate(const float *in, float *out, std::size_t frames, double fs, const NoiseGateParams &p);

}  // namespace dsp
