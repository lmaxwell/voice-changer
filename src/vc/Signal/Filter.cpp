#include "Filter.h"

using namespace Eigen;

void Filter::preEmphasis(ArrayXd & x, double samplingFrequency, double preEmphasisFrequency)
{
    if (preEmphasisFrequency >= 0.5 * samplingFrequency)
        return;

    const double preEmphasis = exp(-2.0 * M_PI * preEmphasisFrequency / samplingFrequency);

    for (int i = x.size() - 1; i >= 1; --i) {
        x(i) -= preEmphasis * x(i - 1);
    }
}

void Filter::apply(const ArrayXd & b, const ArrayXd & x, ArrayXd & y)
{
    y.resize(x.size());
    for (int n = 0; n < x.size(); ++n) {
        y(n) = 0.0;
        for (int i = 0; i < b.size() && n - i >= 0 && n - i < x.size(); ++i) {
            y(n) += b(i) * x(n - i);
        }
    }
}

void Filter::apply(const ArrayXd & b, const ArrayXd & a, const ArrayXd & x, ArrayXd & y)
{
    y.resize(x.size());
    for (int n = 0; n < x.size(); ++n) {
        y(n) = 0.0;
        for (int i = 0; i < b.size() && n - i >= 0 && n - i < x.size(); ++i) {
            y(n) += b(i) * x(n - i);
        }
        for (int j = 1; j < a.size() && n - j >= 0 && n - j < x.size(); ++j) {
            y(n) -= a(j) * x(n - j);
        }
    }
}
