#include "../Signal/Window.h"
#include "../FFT/FFT.h"
#include "Sin.h"

using namespace Eigen;

inline int nextpow2(int x) {
    if (x < 0)
        return 0;
    --x;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return x + 1;
}

inline int sign(double x) {
    return (x > 0.0) - (x < 0.0);
}

static void getSins_f0(const ArrayXcd & S, double fs, double f0, ArrayX5d & sins);
static void delay2spec(int delay, int dftLen, ArrayXcd & shift);
static void findPeaks(const ArrayXd & y, ArrayXd & k, ArrayXd & v);

void sinAnalysis(const ArrayXd & x, double fs, const ArrayXd & f0s, const ArrayXi & times, std::vector<SinFrame> & frames)
{
    constexpr int winDurNbPer = 3;
    constexpr int winOsFactor = 2;

    const int N = times.size();

    frames.reserve(N);

    for (int iframe = 0; iframe < N; ++iframe) {
        int t = times(iframe);
        double T = (double) t / (double) fs;

        double f0 = f0s(iframe);
        
        int winLen = std::round(winDurNbPer * fs / f0 / 2.0) * 2 + 1;
        ArrayXd win = Window::createBlackmanHarris(winLen) / winLen;

        int dftLen = std::pow<int>(2, nextpow2(winLen) + winOsFactor);
        ArrayXcd W;
        delay2spec((winLen - 1) / 2, dftLen, W);
        
        ArrayXi ids;
        ids.setLinSpaced(winLen, t - (winLen - 1) / 2, t + (winLen - 1) / 2);

        if (ids(0) < 0 || ids(last) >= x.size()) {
            continue;
        }

        // Window the signal segment.
        ArrayXd s = x(ids) * win;

        // Compute the spectrum and compensate the window delay.
        fft_plan(dftLen);

        Map<ArrayXcd> in(fft_in(dftLen), dftLen);
        in.setZero();
        in.head(winLen) = s.cast<dcomplex>();

        rcfft(dftLen);

        ArrayXcd S = Map<ArrayXcd>(fft_out(dftLen), dftLen);
        S *= W;

        // Select only peaks around harmonic frequencies.
        ArrayX5d sins;
        getSins_f0(S, fs, f0, sins);

        // Force harmonic frequencies after partial estimation.
        double mf0 = sins(last, 0) / (sins.rows() - 1);
        //mf0 = median(diff(sins(:,0)))
        sins.col(0).setLinSpaced(sins.rows(), 0, mf0 * (sins.rows() - 1));

        frames.push_back({
            .f0 = f0,
            .sins = std::move(sins),
        });
    }

}

static void getSins_f0(const ArrayXcd & S, double fs, double f0, ArrayX5d & sins)
{
    const int maxH = std::round((fs / 2.0 - f0 / 2.0) / f0);
    const int dftLen = S.size();

    const int step = dftLen * f0 / fs;
    
    sins.setZero(maxH + 1, 5);

    // By default, use a simple harmonic sampling.
    sins.col(0).tail(maxH).setLinSpaced(maxH, step, step * maxH);
    sins.col(1).tail(maxH) = S(1 + sins.col(0).cast<int>()).abs();

    // ...and replace by the peaks found.
    ArrayXd k, v;
    ArrayXd las = S(seq(0, last / 2 + 1)).abs().log();
    findPeaks(las, k, v);

    if (k.size() == 1) { 
        for (int n = 1; n <= maxH; ++n) {
            if (abs(k(0) - (1 + sins(n, 0))) < step / 2) {
                sins(n, 0) = k(0) - 1;
                sins(n, 1) = exp(v(0));
                sins(n, 4) = true;
            }
        }
    }
    else if (k.size() > 1) {
        for (int n = 1; n <= maxH; ++n) {
            ArrayXd D = abs(k - 1 + sins(n, 0));
            int mindi;
            double mind = D.maxCoeff(&mindi);
            if (mind < step / 2) {
                sins(n, 0) = k(mindi) - 1;
                sins(n, 1) = exp(v(mindi));
                sins(n, 4) = true;
            }
        }
    }

    // Estimate the phase from linear interpolation of the phase spectrum.
    for (int iq = 1; iq <= maxH; ++iq) {
        double xq = sins(iq, 0);
        int xleft = std::floor(xq);
        double fraction = xq - xleft;

        if (xleft < 0) {
            xleft = 0;
        }
        if (xleft >= dftLen - 1) {
            xleft = dftLen - 2;
        }
        
        double yleft = std::arg(S(xleft));
        double yright = std::arg(S(xleft + 1));

        // Unwrap angle.
        while (yright - yleft > M_PI) {
            yright -= M_PI;
        }
        while (yright - yleft < -M_PI) {
            yright += M_PI;
        }
        
        double yq = yleft * (1 - fraction) + yright * fraction;

        // Wrap angle.
        yq = fmod(yq + M_PI, 2 * M_PI) - M_PI;

        sins(iq, 2) = yq;
    }
    
    sins.col(0).tail(maxH) *= (double) fs / (double) dftLen;

    // The harmonic number.
    sins.col(4).tail(maxH).setLinSpaced(maxH, 1, maxH);

    // Add the DC.
    sins.row(0) << 0, abs(S(0)), std::arg(S(0)), (abs(S(0)) > abs(S(1)));
}

static void delay2spec(int delay, int dftLen, ArrayXcd & shift)
{
    if (delay == 0) {
        shift.setOnes(dftLen);
    }
    else {
        if (dftLen % 2 == 1) {
            // Odd length
            ArrayXcd tmp = exp(dcomplex(0, (delay * 2 * M_PI) / dftLen) * ArrayXcd::LinSpaced((dftLen - 1) / 2, 1, (dftLen - 1) / 2));
            shift << 1, tmp, conj(tmp.reverse());
        }
        else {
            // Even length
            ArrayXcd tmp = exp(dcomplex(0, (delay * 2 * M_PI) / dftLen) * ArrayXcd::LinSpaced(dftLen / 2, 1, dftLen / 2));
            shift << 1, tmp.head(dftLen / 2 - 1), sign(real(tmp(last))), conj(tmp.tail(dftLen / 2 - 1).reverse());
        }
    }
}

static void findPeaks(const ArrayXd & y, ArrayXd & k, ArrayXd & v)
{
    const int ny = y.size();
    
    ArrayXd dx = y(seq(1, last)) - y(seq(0, last - 1));
    
    std::vector<int> _r, _f;
    for (int i = 0; i < ny - 1; ++i) {
        if (dx(i) > 0) {
            _r.push_back(i);
        }
        if (dx(i) < 0) {
            _f.push_back(i);
        }
    }
    
    // We must have at least one rise and one fall.
    if (!_r.empty() && !_f.empty()) {
        ArrayXi r, f;
        r = Map<ArrayXi>(_r.data(), _r.size());
        f = Map<ArrayXi>(_f.data(), _f.size());
        
        // Time since the last rise
        ArrayXi dr = r;
        dr(seq(1, last)) = r(seq(1, last)) - r(seq(0, last - 1));
        ArrayXi rs;
        rs.setOnes(ny);
        rs(r + 1) = 1 - dr;
        rs(0) = 0;
        for (int i = 1; i < rs.size(); ++i) {
            rs(i) += rs(i - 1);
        }
        
        // Time since the last fall
        ArrayXi df = f;
        df(seq(1, last)) = f(seq(1, last)) - f(seq(0, last - 1));
        ArrayXi fs;
        fs.setOnes(ny);
        fs(f + 1) = 1 - df;
        fs(0) = 0;
        for (int i = 1; i < fs.size(); ++i) {
            fs(i) += fs(i - 1);
        }

        // Time to the next rise
        ArrayXi rq(ny);
        rq = -1;
        rq(0) = dr(0) - 1;
        rq(r + 1) << dr(seq(1, last)) - 1, ny - r(last) - 1;
        for (int i = 1; i < rq.size(); ++i) {
            rq(i) += rq(i - 1);
        }

        // Time to the next fall
        ArrayXi fq(ny);
        fq = -1;
        fq(0) = df(0) - 1;
        fq(f + 1) << df(seq(1, last)) - 1, ny - f(last) - 1;
        for (int i = 1; i < fq.size(); ++i) {
            fq(i) += fq(i - 1);
        }

        ArrayXi kint;
        std::vector<int> _k;
        std::vector<double> _v;
        for (int i = 0; i < rs.size(); ++i) {
            if (rs(i) < fs(i) && fq(i) < rq(i) && (fq(i) - rs(i)) / 2 == 0) {
                _k.push_back(i);
                _v.push_back(y(i));
            }
        }
        kint = Map<ArrayXi>(_k.data(), _k.size());
        v = Map<ArrayXd>(_v.data(), _v.size());
        
        // Do quadratic interpolation.
        ArrayXd b = 0.25 * (y(kint + 1) - y(kint - 1));
        ArrayXd a = y(kint) - 2 * b - y(kint - 1);
        
        k.resize(kint.size());

        for (int j = 0; j < k.size(); ++j) {
            if (a(j) > 0) {
                v(j) += (b(j) * b(j)) / a(j);
                k(j) = kint(j) + b(j) / a(j);
            }
            else {
                k(j) = kint(j) + (fq(kint(j)) - rs(kint(j))) / 2;
            }
        }
    }
    else {
        k.setZero(0);
        v.setZero(0);
    }
}
