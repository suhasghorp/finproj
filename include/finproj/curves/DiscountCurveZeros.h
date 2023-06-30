#ifndef FINPROJ_INCLUDE_FINPROJ_CURVES_DISCOUNTCURVEZEROS_H_
#define FINPROJ_INCLUDE_FINPROJ_CURVES_DISCOUNTCURVEZEROS_H_

#include <finproj/curves/DiscountCurve.h>
#include <vector>

class DiscountCuveZeros : public DiscountCurve {
 public:
  DiscountCuveZeros(const ChronoDate& valuation_date,
                    const std::vector<ChronoDate>& zero_dates,
                    const std::vector<double>& zero_rates,
                    FrequencyTypes freq_type = FrequencyTypes::ANNUAL,
                    DayCountTypes day_count_type = DayCountTypes::ACT_ACT_ISDA,
                    InterpTypes interp_type = InterpTypes::FLAT_FWD_RATES);
 private:
  std::vector<ChronoDate> zero_dates_{};
  std::vector<double> zero_rates_{};
  std::vector<double> times_{};
};
#endif//FINPROJ_INCLUDE_FINPROJ_CURVES_DISCOUNTCURVEZEROS_H_
