#include "finproj/utils/Schedule.h"
#include <algorithm>
#include <cmath>
#include <finproj/curves/DiscountCurve.h>
#include <finproj/utils/Misc.h>
#include <tuple>
#include <ranges>

DiscountCurve::DiscountCurve(const ChronoDate& valuation_date,
              FrequencyTypes freq_type,
              DayCountTypes day_count_type,
              InterpTypes interp_type):
                                                        valuation_date_{valuation_date},
                                                        freq_type_{freq_type},
                                                        day_count_type_{day_count_type},
                                                        interp_type_{interp_type}{
  interpolator_ = Interpolator(interp_type_);

}

DiscountCurve::DiscountCurve(const ChronoDate& valuation_date,
                             const std::vector<ChronoDate>& df_dates,
                             const std::vector<double>& df_values,
                             InterpTypes interp_type):
                                                        valuation_date_{valuation_date},
                                                        df_dates_{df_dates},
                                                        df_values_{df_values},
                                                        interp_type_{interp_type}{
  if (df_dates_.empty())
    throw std::runtime_error("Times has zero length");
  if (df_dates_.size() != df_values_.size())
    throw std::runtime_error("Times and values have different lengths");
  times_.push_back(0.0);
  dfs_.push_back(1.0);
  num_points_ = df_values_.size();

  auto start_index = 0;
  if (num_points_ > 0) {
    if (df_dates_[0] == valuation_date_)
      dfs_[0] = df_values_[0];
      start_index = 1;
  }
  for (auto i = start_index; i < num_points_; ++i){
      auto t = (df_dates_[i] - valuation_date_) / 365.0;
      times_.push_back(t);
      dfs_.push_back(df_values_[i]);
  }
  auto i2 = std::adjacent_find(times_.begin(), times_.end(), std::greater_equal<double>());
  if (i2 != times_.end())
      throw  std::runtime_error("Times are not sorted in increasing order");
  freq_type_ = FrequencyTypes::CONTINUOUS;
  day_count_type_ = DayCountTypes::ACT_ACT_ISDA;
  interpolator_ = Interpolator(times_,dfs_,interp_type_);
}

std::vector<double> DiscountCurve::zero_to_df(const std::vector<double>& rates, const std::vector<double>& times,
                                             FrequencyTypes freq_type){
  std::vector<double> df{};
  df.reserve(rates.size());
  auto f = static_cast<int>(freq_type);
  for (int i = 0; i < rates.size(); ++i){
      auto t = fmax(times[i],gSmall );
      if (freq_type == FrequencyTypes::CONTINUOUS)
        df.push_back(-exp(rates[i] * t));
      else if (freq_type == FrequencyTypes::SIMPLE)
        df.push_back(1.0 / (1.0 + rates[i] * t));
      else if (freq_type == FrequencyTypes::ANNUAL ||
               freq_type == FrequencyTypes::SEMI_ANNUAL ||
               freq_type == FrequencyTypes::QUARTERLY ||
               freq_type == FrequencyTypes::MONTHLY){
        df.push_back(1.0 / pow(1.0 + rates[i] / f, f * t));
      } else throw std::runtime_error("Unknown frequency");
  }
  return df;
}

std::vector<double> DiscountCurve::df_to_zero(const std::vector<double>& dfs,const std::vector<ChronoDate>& dates,
                              FrequencyTypes freq_type, DayCountTypes day_count_type){
  auto f = static_cast<int>(freq_type);
  if (dfs.size() != dates.size())
      throw std::runtime_error("disc facrtors and dates have different sizes");
  auto num_dates = dates.size();
  std::vector<double> zero_rates{};
  zero_rates.reserve(num_dates);
  for (int i = 0; i < num_dates; ++i){
      auto year_frac = std::get<0>(DayCount(day_count_type).year_frac(valuation_date_,dates[i], FrequencyTypes::ANNUAL));
      auto t = fmax(year_frac,gSmall);
      if (freq_type == FrequencyTypes::CONTINUOUS){
        zero_rates.push_back(-log(dfs[i]) / t);
      } else if (freq_type == FrequencyTypes::SIMPLE){
        zero_rates.push_back((1.0 / dfs[i] - 1.0) / t);
      } else {
        zero_rates.push_back((pow(dfs[i], -1.0 / (t * f)) - 1.0) * f);
      }
  }
  return zero_rates;
}

double DiscountCurve::df(double time) const{
  double df{};
  if (interp_type_ == InterpTypes::FLAT_FWD_RATES ||
      interp_type_ == InterpTypes::LINEAR_ZERO_RATES ||
      interp_type_ == InterpTypes::LINEAR_FWD_RATES ||
      interp_type_ == InterpTypes::FINCUBIC_ZERO_RATES) {
      df = interpolator_.interpolate(time);
  } else {
      throw std::runtime_error("Interpolation type not supposrted");
  }
  return df;
}

std::vector<double> DiscountCurve::df(const std::vector<ChronoDate>& dates, DayCountTypes day_count_type){
  std::vector<double> dfv{};
  dfv.reserve(dates.size());
  for (int i = 0; i < dates.size(); ++i){
      auto f = df(dates[i]);
      dfv.push_back(f);
  }
  return dfv;
}

double DiscountCurve::df(const ChronoDate& date, DayCountTypes day_count_type) const{
  auto t = std::get<0>(DayCount(day_count_type).year_frac(valuation_date_,date, FrequencyTypes::ANNUAL));
  auto f = df(t);
  return f;
}

std::vector<double> DiscountCurve::zero_rates(const std::vector<ChronoDate>& dates, FrequencyTypes freq_type,
                              DayCountTypes day_count_type){
  auto dfs = df(dates,day_count_type);
  auto zeros = df_to_zero(dfs,dates,freq_type,day_count_type);
  return zeros;
}

std::vector<double> DiscountCurve::cc_rates(const std::vector<ChronoDate>& dates,DayCountTypes day_count_type){
  auto zeros = zero_rates(dates,FrequencyTypes::CONTINUOUS,day_count_type);
  return zeros;
}

std::vector<double> DiscountCurve::swap_rates(const ChronoDate& effective_date, const std::vector<ChronoDate>& maturity_dates,
                              FrequencyTypes freq_type,  DayCountTypes day_count_type){
  if (effective_date < valuation_date_)
      throw std::runtime_error("Swap starts before the curve valuation date");
  if (freq_type == FrequencyTypes::SIMPLE || freq_type == FrequencyTypes::CONTINUOUS)
      throw std::runtime_error("Cannot calculate par rate with continuous or simple freq");
  std::vector<double> par_rates{};
  for (auto mat_date : maturity_dates){
      if (mat_date <= effective_date)
        throw std::runtime_error("Maturity date is before the swap start date.");
      auto schedule = Schedule::create(effective_date, mat_date)
                                              .withFreqType(freq_type);
      auto flow_dates = schedule.get_schedule();
      flow_dates[0] = effective_date;
      auto day_cc = DayCount(day_count_type);
      auto prev_dt = flow_dates[0];
      auto pv01 = 0.0;
      auto discf = 1.0;
      double par_rate{0.0};
      for (auto next_dt : flow_dates | std::views::drop(1)) {
        discf = df(next_dt);
        auto alpha = std::get<0>(day_cc.year_frac(prev_dt, next_dt, FrequencyTypes::ANNUAL));
        pv01 += alpha * discf;
        prev_dt = next_dt;
      }
      if (abs(pv01) < gSmall) {
        par_rate = 0.0;
      }else {
        auto dfStart = df(effective_date);
        par_rate = (dfStart - discf) / pv01;
      }
      par_rates.push_back(par_rate);
  }
  return par_rates;
}

std::vector<double> DiscountCurve::fwd(const std::vector<ChronoDate>& dates){
  std::vector<double> fwds{};
  fwds.reserve(dates.size());
  auto dt = 1.0 / 365.0;
  for(auto d : dates){
      auto df1 = df(d);
      auto df2 = df(d.add_days(1));
      auto fwd = log(df1 / df2) / (1.0 * dt);
      fwds.push_back(fwd);
  }
  return fwds;
}

std::vector<double> DiscountCurve::fwd(const std::vector<double>& times){
  auto dt = 1e-6;
  std::vector<double> fwds{};
  fwds.reserve(times.size());
  for (auto t : times) {
      auto tt = fmax(t, dt);
      auto df1 = df(tt - dt);
      auto df2 = df(tt + dt);
      auto fwd = log(df1 / df2) / (2.0 * dt);
      fwds.push_back(fwd);
  }
  return fwds;
}

double DiscountCurve::fwd_rate(const ChronoDate& start_date, const ChronoDate& date, DayCountTypes day_count_type){
  auto yf = std::get<0>(DayCount(day_count_type).year_frac(start_date,date,FrequencyTypes::ANNUAL));
  auto df1 = df(start_date);
  auto df2 = df(date);
  auto fwd_rate = (df1 / df2 - 1.0) / yf;
  return fwd_rate;
}

double DiscountCurve::fwd_rate(const ChronoDate& start_date, std::string& tenor, DayCountTypes day_count_type){
  auto dt2 = start_date.add_tenor(tenor);
  return fwd_rate(start_date,dt2,day_count_type);
}
