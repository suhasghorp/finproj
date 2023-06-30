#include <finproj/curves/DiscountCurveZeros.h>
#include <tuple>
DiscountCuveZeros::DiscountCuveZeros(const ChronoDate& valuation_date,
                  const std::vector<ChronoDate>& zero_dates,
                  const std::vector<double>& zero_rates,
                  FrequencyTypes freq_type,
                  DayCountTypes day_count_type,
                  InterpTypes interp_type):
                  DiscountCurve(valuation_date,freq_type,day_count_type,interp_type),
                  zero_dates_{zero_dates},
                  zero_rates_{zero_rates}{
  if (zero_dates_.empty())
      throw std::runtime_error("Dates has zero length");

  if (zero_dates_.size() != zero_rates_.size())
      throw std::runtime_error("Dates and Rates are not the same length");

  for (auto d : zero_dates_){
      times_.push_back(std::get<0>(DayCount(day_count_type_).year_frac(valuation_date_, d, FrequencyTypes::ANNUAL)));
  }
  dfs_ = zero_to_df(zero_rates_,times_,freq_type_);
  interpolator_ = Interpolator(times_,dfs_,interp_type_);
}
