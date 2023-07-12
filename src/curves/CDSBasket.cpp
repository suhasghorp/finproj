#include <finproj/curves/CDSBasket.h>
#include <finproj/models/StudentTCopula.h>
#include <finproj/models/GaussCopula.h>

CDSBasket::CDSBasket(const ChronoDate &step_in_date, const ChronoDate &maturity_date, double notional, double running_coupon,
                     bool long_protection, FrequencyTypes freq_type, DayCountTypes day_count_type, CalendarTypes cal_type,
                     BusDayAdjustTypes bus_day_adjust_type, DateGenRuleTypes date_gen_rule_type) :
 step_in_date_{step_in_date},maturity_date_{maturity_date},notional_{notional},running_coupon_{running_coupon},
 long_protection_{long_protection},freq_type_{freq_type},day_count_type_{day_count_type},cal_type_{cal_type},
 bus_day_adjust_type_{bus_day_adjust_type},date_gen_rule_type_{date_gen_rule_type}
{
  cds_contract_ = CDS(step_in_date_,maturity_date_,running_coupon_,1.0, long_protection_,
                      freq_type_, day_count_type_, cal_type_,bus_day_adjust_type_,date_gen_rule_type_);
}

std::tuple<double,double,double> CDSBasket::value_gaussian_mc(const ChronoDate& valuation_date, int nth_to_default,
                                                     const std::vector<CreditCurve>& issuer_curves,
                                                     const MatrixXd& correlation_matrix,
                                                     const IborSingleCurve& libor_curve,
                                                     int num_trials,
                                                     int seed,
                                                     const std::string& random_number_generation) const
{
  int num_credits = issuer_curves.size();
  if (nth_to_default > num_credits or nth_to_default < 1)
    throw std::runtime_error("nToDefault must be 1 to num_credits");
  auto default_times = GaussCopula().default_times_gc(issuer_curves,correlation_matrix,num_trials,seed, random_number_generation);
  auto [rpv01, prot_pv] = value_legs_mc(valuation_date,nth_to_default,default_times,issuer_curves,libor_curve);
  auto spd = prot_pv / rpv01;
  auto value = notional_ * (prot_pv - running_coupon_ * rpv01);
  if (!long_protection_)
    value = value * -1.0;
  return {value, rpv01, spd};
}

std::tuple<double,double,double> CDSBasket::value_student_t_mc(const ChronoDate& valuation_date, int nth_to_default,
                                                      const std::vector<CreditCurve>& issuer_curves,
                                                      const MatrixXd& correlation_matrix,
                                                      const float degrees_of_freedom,
                                                      const IborSingleCurve& libor_curve,
                                                      int num_trials,
                                                      int seed,
                                                      const std::string& random_number_generation) const
{
  int num_credits = issuer_curves.size();
  if (nth_to_default > num_credits or nth_to_default < 1)
    throw std::runtime_error("nToDefault must be 1 to num_credits");
  auto default_times = StudentTCopula().default_times(issuer_curves,correlation_matrix,degrees_of_freedom,num_trials,seed,random_number_generation);
  auto [rpv01, prot_pv] = value_legs_mc(valuation_date,nth_to_default,default_times,issuer_curves,libor_curve);
  auto spd = prot_pv / rpv01;
  auto value = notional_ * (prot_pv - running_coupon_ * rpv01);
  if (!long_protection_)
    value = value * -1.0;
  return {value, rpv01, spd};
}

std::tuple<double,double> CDSBasket::value_legs_mc(const ChronoDate& valuation_date, int nth_to_default,
                                         const MatrixXd& default_times,
                                         const std::vector<CreditCurve>& issuer_curves,
                                         const IborSingleCurve& libor_curve) const
{
  auto num_credits = default_times.rows();
  auto num_trials = default_times.cols();

  auto adjusted_dates = cds_contract_.get_adjusted_dates();
  int num_flows = adjusted_dates.size();
  DayCount day_count{DayCount(day_count_type_)};
  double averageAccrualFactor = 0.0;
  std::vector<double> rpv01_to_times(num_flows,0.0);
  for (int it{1}; it < num_flows; ++it){
    auto t = (adjusted_dates[it] - valuation_date) / 365.0;
    auto dt0 = adjusted_dates[it - 1];
    auto dt1 = adjusted_dates[it];
    auto accrual_factor = std::get<0>(day_count.year_frac(dt0, dt1, FrequencyTypes::ANNUAL));
    averageAccrualFactor += accrual_factor;
    rpv01_to_times[it] = (rpv01_to_times[it - 1] + accrual_factor * libor_curve.df(t));
  }
  averageAccrualFactor /= num_flows;
  auto tmat = (maturity_date_ - valuation_date) / 365.0;
  auto rpv01 = 0.0;
  auto prot = 0.0;
  std::vector<double> assetTau(num_credits, 0.0);
  for (auto trial{0}; trial < num_trials; ++trial){
    for (auto credit{0}; credit < num_credits; ++credit){
      assetTau[credit] = default_times(credit, trial);
    }
    std::sort(assetTau.begin(), assetTau.end());
    //GET THE Nth DEFAULT TIME
    auto minTau = assetTau[nth_to_default - 1];
    auto rpv01Trial{0.0}, protTrial{0.0};
    if (minTau < tmat){
      auto numPaymentsIndex = int(minTau / averageAccrualFactor);
      rpv01Trial = rpv01_to_times[numPaymentsIndex];
      rpv01Trial += (minTau - numPaymentsIndex * averageAccrualFactor);

      //DETERMINE IDENTITY OF N-TO-DEFAULT CREDIT IF BASKET NOT HOMO
      auto assetIndex = 0;
      for (auto credit{0}; credit < num_credits; ++credit) {
        if (minTau == default_times(credit, trial)) {
          assetIndex = credit;
          break;
        }
      }
      protTrial = (1.0 - issuer_curves[assetIndex].recovery_rate_);
      protTrial *= libor_curve.df(minTau);
    } else {
      auto numPaymentsIndex = int(tmat / averageAccrualFactor);
      rpv01Trial = rpv01_to_times[numPaymentsIndex];
      protTrial = 0.0;
    }
    rpv01 += rpv01Trial;
    prot += protTrial;
  }
  rpv01 = rpv01 / num_trials;
  prot = prot / num_trials;
  return {rpv01,prot};
}
