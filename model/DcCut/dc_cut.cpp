#include "dc_cut.h"
#include <cmath>

namespace dsp {

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ===== Q15 utilities =====
int16_t sat_q15_from_norm(double y)
{
    double v = y * 32768.0;
    long   t = std::lround(v);
    if (t > 32767)
        t = 32767;
    if (t < -32768)
        t = -32768;
    return static_cast<int16_t>(t);
}

// ===== init =====

void init(DcBlockState &st, double fs, double fc)
{
    const double w = 2.0 * M_PI * fc / fs;
    const double a = std::exp(-w);

    st.a   = a;
    st.x1L = st.y1L = 0.0;
    st.x1R = st.y1R = 0.0;
}

// ===== core: S16 in/S16 out =====

void core(DcBlockState &st, const int16_t *in, int16_t *out, std::size_t frames)
{
    double a   = st.a;
    double x1L = st.x1L;
    double y1L = st.y1L;
    double x1R = st.x1R;
    double y1R = st.y1R;

    for (std::size_t n = 0; n < frames; ++n) {
        // 入力 Q15 → 正規化
        const double xL = static_cast<double>(in[2 * n + 0]) / 32768.0;
        const double xR = static_cast<double>(in[2 * n + 1]) / 32768.0;

        // 1次HPF
        const double yL = xL - x1L + a * y1L;
        const double yR = xR - x1R + a * y1R;

        // 正規化 → Q15
        out[2 * n + 0] = sat_q15_from_norm(yL);
        out[2 * n + 1] = sat_q15_from_norm(yR);

        // 状態更新（正規化 float で保持）
        x1L = xL;
        y1L = yL;
        x1R = xR;
        y1R = yR;
    }

    st.x1L = x1L;
    st.y1L = y1L;
    st.x1R = x1R;
    st.y1R = y1R;
}

void dc_cut(const int16_t *in, int16_t *out, std::size_t frames, double fs, double fc)
{
    DcBlockState st{};
    init(st, fs, fc);
    core(st, in, out, frames);
}

}  // namespace dsp
