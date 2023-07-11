#include <finproj/utils/Interpolator.h>
#include <libInterpolate/Interpolate.hpp>
#include <libInterpolate/AnyInterpolator.hpp>
#include <cmath>

Interpolator::Interpolator(const std::vector<double>& times, const std::vector<double>& dfs, InterpTypes inter_type):
times_{times},dfs_{dfs},inter_type_{inter_type}{
  num_points_ = times.size();
}

Interpolator::Interpolator(){}

Interpolator::Interpolator(InterpTypes inter_type):inter_type_{inter_type}{}

void Interpolator::fit(const std::vector<double>& times, const std::vector<double>& dfs){
  times_ = times;
  dfs_ = dfs;
  num_points_ = times_.size();
}

double Interpolator::interpolate(double t) const{
  double ret;

  if (t < small)
    return 1.0;

  if (t == times_[0])
    ret = dfs_[0];

  auto i = 0;
  while (times_[i] < t && i < num_points_ - 1)
    i = i + 1;
  if (t > times_[i])
    i = num_points_;

  if (inter_type_ == InterpTypes::LINEAR_ZERO_RATES){
    if (i == 1) {
      double r1 = -log(dfs_[i]) / times_[i];
      double r2 = -log(dfs_[i]) / times_[i];
      double dt = times_[i] - times_[i - 1];
      double rvalue = ((times_[i] - t) * r1 + (t - times_[i - 1]) * r2) / dt;
      ret = exp(-rvalue * t);
    } else if (i < num_points_){
      double r1 = -log(dfs_[i - 1]) / times_[i - 1];
      double r2 = -log(dfs_[i]) / times_[i];
      double dt = times_[i] - times_[i - 1];
      double rvalue = ((times_[i] - t) * r1 + (t - times_[i - 1]) * r2) / dt;
      ret = exp(-rvalue * t);
    } else {
      double r1 = -log(dfs_[i - 1]) / times_[i - 1];
      double r2 = -log(dfs_[i - 1]) / times_[i - 1];
      double dt = times_[i - 1] - times_[i - 2];
      double rvalue = ((times_[i - 1] - t) * r1 + (t - times_[i - 2]) * r2) / dt;
      ret = exp(-rvalue * t);
    }
  } else if (inter_type_ == InterpTypes::FLAT_FWD_RATES) {
    if (i == 1) {
      double rt1 = -log(dfs_[i - 1]);
      double rt2 = -log(dfs_[i]);
      double dt = times_[i] - times_[i - 1];
      double rtvalue = ((times_[i] - t) * rt1 + (t - times_[i - 1]) * rt2) / dt;
      ret = exp(-rtvalue);
    } else if (i < num_points_){
      double rt1 = -log(dfs_[i - 1]);
      double rt2 = -log(dfs_[i]);
      double dt = times_[i] - times_[i - 1];
      double rtvalue = ((times_[i] - t) * rt1 + (t - times_[i - 1]) * rt2) / dt;
      ret = exp(-rtvalue);
    } else {
      double rt1 = -log(dfs_[i - 2]);
      double rt2 = -log(dfs_[i - 1]);
      double dt = times_[i - 1] - times_[i - 2];
      double rtvalue = ((times_[i - 1] - t) * rt1 + (t - times_[i - 2]) * rt2) / dt;
      ret = exp(-rtvalue);
    }
  } else if (inter_type_ == InterpTypes::LINEAR_FWD_RATES) {
    if (i == 1) {
      double y2 = -log(dfs_[i] + small);
      double yvalue = t * y2 / (times_[i] + small);
      ret = exp(-yvalue);
    } else if (i < num_points_){
      double fwd1 = -log(dfs_[i - 1] / dfs_[i - 2]) / (times_[i - 1] - times_[i - 2]);
      double fwd2 = -log(dfs_[i] / dfs_[i - 1]) / (times_[i] - times_[i - 1]);
      double dt = times_[i] - times_[i - 1];
      double fwd = ((times_[i] - t) * fwd1 + (t - times_[i - 1]) * fwd2) / dt;
      ret = dfs_[i - 1] * exp(-fwd * (t - times_[i - 1]));
    } else {
      double fwd = -log(dfs_[i - 1] / dfs_[i - 2]) / (times_[i - 1] - times_[i - 2]);
      ret = dfs_[i - 1] * exp(-fwd * (t - times_[i - 1]));
    }
  } else if (inter_type_ == InterpTypes::FINCUBIC_ZERO_RATES) {
    std::vector<double> zero_rates{};
    for (size_t j{0}; j < dfs_.size();++j){
      zero_rates.push_back(-log(dfs_[j]) / (times_[j] + small));
    }
    if (times_[0] == 0.0)
      zero_rates[0] = zero_rates[1];

    _1D::CubicSplineInterpolator<double> interp;
    interp.setData( times_, zero_rates );
    ret = exp(-t * interp(t));
  }
  return ret;
}
