#include "../Signal/Filter.h"
#include "../Signal/Window.h"
#include "LPC.h"

using namespace Eigen;

void lpcResidual(const ArrayXd & x, int L, int shift, int order, ArrayXd & res)
{
    const int len(x.size());
    int start, stop;
   
    ArrayXd win = Window::createHanning(L);
  
    start = 0;
    stop = start + L;

    res.setZero(len);

    ArrayXd segment, A, inv;

    while (stop < len) {
        segment = x(seq(start, stop)) * win;
        
        lpcFrame(segment, order, A);
        Filter::apply(A, segment, inv);
        inv *= std::sqrt(segment.square().sum() / inv.square().sum());

        res(seq(start, stop)) += inv;

        start += shift;
        stop += shift;
    }

    res /= res.abs().maxCoeff();

}
