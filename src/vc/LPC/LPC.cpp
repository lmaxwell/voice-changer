#include "LPC.h"

using namespace Eigen;

void lpcFrame(const ArrayXd & x, int order, ArrayXd & a)
{ 
    const int nCoefficients = order + 1;

    a.setZero(nCoefficients);

    const int n = x.size(), m = a.size();

    ArrayXd b1 = ArrayXd::Zero(n + 1);
    ArrayXd b2 = ArrayXd::Zero(n + 1);
    ArrayXd aa = ArrayXd::Zero(m + 1);

    double p = 0.0;
    for (int j = 0; j < n; ++j)
        p += x(j) * x(j);

    double xms = p / static_cast<double>(n);
    if (xms <= 0.0)
        goto end;

    b1(1) = x(0);
    b2(n - 1) = x(n - 1);
    for (int j = 2; j <= n; ++j)
        b1(j) = b2(j - 1) = x(j - 1);

    for (int i = 1; i <= m; ++i) {
        double num = 0.0, denom = 0.0;
        for (int j = 1; j <= n - i; ++j) {
            num += b1(j) * b2(j);
            denom += b1(j) * b1(j) + b2(j) * b2(j);
        }

        if (denom <= 0.0)
            goto end;

        a(i - 1) = (2.0 * num) / denom;

        xms *= 1.0 - a(i - 1) * a(i - 1);

        for (int j = 1; j <= i - 1; ++j)
            a(j - 1) = aa(j) - a(i - 1) * aa(i - j);

        if (i < m) {
            for (int j = 1; j <= i; ++j)
                aa(j) = a(j - 1);
            for (int j = 1; j <= n - i - 1; ++j) {
                b1(j) -= aa(i) * b2(j);
                b2(j) = b2(j + 1) - aa(i) * b1(j + 1);
            }
        }
    }

end:

    a = -a;

}

