#include <iostream>
#include "../Signal/Filter.h"
#include "../Signal/Window.h"
#include "../LPC/LPC.h"
#include "IAIF.h"

using namespace Eigen;

static ArrayXd conv(const ArrayXd & a, const ArrayXd & b);

void iaif(IAIF_Result & result, ArrayXd & x, double fs, double winLenMs, double winShiftMs, int p_vt, int p_gl, double d)
{
    // Defaults:
    //  winLen = 25.0 ms
    //  winShift = 5.0 ms
    //  p_vt = fs / 1000 + 2
    //  p_gl = 4
    //  d = 0.997

    const int lenx = x.size();
    const int winLen = std::round(winLenMs / 1000.0 * fs);
    const int winShift = std::round(winShiftMs / 1000.0 * fs);
    const int numFramesEstimate = std::round((lenx - winLen) / winShift);

    ArrayXd win = Window::createHamming(winLen);

    ArrayXd wins;
    wins.setZero(lenx);
    result.g.setZero(lenx);
    result.ag.reserve(numFramesEstimate);
    result.av.reserve(numFramesEstimate);

    int start, stop, index;
    IAIF_Frame frame;

    start = 0;
    stop = start + winLen - 1;
    index = 0;
    while (stop < lenx) {
        ArrayXd xframe = x(seq(start, stop));
    
        iaifFrame(frame, xframe, fs, p_vt, p_gl, d, win);
    
        result.ag.push_back(frame.ag);
        result.av.push_back(frame.av);

        result.g(seq(start, stop)) += frame.g * win;
        
        wins(seq(start, stop)) += win;

        start += winShift;
        stop = start + winLen - 1;
        index++;

        std::cout << "IAIF: Frame " << index << " out of " << numFramesEstimate << std::endl;
    }

    for (int i = 0; i < lenx; ++i) {
        if (wins(i) > 0) {
            result.g(i) /= wins(i);
        }
    }
}

void iaifFrame(IAIF_Frame & frame, ArrayXd & s_gv1, double fs, int p_vt, int p_gl, double d, const ArrayXd & win)
{
    // Defaults:
    //  p_vt = fs / 1000 + 2
    //  p_gl = 4
    //  d = 0.997

    const int len = s_gv1.size();

    ArrayXd lib(1);
    ArrayXd lia(2);
    lib(0) = 1.0;
    lia(0) = 1.0;
    lia(1) = -d;

    int preflt = p_vt + 1;
   
    // Addition of pre-frame.

    ArrayXd x_gv1(preflt + len);
    x_gv1.head(preflt).setLinSpaced(-s_gv1(0), s_gv1(0));
    x_gv1.tail(len) = s_gv1;
    
    // Cancel lip radiation contribution.

    ArrayXd s_gv, x_gv;
    Filter::apply(lib, lia, s_gv1, s_gv);
    Filter::apply(lib, lia, x_gv1, x_gv);

    // Gross glottis estimation.

    ArrayXd ag1, ag1x, x_v1x, s_v1x;

    lpcFrame(s_gv * win, 1, ag1);

    for (int i = 1; i <= p_gl - 1; ++i) {
        Filter::apply(ag1, x_gv, x_v1x);
        s_v1x = x_v1x.tail(len);

        lpcFrame(s_v1x * win, 1, ag1x);

        ag1 = conv(ag1, ag1x);
    }

    // Gross vocal tract estimation.
    
    ArrayXd x_v1, s_v1, av1;

    Filter::apply(ag1, x_gv, x_v1);
    s_v1 = x_v1.tail(len);

    lpcFrame(s_v1 * win, p_vt, av1);

    // Fine glottis estimation.

    ArrayXd x_g1, s_g1, ag;

    Filter::apply(av1, x_gv, x_g1);
    s_g1 = x_g1.tail(len);

    lpcFrame(s_g1 * win, p_gl, ag);
    
    // Fine vocal tract estimation.

    ArrayXd x_v, s_v, av;

    Filter::apply(ag, x_gv, x_v);
    s_v = x_v.tail(len);

    lpcFrame(s_v * win, p_vt, av);

    frame.ag = ag;
    frame.av = av;
    frame.g = s_v;
}

static ArrayXd conv(const ArrayXd & a, const ArrayXd & b)
{
    const int na = a.size();
    const int nb = b.size();
    const int n = na + nb - 1;

    ArrayXd out(n);

    for (int i = 0; i < n; ++i) {
        const int jmn = (i >= nb - 1) ? i - (nb - 1) : 0;
        const int jmx = (i < na - 1) ? i : na - 1;

        out(i) = 0.0;
        for (int j = jmn; j <= jmx; ++j) {
            out(i) += (a(j) * b(i - j));
        }
    }

    return out;
}
