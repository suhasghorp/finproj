#ifndef FINPROJ_INCLUDE_FINPROJ_CURVES_IBORSWAP_H_
#define FINPROJ_INCLUDE_FINPROJ_CURVES_IBORSWAP_H_
#include <finproj/utils/ChronoDate.h>
#include <finproj/utils/SwapTypes.h>
#include <finproj/utils/Calendar.h>
#include <finproj/utils/DayCount.h>
#include <finproj/curves/DiscountCurve.h>
#include <finproj/curves/SwapFixedLeg.h>
#include <finproj/curves/SwapFloatLeg.h>

class IborSwap {
 public:
  IborSwap(const ChronoDate& eff_date, const ChronoDate& termination_date,
           SwapTypes fixed_leg_type,double fixed_coupon,FrequencyTypes fixed_freq_type,
           DayCountTypes fixed_day_count_type,
           double notional = 1'000'000,double double_spread = 0.0,
           FrequencyTypes double_freq_type = FrequencyTypes::QUARTERLY,
           DayCountTypes double_day_count_type = DayCountTypes::THIRTY_E_360,
           CalendarTypes cal_type = CalendarTypes::WEEKEND,
           BusDayAdjustTypes bus_day_adjust_type = BusDayAdjustTypes::FOLLOWING,
           DateGenRuleTypes date_gen_rule_type = DateGenRuleTypes::BACKWARD);
  IborSwap(const ChronoDate& eff_date, std::string&& tenor,
           SwapTypes fixed_leg_type,double fixed_coupon,FrequencyTypes fixed_freq_type,
           DayCountTypes fixed_day_count_type,
           double notional = 1'000'000,double double_spread = 0.0,
           FrequencyTypes double_freq_type = FrequencyTypes::QUARTERLY,
           DayCountTypes double_day_count_type = DayCountTypes::THIRTY_E_360,
           CalendarTypes cal_type = CalendarTypes::WEEKEND,
           BusDayAdjustTypes bus_day_adjust_type = BusDayAdjustTypes::FOLLOWING,
           DateGenRuleTypes date_gen_rule_type = DateGenRuleTypes::BACKWARD);
  double value(const ChronoDate& val_date, const DiscountCurve& disc_curve,std::optional<DiscountCurve> index_curve,
              std::optional<double> first_fixing_rate);
  double pv01(const ChronoDate& val_date,const DiscountCurve& disc_curve) ;
  double swap_rate(const ChronoDate& val_date, const DiscountCurve& disc_curve,std::optional<DiscountCurve>& index_curve,
              std::optional<double> first_fixing);
  SwapFixedLeg get_fixed_leg() const;
  SwapFloatLeg get_double_leg() const;
  ChronoDate get_eff_date() const;
  ChronoDate get_maturity_date() const;

 private:
  ChronoDate eff_date_{},termination_date_{}, maturity_date_{};
  std::string tenor_{};
  SwapTypes fixed_leg_type_{},double_leg_type_{};
  double fixed_coupon_{}, notional_{}, double_spread_{};
  FrequencyTypes fixed_freq_type_{},double_freq_type_{};
  DayCountTypes fixed_day_count_type_{},double_day_count_type_{};
  CalendarTypes cal_type_{};
  BusDayAdjustTypes bus_day_adjust_type_{};
  DateGenRuleTypes date_gen_rule_type_{};
  SwapFixedLeg fixed_leg_; SwapFloatLeg double_leg_;

};

#endif//FINPROJ_INCLUDE_FINPROJ_CURVES_IBORSWAP_H_
