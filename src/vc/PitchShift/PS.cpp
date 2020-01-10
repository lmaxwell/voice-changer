#include "../FFT/FFT.h"
#include "../Signal/Resample.h"
#include "PS.h"

using namespace Eigen;

ArrayXd pitchShift(int lenx, int winLen, int winShift, std::vector<ArrayXd> & Hgvec, double fs, int nfft, double factor)
{
    rfft_plan(nfft);
    irfft_plan(nfft);

    Map<ArrayXd> fin(rfft_in(nfft), nfft);    
    Map<ArrayXd> fout(rfft_out(nfft), nfft);

    Map<ArrayXd> bin(irfft_in(nfft), nfft);    
    Map<ArrayXd> bout(irfft_out(nfft), nfft);
   
    ArrayXd g;
    g.setZero(lenx);

    int start = 0;

    for (ArrayXd & Hg : Hgvec)
    {
        fin.setZero();
        fin.head(Hg.size()) = Hg;

        rfft(nfft);
        
        ArrayXd x = fout.head(nfft / 2);
        ArrayXd y = Resample::resample(x, 1.0, factor, 1);
        const int leny = y.size();
        y.conservativeResize(nfft / 2);
        if (leny < nfft / 2) {
            y.tail(nfft / 2 - leny).setZero();
        }
        
        bin.head(nfft / 2) = y;
        bin.tail(nfft / 2) = y.reverse();

        irfft(nfft);

        g.segment(start, winLen) += bout.head(winLen); 

        start += winShift;
    }

    return g;
}
