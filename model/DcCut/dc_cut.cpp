#include "dc_cut.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace dsp {

void init(DcBlockState &st, double fs, double fc)
{
    const double w = 2.0 * M_PI * fc / fs;
    const double a = std::exp(-w);

    // 係数aを直接指定して初期化
    st.a   = a;
    st.x1L = st.y1L = 0.0;
    st.x1R = st.y1R = 0.0;
}

void core(DcBlockState &st, const float *in, float *out, std::size_t frames)
{
    double a   = st.a;
    double x1L = st.x1L;
    double y1L = st.y1L;
    double x1R = st.x1R;
    double y1R = st.y1R;

    for (std::size_t n = 0; n < frames; ++n) {
        const double xL = static_cast<double>(in[2 * n + 0]);
        const double xR = static_cast<double>(in[2 * n + 1]);

        const double yL = xL - x1L + a * y1L;
        const double yR = xR - x1R + a * y1R;

        out[2 * n + 0] = static_cast<float>(yL);
        out[2 * n + 1] = static_cast<float>(yR);

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

void dc_cut(const float *in, float *out, std::size_t frames, double fs, double fc)
{
    DcBlockState st{};
    init(st, fs, fc);
    core(st, in, out, frames);
}

}  // namespace dsp
