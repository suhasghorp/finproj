#ifndef FINPROJ_INCLUDE_FINPROJ_UTILS_MISC_H_
#define FINPROJ_INCLUDE_FINPROJ_UTILS_MISC_H_

#include <vector>

constexpr double gSmall = 1e-12;
constexpr double gdays_in_month = 365.242/12.0;

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

#endif//FINPROJ_INCLUDE_FINPROJ_UTILS_MISC_H_
