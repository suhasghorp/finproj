#ifndef FINPROJ_INCLUDE_FINPROJ_CURVES_CDS_H_
#define FINPROJ_INCLUDE_FINPROJ_CURVES_CDS_H_
#include <finproj/utils/ChronoDate.h>
#include <finproj/utils/SwapTypes.h>
#include <finproj/utils/Calendar.h>
#include <finproj/utils/DayCount.h>
#include <finproj/curves/CreditCurve.h>


class CDS {
 public:
  CDS(const ChronoDate& step_in_date,const ChronoDate& maturity_date, double running_coupon,
      double notional = 1'000'000.0, bool long_protection = true,
      FrequencyTypes freq_type = FrequencyTypes::QUARTERLY,
      DayCountTypes day_count_type = DayCountTypes::ACT_360,
      CalendarTypes cal_type = CalendarTypes::WEEKEND,
      BusDayAdjustTypes bus_day_adjust_type = BusDayAdjustTypes::FOLLOWING,
      DateGenRuleTypes date_gen_rule_type = DateGenRuleTypes::BACKWARD);
  CDS(const ChronoDate& step_in_date,const std::string&& tenor, double running_coupon,
      double notional = 1'000'000.0, bool long_protection = true,
      FrequencyTypes freq_type = FrequencyTypes::QUARTERLY,
      DayCountTypes day_count_type = DayCountTypes::ACT_360,
      CalendarTypes cal_type = CalendarTypes::WEEKEND,
      BusDayAdjustTypes bus_day_adjust_type = BusDayAdjustTypes::FOLLOWING,
      DateGenRuleTypes date_gen_rule_type = DateGenRuleTypes::BACKWARD);
  void generate_adjusted_cds_payment_dates();
  void calc_flows();
  ChronoDate get_maturity_date() const ;
  std::tuple<double,double> value(const ChronoDate& valuation_date, const CreditCurve& credit_curve,
                                   double recovery_rate, int pv01_method = 0, int prot_method = 0, int num_of_steps = 25);
  std::tuple<double,double> risky_pv01(const ChronoDate& valuation_date, const CreditCurve& credit_curve,int pv01_method) const;
  double protection_leg_pv(const ChronoDate& valuation_date, const CreditCurve& credit_curve,
                           double recovery_rate,int num_of_steps = 25,int prot_method = 0) const;
 private:
  ChronoDate step_in_date_{}, maturity_date_{};
  double running_coupon_{},notional_{};
  bool long_protection_{};
  FrequencyTypes freq_type_{};
  DayCountTypes day_count_type_{};
  CalendarTypes cal_type_{};
  BusDayAdjustTypes bus_day_adjust_type_{};
  DateGenRuleTypes date_gen_rule_type_{};
  std::vector<ChronoDate> adjusted_dates_{};
  std::vector<double> accrual_factors_{}, flows_{};
};

#endif//FINPROJ_INCLUDE_FINPROJ_CURVES_CDS_H_
