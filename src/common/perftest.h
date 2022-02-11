#ifndef PERFTEST_H
#define PERFTEST_H

#include <vector>

double min(std::vector<double> const& v);
double max(std::vector<double> const& v);
double average(std::vector<double> const& v);
double standard_deviation(std::vector<double> const& v);
double median(std::vector<double> v);

#endif