#ifndef FINPROJ_INCLUDE_FINPROJ_CURVES_CDSINDEXPORTFOLIO_H_
#define FINPROJ_INCLUDE_FINPROJ_CURVES_CDSINDEXPORTFOLIO_H_
#include <finproj/utils/Calendar.h>
#include <finproj/utils/DayCount.h>
#include <finproj/curves/CreditCurve.h>
class CDSIndexPortfolio{
 public:
  CDSIndexPortfolio(FrequencyTypes freq_type = FrequencyTypes::QUARTERLY,
                    DayCountTypes day_count_type = DayCountTypes::ACT_360,
                    CalendarTypes cal_type = CalendarTypes::WEEKEND,
                    DateGenRuleTypes date_gen_rule_type = DateGenRuleTypes::BACKWARD);
  double intrinsic_spread(const ChronoDate& valuation_date,const ChronoDate& step_in_date,const ChronoDate& maturity_date,
                          std::vector<CreditCurve>& issuer_curves) const;
  double intrinsic_protection_leg_pv(const ChronoDate& valuation_date,const ChronoDate& step_in_date,const ChronoDate& maturity_date,
                                     std::vector<CreditCurve>& issuer_curves) const;
  double intrinsic_rpv01(const ChronoDate& valuation_date,const ChronoDate& step_in_date,const ChronoDate& maturity_date,
                         std::vector<CreditCurve>& issuer_curves) const;

 private:
  FrequencyTypes freq_type_{};
  DayCountTypes day_count_type_{};
  CalendarTypes cal_type_{};
  DateGenRuleTypes date_gen_rule_type_{};
};

#endif//FINPROJ_INCLUDE_FINPROJ_CURVES_CDSINDEXPORTFOLIO_H_
