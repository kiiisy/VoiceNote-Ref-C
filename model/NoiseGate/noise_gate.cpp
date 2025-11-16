#include "noise_gate.h"
#include <cmath>

namespace dsp {

namespace {

inline double exp_coeff(double fs, double tau)
{
    if (tau <= 0.0) {
        return 0.0;  // 即座に target に追従
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

void core(NoiseGateState &st, const float *in, float *out, std::size_t frames)
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
        const double xL = static_cast<double>(in[2 * n + 0]);
        const double xR = static_cast<double>(in[2 * n + 1]);

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

        gainL = aL * gainL + (1.0 - aL) * targetL;
        gainR = aR * gainR + (1.0 - aR) * targetR;

        out[2 * n + 0] = static_cast<float>(gainL * xL);
        out[2 * n + 1] = static_cast<float>(gainR * xR);
    }

    st.gainL      = gainL;
    st.gainR      = gainR;
    st.gate_openL = gate_openL;
    st.gate_openR = gate_openR;
}

void noise_gate(const float *in, float *out, std::size_t frames, double fs, const NoiseGateParams &p)
{
    NoiseGateState st{};
    init(st, fs, p);
    core(st, in, out, frames);
}

}  // namespace dsp
