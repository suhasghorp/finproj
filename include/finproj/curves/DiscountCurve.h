#ifndef FINPROJ_INCLUDE_FINPROJ_CURVES_DISCOUNTCURVE_H_
#define FINPROJ_INCLUDE_FINPROJ_CURVES_DISCOUNTCURVE_H_

#include <finproj/utils/ChronoDate.h>
#include <finproj/utils/Interpolator.h>
#include <finproj/utils/Calendar.h>
#include <finproj/utils/DayCount.h>
#include <vector>

class DiscountCurve {
 public:
  DiscountCurve() = default;
  DiscountCurve(const ChronoDate& valuation_date,
                const std::vector<ChronoDate>& df_dates,
                const std::vector<double>& df_values,
                InterpTypes interp_type = InterpTypes::FLAT_FWD_RATES);
  DiscountCurve(const ChronoDate& valuation_date,
                               FrequencyTypes freq_type = FrequencyTypes::ANNUAL,
                               DayCountTypes day_count_type = DayCountTypes::ACT_ACT_ISDA,
                               InterpTypes interp_type = InterpTypes::FLAT_FWD_RATES);
  std::vector<double> zero_to_df(const std::vector<double>& rates, const std::vector<double>& times,
                                FrequencyTypes freq_type) const;
  std::vector<double> df_to_zero(const std::vector<double>& dfs,const std::vector<ChronoDate>& dates,
                                FrequencyTypes freq_type, DayCountTypes day_count_type) const;
  std::vector<double> df_to_zero(const std::vector<double>& dfs,const std::vector<double>& times,
                                 FrequencyTypes freq_type) const;
  std::vector<double> df(const std::vector<ChronoDate>& dates);
  double df(const ChronoDate& date, DayCountTypes day_count_type = DayCountTypes::ACT_ACT_ISDA) const;
  double df(double time) const;
  std::vector<double> zero_rates(const std::vector<ChronoDate>& dates, FrequencyTypes freq_type,
                                DayCountTypes day_count_type);
  std::vector<double> cc_rates(const std::vector<ChronoDate>& dates,DayCountTypes day_count_type = DayCountTypes::SIMPLE);
  std::vector<double> swap_rates(const ChronoDate& effective_date, const std::vector<ChronoDate>& maturity_dates,
                                FrequencyTypes freq_type = FrequencyTypes::ANNUAL,
                                DayCountTypes day_count_type = DayCountTypes::THIRTY_E_360);
  std::vector<double> fwd(const std::vector<ChronoDate>& dates);
  std::vector<double> fwd(const std::vector<double>& times);
  double fwd_rate(const ChronoDate& start_date, const ChronoDate& date, DayCountTypes day_count_type = DayCountTypes::ACT_360);
  double fwd_rate(const ChronoDate& start_date, std::string& tenor, DayCountTypes day_count_type = DayCountTypes::ACT_360);

  ChronoDate valuation_date_{};
  std::vector<ChronoDate> df_dates_{};
  std::vector<double> df_values_{};
  InterpTypes interp_type_{};

  std::vector<double> dfs_{};
  std::vector<double> times_{};
  int num_points_{};
  FrequencyTypes freq_type_{};
  DayCountTypes day_count_type_{};
  Interpolator interpolator_{};


};

#endif//FINPROJ_INCLUDE_FINPROJ_CURVES_DISCOUNTCURVE_H_
