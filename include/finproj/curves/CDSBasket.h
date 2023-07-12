#ifndef FINPROJ_INCLUDE_FINPROJ_CURVES_CDSBASKET_H_
#define FINPROJ_INCLUDE_FINPROJ_CURVES_CDSBASKET_H_
#include <Eigen/Dense>
#include <finproj/utils/ChronoDate.h>
#include <finproj/utils/Calendar.h>
#include <finproj/utils/DayCount.h>
#include <finproj/curves/CDS.h>
#include <tuple>
using namespace Eigen;

class CDSBasket {
 public:
  CDSBasket(const ChronoDate& step_in_date, const ChronoDate& maturity_date,
            double notional = 1000000.0, double running_coupon = 0.0, bool long_protection = true,
            FrequencyTypes freq_type = FrequencyTypes::QUARTERLY,
            DayCountTypes day_count_type = DayCountTypes::ACT_360,
            CalendarTypes cal_type = CalendarTypes::WEEKEND,
            BusDayAdjustTypes bus_day_adjust_type = BusDayAdjustTypes::FOLLOWING,
            DateGenRuleTypes date_gen_rule_type = DateGenRuleTypes::BACKWARD);
  std::tuple<double,double,double> value_gaussian_mc(const ChronoDate& valuation_date, int nth_to_default,
                                                       const std::vector<CreditCurve>& issuer_curves,
                                                       const MatrixXd& correlation_matrix,
                                                       const IborSingleCurve& libor_curve,
                                                       int num_trials,
                                                       int seed,
                                                       const std::string& random_number_generation) const;
  std::tuple<double,double,double> value_student_t_mc(const ChronoDate& valuation_date, int nth_to_default,
                                                       const std::vector<CreditCurve>& issuer_curves,
                                                       const MatrixXd& correlation_matrix,
                                                        const float degrees_of_freedom,
                                                       const IborSingleCurve& libor_curve,
                                                       int num_trials,
                                                       int seed,
                                                        const std::string& random_number_generation) const;
  std::tuple<double,double> value_legs_mc(const ChronoDate& valuation_date, int nth_to_default,
                                           const MatrixXd& default_times,
                                           const std::vector<CreditCurve>& issuer_curves,
                                           const IborSingleCurve& libor_curve) const;

 private:
  ChronoDate step_in_date_{};
  ChronoDate maturity_date_{};
  double notional_{}, running_coupon_{};
  bool long_protection_{};
  FrequencyTypes freq_type_{};
  DayCountTypes day_count_type_{};
  CalendarTypes cal_type_{};
  BusDayAdjustTypes bus_day_adjust_type_{};
  DateGenRuleTypes date_gen_rule_type_{};
  CDS cds_contract_;
};

#endif//FINPROJ_INCLUDE_FINPROJ_CURVES_CDSBASKET_H_
