#ifndef FINPROJ_INCLUDE_FINPROJ_CURVES_SWAPFLOATLEG_H_
#define FINPROJ_INCLUDE_FINPROJ_CURVES_SWAPFLOATLEG_H_
#include <finproj/utils/ChronoDate.h>
#include <string>
#include <finproj/utils/Calendar.h>
#include <finproj/utils/DayCount.h>
#include <finproj//curves/DiscountCurve.h>
#include <finproj/utils/SwapTypes.h>

class SwapFloatLeg{
 public:
  SwapFloatLeg() = default;
  SwapFloatLeg(const ChronoDate& eff_date, const ChronoDate& termination_date, SwapTypes leg_type,double spread,
               FrequencyTypes freq_type, DayCountTypes day_count_type,double notional = 1'000'000,
               double principal = 0.0,int payment_lag = 0,CalendarTypes cal_type = CalendarTypes::WEEKEND,
               BusDayAdjustTypes bus_day_adjust_type = BusDayAdjustTypes::FOLLOWING,
               DateGenRuleTypes date_gen_rule_type = DateGenRuleTypes::BACKWARD,
               bool end_of_month = false);
  SwapFloatLeg(const ChronoDate& eff_date, const std::string& tenor, SwapTypes leg_type,double spread,
               FrequencyTypes freq_type, DayCountTypes day_count_type,double notional = 1'000'000,
               double principal = 0.0,int payment_lag = 0,CalendarTypes cal_type = CalendarTypes::WEEKEND,
               BusDayAdjustTypes bus_day_adjust_type = BusDayAdjustTypes::FOLLOWING,
               DateGenRuleTypes date_gen_rule_type = DateGenRuleTypes::BACKWARD,
               bool end_of_month = false);
  std::vector<ChronoDate> generate_payment_dates() ;
  double value(const ChronoDate& val_date, const DiscountCurve& disc_curve,
              const DiscountCurve& index_curve,std::optional<double> first_fixing_rate) ;
  DayCountTypes get_day_count_type() const;

 private:
  ChronoDate eff_date_{},termination_date_{}, maturity_date_{};
  SwapTypes leg_type_{};
  double spread_{};
  FrequencyTypes freq_type_{};
  DayCountTypes day_count_type_{};
  double notional_{}, principal_{};
  int payment_lag_{};
  CalendarTypes cal_type_{};
  BusDayAdjustTypes bus_day_adjust_type_{};
  DateGenRuleTypes date_gen_rule_type_{};
  bool end_of_month_{};
  std::string tenor_{};
  std::vector<ChronoDate> payment_dates_{};
  std::vector<ChronoDate> start_accrual_dates_{},end_accrual_dates_{};
  std::vector<double> year_fracs_{},rates_{},payments_{}, payment_dfs_{}, payment_pvs_{}, cumulative_pvs_{}, notionals_{};
};

#endif//FINPROJ_INCLUDE_FINPROJ_CURVES_SWAPFLOATLEG_H_
