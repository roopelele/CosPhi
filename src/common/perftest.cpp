#include "perftest.h"
#include <algorithm>
#include <math.h>
#include <numeric>

double min(std::vector<double> const& v)
{
    return *min_element(v.begin(), v.end());
}

double max(std::vector<double> const& v)
{
    return *max_element(v.begin(), v.end());
}

double average(std::vector<double> const& v)
{
    if (v.empty()) {
        return 0;
    }

    auto const count = static_cast<double>(v.size());
    return std::reduce(v.begin(), v.end()) / count;
}

double standard_deviation(std::vector<double> const& v)
{
    double mean   = average(v);
    double sq_sum = std::inner_product(v.begin(), v.end(), v.begin(), 0.0);
    return std::sqrt(sq_sum / v.size() - mean * mean);
}

double median(std::vector<double> v)
{
    unsigned long size = v.size();

    if (size == 0) {
        return 0; // Undefined, really.
    }
    else {
        std::sort(v.begin(), v.end());
        if (size % 2 == 0) {
            return (v[size / 2 - 1] + v[size / 2]) / 2;
        }
        else {
            return v[size / 2];
        }
    }
}