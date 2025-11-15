#pragma once
#include <cstddef>

namespace dsp {

// DCブロック（1次HPF）ステレオ用ステート
struct DcBlockState
{
    double a;  // フィルタ係数

    double x1L;  // 前回入力L
    double y1L;  // 前回出力L
    double x1R;  // 前回入力R
    double y1R;  // 前回出力R
};

void dc_cut(const float *in, float *out, std::size_t frames, double fs, double fc);

}  // namespace dsp
