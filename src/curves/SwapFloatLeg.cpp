#include <finproj/curves/SwapFloatLeg.h>
#include <finproj/utils/Schedule.h>
#include <ranges>

SwapFloatLeg::SwapFloatLeg(const ChronoDate& eff_date, const ChronoDate& termination_date, SwapTypes leg_type,double spread,
                           FrequencyTypes freq_type, DayCountTypes day_count_type,double notional,
                           double principal,int payment_lag,CalendarTypes cal_type,
                           BusDayAdjustTypes bus_day_adjust_type,
                           DateGenRuleTypes date_gen_rule_type,
                           bool end_of_month):
                                                eff_date_{eff_date},termination_date_{termination_date},leg_type_{leg_type},spread_{spread},freq_type_{freq_type},
                                                day_count_type_{day_count_type},notional_{notional},principal_{principal},payment_lag_{payment_lag},cal_type_{cal_type},
                                                bus_day_adjust_type_{bus_day_adjust_type},date_gen_rule_type_{date_gen_rule_type},end_of_month_{end_of_month}
{
  Calendar calendar{cal_type_};
  maturity_date_ = calendar.adjust(termination_date_, bus_day_adjust_type_);
  if (eff_date_ > maturity_date_)
    throw std::runtime_error("Effective date after maturity date");
  payment_dates_ = generate_payment_dates();
}
SwapFloatLeg::SwapFloatLeg(const ChronoDate &eff_date, const std::string &tenor, SwapTypes leg_type, double spread,
                           FrequencyTypes freq_type, DayCountTypes day_count_type, double notional, double principal,
                           int payment_lag, CalendarTypes cal_type, BusDayAdjustTypes bus_day_adjust_type,
                           DateGenRuleTypes date_gen_rule_type, bool end_of_month):
eff_date_{eff_date},leg_type_{leg_type},tenor_{tenor},spread_{spread},freq_type_{freq_type},
day_count_type_{day_count_type},notional_{notional},principal_{principal},payment_lag_{payment_lag},cal_type_{cal_type},
bus_day_adjust_type_{bus_day_adjust_type},date_gen_rule_type_{date_gen_rule_type},end_of_month_{end_of_month}
{
  termination_date_ = eff_date_.add_tenor(tenor_);
  Calendar calendar{cal_type_};
  maturity_date_ = calendar.adjust(termination_date_, bus_day_adjust_type_);
  if (eff_date_ > maturity_date_)
    throw std::runtime_error("Effective date after maturity date");
  payment_dates_ = generate_payment_dates();
}

std::vector<ChronoDate> SwapFloatLeg::generate_payment_dates(){
  /** Generate the doubleing leg payment dates and accrual factors. The
coupons cannot be generated yet as we do not have the index curve. */

  Schedule schedule = Schedule::create(eff_date_,termination_date_)
                          .withFreqType(freq_type_)
                          .withCalendar(cal_type_)
                          .withBusDayAdjustType(bus_day_adjust_type_)
                          .withDateGenRuleType(date_gen_rule_type_)
                          .withEndOfMonth(end_of_month_);

  auto schedule_dates = schedule.get_schedule();
  if (schedule_dates.size() < 2)
    throw std::runtime_error("Schedule has none or only one date");
  auto prev_dt = schedule_dates[0];
  auto dc = DayCount{day_count_type_};
  auto calendar = Calendar{cal_type_};
  std::vector<unsigned int> accrued_days{};
  for (auto next_dt : schedule_dates | std::views::drop(1)){
    start_accrual_dates_.push_back(prev_dt);
    end_accrual_dates_.push_back(next_dt);
    auto payment_date = next_dt;
    if (payment_lag_ != 0)
      payment_date = calendar.add_business_days(next_dt, payment_lag_);
    payment_dates_.push_back(payment_date);
    std::tuple<double,unsigned int, unsigned int> temp = dc.year_frac(prev_dt,next_dt,FrequencyTypes::ANNUAL);
    year_fracs_.push_back(std::get<0>(temp));
    accrued_days.push_back(std::get<1>(temp));
    prev_dt = next_dt;
  }
  return payment_dates_;
}

double SwapFloatLeg::value(const ChronoDate& val_date, const DiscountCurve& disc_curve,
            const DiscountCurve& index_curve,std::optional<double> first_fixing_rate){
  auto df_value = disc_curve.df(val_date);
  double leg_pv{},df_pmnt{};
  ChronoDate pmnt_dt{};
  bool first_payment = false;
  auto num_payments = payment_dates_.size();
  auto index_basis = index_curve.day_count_type_;
  auto index_day_counter = DayCount(index_basis);
  for (size_t i{0}; i < num_payments; ++i){
    pmnt_dt = payment_dates_[i];
    if (pmnt_dt > val_date){
      auto start_accrued_dt = start_accrual_dates_[i];
      auto end_accrued_dt = end_accrual_dates_[i];
      auto pay_alpha = year_fracs_[i];
      std::tuple<double,unsigned int, unsigned int> temp = index_day_counter.year_frac(
          start_accrued_dt,end_accrued_dt,FrequencyTypes::ANNUAL);
      double fwd_rate{};
      if (!first_payment && first_fixing_rate.has_value()){
        fwd_rate = first_fixing_rate.value();
        first_payment = true;
      } else {
        auto df_start = index_curve.df(start_accrued_dt);
        auto df_end = index_curve.df(end_accrued_dt);
        fwd_rate = (df_start / df_end - 1.0) / std::get<0>(temp);
      }
      auto pmnt_amount = (fwd_rate + spread_) * pay_alpha * notional_;

      auto df_pmnt = disc_curve.df(pmnt_dt) / df_value;
      auto pmnt_pv = pmnt_amount * df_pmnt;
      leg_pv += pmnt_pv;

      rates_.push_back(fwd_rate);
      payments_.push_back(pmnt_amount);
      payment_dfs_.push_back(df_pmnt);
      payment_pvs_.push_back(pmnt_pv);
      cumulative_pvs_.push_back(leg_pv);
    } else {
      rates_.push_back(0.0);
      payments_.push_back(0.0);
      payment_dfs_.push_back(0.0);
      payment_pvs_.push_back(0.0);
      cumulative_pvs_.push_back(0.0);
    }
  }
  if (pmnt_dt > val_date){
    auto payment_pv = principal_ * df_pmnt * notional_;
    payment_pvs_.back() += payment_pv;
    leg_pv += payment_pv;
    cumulative_pvs_.back() = leg_pv;
  }
  if (leg_type_ == SwapTypes::PAY)
    leg_pv = (-1.0)*leg_pv;

  return leg_pv;
}

DayCountTypes SwapFloatLeg::get_day_count_type() const {
  return day_count_type_;
}

