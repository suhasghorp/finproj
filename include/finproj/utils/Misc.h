#ifndef FINPROJ_INCLUDE_FINPROJ_UTILS_MISC_H_
#define FINPROJ_INCLUDE_FINPROJ_UTILS_MISC_H_

#include <vector>
#include <cmath>
#include <Eigen/Dense>
using namespace Eigen;

constexpr double gSmall = 1e-12;
constexpr double gdays_in_month = 365.242/12.0;
constexpr double PI = 3.14159265358979323846;
constexpr double INVROOT2PI = 0.3989422804014327;


template <typename T>
std::vector<T> linspace(T a, T b, std::size_t N) {
  T h = (b - a) / static_cast<T>(N-1);
  std::vector<T> xs(N);
  typename std::vector<T>::iterator x;
  T val;
  for (x = xs.begin(), val = a; x != xs.end(); ++x, val += h)
    *x = val;
  return xs;
}

inline double N(double x) {
  auto a1 = 0.319381530;
  auto a2 = -0.356563782;
  auto a3 = 1.781477937;
  auto a4 = -1.821255978;
  auto a5 = 1.330274429;
  auto g = 0.2316419;

  auto k = 1.0 / (1.0 + g * fabs(x));
  auto k2 = k * k;
  auto k3 = k2 * k;
  auto k4 = k3 * k;
  auto k5 = k4 * k;
  double phi{0.0};
  if (x >= 0.0) {
    auto c = (a1 * k + a2 * k2 + a3 * k3 + a4 * k4 + a5 * k5);
    phi = 1.0 - c * exp(-x * x / 2.0) * INVROOT2PI;
  } else {
    phi = 1.0 - N(-x);
  }
  return phi;
}

inline double norminvcdf(double p) {
  auto a1 = -39.6968302866538;
  auto a2 = 220.946098424521;
  auto a3 = -275.928510446969;
  auto a4 = 138.357751867269;
  auto a5 = -30.6647980661472;
  auto a6 = 2.50662827745924;

  auto b1 = -54.4760987982241;
  auto b2 = 161.585836858041;
  auto b3 = -155.698979859887;
  auto b4 = 66.8013118877197;
  auto b5 = -13.2806815528857;

  auto c1 = -7.78489400243029E-03;
  auto c2 = -0.322396458041136;
  auto c3 = -2.40075827716184;
  auto c4 = -2.54973253934373;
  auto c5 = 4.37466414146497;
  auto c6 = 2.93816398269878;

  auto d1 = 7.78469570904146E-03;
  auto d2 = 0.32246712907004;
  auto d3 = 2.445134137143;
  auto d4 = 3.75440866190742;

  auto inverseCDF = 0.0;

  auto p_low = 0.02425;
  auto p_high = 1.0 - p_low;
  if (p < 0.0 or p > 1.0)
    throw std::runtime_error("p must be between 0.0 and 1.0");
  if (p == 0.0)
    p = 1e-10;

  if (p == 1.0)
    p = 1.0 - 1e-10;
  if (p < p_low) {
    auto q = sqrt(-2.0 * log(p));
    inverseCDF = (((((c1 * q + c2) * q + c3) * q + c4) * q + c5) * q + c6) / ((((d1 * q + d2) * q + d3) * q + d4) * q + 1.0);
  } else if (p <= p_high){
    auto q = p - 0.5;
    auto r = q * q;
    inverseCDF = (((((a1 * r + a2) * r + a3) * r + a4) * r + a5) * r + a6) * q / (((((b1 * r + b2) * r + b3) * r + b4) * r + b5) * r + 1.0);
  } else if (p < 1.0){
    auto q = sqrt(-2.0 * log(1 - p));
    inverseCDF = -(((((c1 * q + c2) * q + c3) * q + c4) * q + c5) * q + c6) / ((((d1 * q + d2) * q + d3) * q + d4) * q + 1.0);
  }
  return inverseCDF;
}

inline double uniform_to_default_time(double u, const std::vector<double>& times, const std::vector<double>& values) {
  if (u == 0.0)
    return 99999.0;
  if (u == 1.0)
    return 0.0;
  auto num_points = times.size();
  auto index = 0;
  for (auto i{1};i<num_points;++i){
    if (u <= values.at(i - 1) && u > values.at(i)){
      index = i;
      break;
    }
  }
  double tau = 0.0;
  if (index == num_points + 1) {
    auto t1 = times[num_points - 1];
    auto q1 = values[num_points - 1];
    auto t2 = times[num_points];
    auto q2 = values[num_points];
    auto lam = log(q1 / q2) / (t2 - t1);
    tau = t2 - log(u / q2) / lam;
  } else if (index == 0){
    auto t1 = times.back();
    auto q1 = values.back();
    auto t2 = times[index];
    auto q2 = values[index];
    tau = (t1 * log(q2 / u) + t2 * log(u / q1)) / log(q2 / q1);
  } else {
    auto t1 = times[index - 1];
    auto q1 = values[index - 1];
    auto t2 = times[index];
    auto q2 = values[index];
    tau = (t1 * log(q2 / u) + t2 * log(u / q1)) / log(q2 / q1);
  }
  return tau;
}

inline MatrixXd corr_matrix_generator(double rho, int n){
  MatrixXd corr_matrix = MatrixXd::Zero(n,n);
  for (int i{0}; i < n; ++i){
    corr_matrix(i, i) = 1.0;
    for (int j{0}; j < i; ++j){
      corr_matrix(i, j) = rho;
      corr_matrix(j, i) = rho;
    }
  }
  return corr_matrix;
}


#endif//FINPROJ_INCLUDE_FINPROJ_UTILS_MISC_H_
