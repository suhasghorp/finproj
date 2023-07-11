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

inline double uniform_to_default_time(double u, const std::vector<double>& times, const std::vector<double>& values) {
  if (u == 0.0)
    return 99999.0;
  if (u == 1.0)
    return 0.0;
  size_t num_points = times.size();
  size_t index = 0;
  for (size_t i{1};i<num_points;++i){
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

inline std::vector<std::string> split(const std::string& str, const std::string& delim) {
  std::vector<std::string> tokens;
  size_t prev = 0, pos = 0;
  do
  {
    pos = str.find(delim, prev);
    if (pos == std::string::npos) pos = str.length();
    std::string token = str.substr(prev, pos-prev);
    if (!token.empty()) tokens.push_back(token);
    prev = pos + delim.length();
  }
  while (pos < str.length() && prev < str.length());
  return tokens;
}

inline double RationalApproximation(double t)
{
  // Abramowitz and Stegun formula 26.2.23.
  // The absolute value of the error should be less than 4.5 e-4.
  double c[] = {2.515517, 0.802853, 0.010328};
  double d[] = {1.432788, 0.189269, 0.001308};
  return t - ((c[2]*t + c[1])*t + c[0]) /
      (((d[2]*t + d[1])*t + d[0])*t + 1.0);
}

//https://www.johndcook.com/blog/normal_cdf_inverse/
inline double NormalCDFInverse(double p)
{
  if (p <= 0.0 || p >= 1.0)
  {
    std::stringstream os;
    os << "Invalid input argument (" << p
       << "); must be larger than 0 but less than 1.";
    throw std::invalid_argument( os.str() );
  }

  // See article above for explanation of this section.
  if (p < 0.5)
  {
    // F^-1(p) = - G^-1(p)
    return -RationalApproximation( sqrt(-2.0*log(p)) );
  }
  else
  {
    // F^-1(p) = G^-1(1-p)
    return RationalApproximation( sqrt(-2.0*log(1-p)) );
  }
}


#endif//FINPROJ_INCLUDE_FINPROJ_UTILS_MISC_H_
