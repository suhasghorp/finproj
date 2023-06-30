#include <finproj/curves/CDS.h>
#include <tuple>
#include <cmath>

CDS::CDS(const ChronoDate &step_in_date, const ChronoDate &maturity_date, double running_coupon, double notional,
         bool long_protection, FrequencyTypes freq_type, DayCountTypes day_count_type, CalendarTypes cal_type,
         BusDayAdjustTypes bus_day_adjust_type, DateGenRuleTypes date_gen_rule_type):
step_in_date_{step_in_date},maturity_date_{maturity_date},running_coupon_{running_coupon},notional_{notional},
long_protection_{long_protection},freq_type_{freq_type},day_count_type_{day_count_type},cal_type_{cal_type},
bus_day_adjust_type_{bus_day_adjust_type},date_gen_rule_type_{date_gen_rule_type}
{
  if (step_in_date_ > maturity_date_){
    throw std::runtime_error("Step in date after maturity date");
  }
  generate_adjusted_cds_payment_dates();
  calc_flows();
}

CDS::CDS(const ChronoDate& step_in_date,const std::string&& tenor, double running_coupon,
         double notional, bool long_protection,
         FrequencyTypes freq_type,
         DayCountTypes day_count_type,
         CalendarTypes cal_type,
         BusDayAdjustTypes bus_day_adjust_type,
         DateGenRuleTypes date_gen_rule_type)
{
  auto maturity_date = step_in_date.add_tenor(tenor);
  maturity_date = maturity_date.next_cds_date();
  new (this) CDS(step_in_date,maturity_date,running_coupon,notional,long_protection,freq_type,day_count_type,
  cal_type,bus_day_adjust_type,date_gen_rule_type);
}

void CDS::generate_adjusted_cds_payment_dates(){
  auto frequency = static_cast<int>(freq_type_);
  Calendar calendar{cal_type_};
  auto start_date = step_in_date_;
  auto end_date = maturity_date_;
  auto num_months = int(12.0 / frequency);
  std::vector<ChronoDate> unadjusted_schedule_dates {};
  if (date_gen_rule_type_ == DateGenRuleTypes::BACKWARD){
    auto next_date = end_date;
    auto flow_num = 0;
    while (next_date > start_date){
      unadjusted_schedule_dates.push_back(next_date);
      next_date = next_date.add_months(-num_months);
      ++flow_num;
    }
    unadjusted_schedule_dates.push_back(next_date);
    ++flow_num;
    // reverse order
    for (int i{0}; i < flow_num;++i){
      auto dt = unadjusted_schedule_dates[flow_num - i - 1];
      adjusted_dates_.push_back(dt);
    }
    //holiday adjust
    for (int i{0}; i < flow_num - 1;++i){
      auto dt = calendar.adjust(adjusted_dates_.at(i), bus_day_adjust_type_);
      adjusted_dates_.at(i) = dt;
    }
    auto final_date = adjusted_dates_.at(flow_num - 1);
    adjusted_dates_.at(flow_num - 1) = final_date.add_days(1);
  } else if (date_gen_rule_type_ == DateGenRuleTypes::FORWARD){
    auto next_date = start_date;
    auto flow_num = 0;
    unadjusted_schedule_dates.push_back(next_date);
    flow_num = 1;
    while (next_date < end_date){
      unadjusted_schedule_dates.push_back(next_date);
      next_date = next_date.add_months(num_months);
      ++flow_num;
    }
    for (int i{1}; i < flow_num;++i){
      auto dt = calendar.adjust(unadjusted_schedule_dates.at(i),bus_day_adjust_type_);
      adjusted_dates_.push_back(dt);
    }
    auto final_date = end_date.add_days(1);
    adjusted_dates_.push_back(final_date);
  }
}

void CDS::calc_flows() {
  auto payment_dates = adjusted_dates_;
  auto day_count = DayCount(day_count_type_);
  accrual_factors_.push_back(0.0);
  flows_.push_back(0.0);
  auto num_flows = payment_dates.size();
  for (int i{1};i<num_flows;++i){
    auto t0 = payment_dates.at(i - 1);
    auto t1 = payment_dates.at(i);
    auto accrual_factor = std::get<0>(day_count.year_frac(t0, t1, FrequencyTypes::ANNUAL));
    auto flow = accrual_factor * running_coupon_ * notional_;
    accrual_factors_.push_back(accrual_factor);
    flows_.push_back(flow);
  }
}

std::tuple<double,double> CDS::risky_pv01(const ChronoDate& valuation_date, const CreditCurve& credit_curve,int pv01_method) const{
  std::vector<double> payment_times{};
  for (int i{0};i < adjusted_dates_.size(); ++i){
    auto t = (adjusted_dates_.at(i) - valuation_date)/365.0;
    payment_times.push_back(t);
  }
  auto pcd = adjusted_dates_.at(0);
  auto eff = step_in_date_;
  auto day_count = DayCount(day_count_type_);
  auto accrual_factorPCDToNow = std::get<0>(day_count.year_frac(pcd, eff, FrequencyTypes::ANNUAL));
  auto year_fracs = accrual_factors_;
  auto teff = (eff - valuation_date) / 365.0;

  auto couponAccruedIndicator = 1;
  auto tncd = payment_times.at(1);
  auto credit_interp = Interpolator(credit_curve.times_,credit_curve.values_,InterpTypes::FLAT_FWD_RATES);
  auto rates_interp = Interpolator(credit_curve.libor_curve_.times_,credit_curve.libor_curve_.dfs_,InterpTypes::FLAT_FWD_RATES);
  auto qeff = credit_interp.interpolate(teff);
  auto q1 = credit_interp.interpolate(tncd);
  auto z1 = rates_interp.interpolate(tncd);
  auto full_rpv01 = q1 * z1 * year_fracs.at(1);
  full_rpv01 = full_rpv01 + z1 * (qeff - q1) * accrual_factorPCDToNow * couponAccruedIndicator;
  full_rpv01 += 0.5 * z1 * (qeff - q1) * (year_fracs.at(1) - accrual_factorPCDToNow) * couponAccruedIndicator;
  for (int i{2}; i<payment_times.size();++i){
    auto t2 = payment_times.at(i);
    auto q2 = credit_interp.interpolate(t2);
    auto z2 = rates_interp.interpolate(t2);
    auto accrual_factor = year_fracs.at(i);
    full_rpv01 += q2 * z2 * accrual_factor;
    auto tau = accrual_factor;
    auto h12 = -log(q2 / q1) / tau;
    auto r12 = -log(z2 / z1) / tau;
    auto alpha = h12 + r12;
    auto expTerm = 1.0 - exp(-alpha * tau) - alpha * tau * exp(-alpha * tau) ;
    auto dfull_rpv01 = q1 * z1 * h12 * expTerm / fabs(alpha * alpha + 1e-20);
    full_rpv01 = full_rpv01 + dfull_rpv01;
    q1 = q2;
  }
  auto clean_rpv01 = full_rpv01 - accrual_factorPCDToNow;
  return {full_rpv01,clean_rpv01};
}

double CDS::protection_leg_pv(const ChronoDate& valuation_date, const CreditCurve& credit_curve,
                         double recovery_rate,int num_of_steps,int prot_method) const
{
  auto teff = (step_in_date_ - valuation_date) / 365.0;
  auto tmat = (maturity_date_ - valuation_date) / 365.0;
  auto dt = (tmat - teff) / num_of_steps;
  auto t = teff;
  auto credit_interp = Interpolator(credit_curve.times_,credit_curve.values_,InterpTypes::FLAT_FWD_RATES);
  auto rates_interp = Interpolator(credit_curve.libor_curve_.times_,credit_curve.libor_curve_.dfs_,InterpTypes::FLAT_FWD_RATES);
  auto z1 = rates_interp.interpolate(t);
  auto q1 = credit_interp.interpolate(t);
  auto prot_pv = 0.0;
  auto small = 1e-8;
  for (int i{0}; i < num_of_steps;++i){
    t = t + dt;
    auto z2 = rates_interp.interpolate(t);
    auto q2 = credit_interp.interpolate(t);
    auto h12 = -log(q2 / q1) / dt;
    auto r12 = -log(z2 / z1) / dt;
    auto expTerm = exp(-(r12 + h12) * dt);
    auto dprot_pv = h12 * (1.0 - expTerm) * q1 * z1 / (fabs(h12 + r12) + small);
    prot_pv += dprot_pv;
    q1 = q2;
    z1 = z2;
  }
  prot_pv = prot_pv * (1.0 - recovery_rate);
  return prot_pv * notional_;
}

std::tuple<double,double> CDS::value(const ChronoDate& valuation_date, const CreditCurve& credit_curve,
                                 double recovery_rate, int pv01_method, int prot_method, int num_of_steps)
{
  auto rpv01 = risky_pv01(valuation_date,credit_curve,pv01_method);
  auto full_rpv01 = std::get<0>(rpv01);
  auto clean_rpv01 = std::get<1>(rpv01);
  auto prot_pv = protection_leg_pv(valuation_date,credit_curve,recovery_rate,num_of_steps,prot_method);
  auto fwd_df = 1.0;
  auto long_prot = long_protection_ ? 1 : -1;
  auto full_pv = fwd_df * long_prot * (prot_pv - running_coupon_ * full_rpv01 * notional_);
  auto clean_pv = fwd_df * long_prot * (prot_pv - running_coupon_ * clean_rpv01 * notional_);
  return {full_pv, clean_pv};
}

double CDS::par_spread(const ChronoDate& valuation_date, const CreditCurve& credit_curve,double recovery_rate, int pv01_method,
                  int prot_method, int num_of_steps) const
{
  auto rpv01 = risky_pv01(valuation_date,credit_curve,pv01_method);
  auto clean_rpv01 = std::get<1>(rpv01);
  auto prot_pv = protection_leg_pv(valuation_date,credit_curve,recovery_rate,num_of_steps,prot_method);
  auto spd = prot_pv / clean_rpv01 / notional_;
  return spd;
}

double CDS::clean_price(const ChronoDate& valuation_date, const CreditCurve& credit_curve,double recovery_rate,
                        int pv01_method, int prot_method, int num_of_steps) const
{
  auto rpv01 = risky_pv01(valuation_date,credit_curve,pv01_method);
  auto clean_rpv01 = std::get<1>(rpv01);
  auto prot_pv = protection_leg_pv(valuation_date,credit_curve,recovery_rate,num_of_steps,prot_method);
  auto fwd_df = 1.0;
  auto clean_pv = fwd_df * (prot_pv - running_coupon_ * clean_rpv01 * notional_);
  auto clean_price = (notional_ - clean_pv) / notional_ * 100.0;
  return clean_price;
}

unsigned int CDS::accrued_days() const{
    auto pcd = adjusted_dates_.at(0);
    unsigned int accrued_days = (step_in_date_ - pcd);
    return accrued_days;
}

double CDS::accrued_interest() const {
    auto day_count = DayCount(day_count_type_);
    auto pcd = adjusted_dates_.at(0);
    auto accrual_factor = std::get<0>(day_count.year_frac(pcd, step_in_date_, FrequencyTypes::ANNUAL));
    auto accrued_interest = accrual_factor * notional_ * running_coupon_;
    auto long_prot = long_protection_ ? -1 : 1;
    return long_prot * accrued_interest;
}

double CDS::premium_leg_pv(const ChronoDate& valuation_date, const CreditCurve& credit_curve,
                      int pv01_method) const
{
    auto rpv01 = risky_pv01(valuation_date,credit_curve,pv01_method);
    auto full_rpv01 = std::get<0>(rpv01);
    auto v = full_rpv01 * notional_ * running_coupon_;
    return v;
}

std::tuple<double,double,double,double> CDS::value_fast_approx(const ChronoDate& valuation_date, double flat_cont_int_rate, double flat_cds_curve_spread,
                                                     double curve_rec_rate, double contract_rec_rate) const
{
    auto t_mat = (maturity_date_ - valuation_date) / 365.0;
    auto t_eff = (step_in_date_ - valuation_date) / 365.0;
    auto h = flat_cds_curve_spread / (1.0 - curve_rec_rate);
    auto r = flat_cont_int_rate;
    auto fwd_df = 1.0;
    auto bump_size = 0.0001;
    auto long_prot = long_protection_ ? 1 : -1;
    auto accrued = accrued_interest();

    auto w = r + h;
    auto z = exp(-w * t_eff) - exp(-w * t_mat);
    auto clean_rpv01 = (z / w) * 365.0 / 360.0;
    auto prot_pv = h * (1.0 - contract_rec_rate) * (z / w) * notional_;
    auto clean_pv = fwd_df * long_prot * (prot_pv - running_coupon_ * clean_rpv01 * notional_);
    auto full_pv = clean_pv + fwd_df * accrued;

    //bump CDS spread and calculate

    h = (flat_cds_curve_spread + bump_size) / (1.0 - contract_rec_rate);
    w = r + h;
    z = exp(-w * t_eff) - exp(-w * t_mat);
    clean_rpv01 = (z / w) * 365.0 / 360.0;
    prot_pv = h * (1.0 - contract_rec_rate) * (z / w) * notional_;
    auto clean_pv_credit_bumped = fwd_df * long_prot * (prot_pv - running_coupon_ * clean_rpv01 * notional_);
    auto full_pv_credit_bumped = clean_pv_credit_bumped + fwd_df * long_prot * accrued;
    auto credit01 = full_pv_credit_bumped - full_pv;

    //bump Rate and calculate

    h = flat_cds_curve_spread / (1.0 - contract_rec_rate);
    r = flat_cont_int_rate + bump_size;
    w = r + h;
    z = exp(-w * t_eff) - exp(-w * t_mat);
    clean_rpv01 = (z / w) * 365.0 / 360.0;
    prot_pv = h * (1.0 - contract_rec_rate) * (z / w) * notional_;
    auto clean_pv_ir_bumped = fwd_df * long_prot * (prot_pv - running_coupon_ * clean_rpv01 * notional_);
    auto full_pv_ir_bumped = clean_pv_ir_bumped + fwd_df * long_prot * accrued;
    auto ir01 = full_pv_ir_bumped - full_pv;

    return {full_pv, clean_pv, credit01, ir01};
}

ChronoDate CDS::get_maturity_date() const {
  return maturity_date_;
}