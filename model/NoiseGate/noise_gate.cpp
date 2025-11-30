#include "noise_gate.h"
#include <cmath>

namespace dsp {

namespace {

// 正規化値(-1〜1) → Q15
inline int16_t sat_q15_from_norm(double y_norm)
{
    double v = y_norm * 32768.0;
    long   t = std::lround(v);
    if (t > 32767) {
        t = 32767;
    }
    if (t < -32768) {
        t = -32768;
    }
    return static_cast<int16_t>(t);
}

// exp smoothing の係数
inline double exp_coeff(double fs, double tau)
{
    if (tau <= 0.0) {
        // tau=0 のときは即追従
        return 0.0;
    }
    return std::exp(-1.0 / (fs * tau));
}

}  // namespace

void init(NoiseGateState &st, double fs, const NoiseGateParams &p)
{
    st.gainL = 0.0;
    st.gainR = 0.0;

    st.gate_openL = false;
    st.gate_openR = false;

    st.a_attack  = exp_coeff(fs, p.attack_time);
    st.a_release = exp_coeff(fs, p.release_time);

    st.th_open  = p.th_open;
    st.th_close = p.th_close;
}

// ★ 16bit(Q15) 前提のリファレンス core
//   in/out: [L0, R0, L1, R1, ...] の int16_t(Q15)
void core(NoiseGateState &st, const int16_t *in, int16_t *out, std::size_t frames)
{
    double gainL      = st.gainL;
    double gainR      = st.gainR;
    bool   gate_openL = st.gate_openL;
    bool   gate_openR = st.gate_openR;

    const double th_open   = st.th_open;
    const double th_close  = st.th_close;
    const double a_attack  = st.a_attack;
    const double a_release = st.a_release;

    for (std::size_t n = 0; n < frames; ++n) {
        // 入力 Q15 → 正規化 double
        const double xL = static_cast<double>(in[2 * n + 0]) / 32768.0;
        const double xR = static_cast<double>(in[2 * n + 1]) / 32768.0;

        const double levelL = std::fabs(xL);
        const double levelR = std::fabs(xR);

        // --- L チャンネル: ヒステリシス付きゲート状態更新 ---
        if (gate_openL) {
            if (levelL <= th_close) {
                gate_openL = false;
            }
        } else {
            if (levelL >= th_open) {
                gate_openL = true;
            }
        }

        // --- R チャンネル ---
        if (gate_openR) {
            if (levelR <= th_close) {
                gate_openR = false;
            }
        } else {
            if (levelR >= th_open) {
                gate_openR = true;
            }
        }

        const double targetL = gate_openL ? 1.0 : 0.0;
        const double targetR = gate_openR ? 1.0 : 0.0;

        const double aL = gate_openL ? a_attack : a_release;
        const double aR = gate_openR ? a_attack : a_release;

        // スムージング (ゲイン)
        gainL = aL * gainL + (1.0 - aL) * targetL;
        gainR = aR * gainR + (1.0 - aR) * targetR;

        // 出力: 正規化 * ゲイン → Q15
        const double yL_norm = gainL * xL;
        const double yR_norm = gainR * xR;

        out[2 * n + 0] = sat_q15_from_norm(yL_norm);
        out[2 * n + 1] = sat_q15_from_norm(yR_norm);
    }

    st.gainL      = gainL;
    st.gainR      = gainR;
    st.gate_openL = gate_openL;
    st.gate_openR = gate_openR;
}

// 外から呼ぶラッパ（S16 in/out）
void noise_gate(const int16_t *in, int16_t *out, std::size_t frames, double fs, const NoiseGateParams &p)
{
    NoiseGateState st{};
    init(st, fs, p);
    core(st, in, out, frames);
}

}  // namespace dsp
