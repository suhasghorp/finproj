#ifndef FINPROJ_INCLUDE_FINPROJ_UTILS_INTERPOLATOR_H_
#define FINPROJ_INCLUDE_FINPROJ_UTILS_INTERPOLATOR_H_
#include <vector>
enum class InterpTypes {
  FLAT_FWD_RATES = 1,
  LINEAR_FWD_RATES = 2,
  LINEAR_ZERO_RATES = 4,
  FINCUBIC_ZERO_RATES = 7,
  NATCUBIC_LOG_DISCOUNT = 8,
  NATCUBIC_ZERO_RATES = 9,
  PCHIP_ZERO_RATES = 10,
  PCHIP_LOG_DISCOUNT = 11
};

class Interpolator{
 public:
  Interpolator(const std::vector<double>& times, const std::vector<double>& dfs, InterpTypes inter_type);
  Interpolator();
  Interpolator(InterpTypes inter_type);
  void fit(const std::vector<double>& times, const std::vector<double>& dfs);
  double interpolate(double t) const ;

 private:
  std::vector<double> times_{};
  std::vector<double> dfs_{};
  InterpTypes inter_type_;
  int num_points_{};
  double small = 1e-10;
};

#endif//FINPROJ_INCLUDE_FINPROJ_UTILS_INTERPOLATOR_H_
