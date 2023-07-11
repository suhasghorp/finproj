#include <finproj/curves/IborSwap.h>
#include <finproj/utils/Misc.h>
#include <cmath>

IborSwap::IborSwap(const ChronoDate& eff_date, const ChronoDate& termination_date,
                   SwapTypes fixed_leg_type,double fixed_coupon,FrequencyTypes fixed_freq_type,
                   DayCountTypes fixed_day_count_type,
                   double notional,double double_spread,
                   FrequencyTypes double_freq_type,
                   DayCountTypes double_day_count_type,
                   CalendarTypes cal_type,
                   BusDayAdjustTypes bus_day_adjust_type,
                   DateGenRuleTypes date_gen_rule_type):
eff_date_{eff_date},termination_date_{termination_date},fixed_leg_type_{fixed_leg_type},fixed_coupon_{fixed_coupon},
notional_{notional},double_spread_{double_spread},fixed_freq_type_{fixed_freq_type},double_freq_type_{double_freq_type},
fixed_day_count_type_{fixed_day_count_type},double_day_count_type_{double_day_count_type},cal_type_{cal_type},bus_day_adjust_type_{bus_day_adjust_type},
date_gen_rule_type_{date_gen_rule_type}
{
  Calendar calendar{cal_type};
  maturity_date_ = calendar.adjust(termination_date_, bus_day_adjust_type_);
  if (eff_date_ > maturity_date_)
    throw std::runtime_error("Effective date after maturity date");
  double_leg_type_ = SwapTypes::PAY;
  if (fixed_leg_type_ == SwapTypes::PAY)
    double_leg_type_ = SwapTypes::RECEIVE;
  auto payment_lag = 0;
  auto principal = 0.0;
  fixed_leg_ = SwapFixedLeg{eff_date_, termination_date_,fixed_leg_type_,
  fixed_coupon_,fixed_freq_type_,fixed_day_count_type_,notional_,
                            principal,payment_lag,cal_type_,bus_day_adjust_type_,date_gen_rule_type_};

  double_leg_ = SwapFloatLeg{eff_date_, termination_date_, double_leg_type_,double_spread_,double_freq_type_,
  double_day_count_type_,notional_,principal,payment_lag,cal_type_,bus_day_adjust_type_,date_gen_rule_type_};
}

IborSwap::IborSwap(const ChronoDate& eff_date, std::string&& tenor,
                   SwapTypes fixed_leg_type,double fixed_coupon,FrequencyTypes fixed_freq_type,
                   DayCountTypes fixed_day_count_type,
                   double notional,double double_spread,
                   FrequencyTypes double_freq_type,
                   DayCountTypes double_day_count_type,
                   CalendarTypes cal_type,
                   BusDayAdjustTypes bus_day_adjust_type,
                   DateGenRuleTypes date_gen_rule_type)
{

  auto termination_date = eff_date.add_tenor(tenor);
  new (this) IborSwap(eff_date,termination_date,fixed_leg_type,fixed_coupon,fixed_freq_type,fixed_day_count_type,
                      notional,double_spread,double_freq_type,double_day_count_type,cal_type,
                      bus_day_adjust_type,date_gen_rule_type);
}

double IborSwap::value(const ChronoDate& val_date, const DiscountCurve& disc_curve,std::optional<DiscountCurve> index_curve,
            std::optional<double> first_fixing_rate){

  DiscountCurve idx_curve{};
  if (!index_curve.has_value())
    idx_curve = disc_curve;
  else idx_curve = index_curve.value();

  auto fixed_leg_value = fixed_leg_.value(val_date,disc_curve);
  auto double_leg_value = double_leg_.value(val_date,disc_curve,idx_curve,first_fixing_rate);
  auto value = fixed_leg_value + double_leg_value;
  return value;
}

double IborSwap::pv01(const ChronoDate& val_date,const DiscountCurve& disc_curve)  {
  /** Calculate the value of 1 basis point coupon on the fixed leg.*/
  auto pv = fixed_leg_.value(val_date, disc_curve);
  auto pv01 = pv / fixed_leg_.get_coupon() / fixed_leg_.get_notional();
  //Needs to be positive even if it is a payer leg
  pv01 = fabs(pv01);
  return pv01;
}

double IborSwap::swap_rate(const ChronoDate& val_date, const DiscountCurve& disc_curve,std::optional<DiscountCurve>& index_curve,
                std::optional<double> first_fixing) {
  auto pv = pv01(val_date,disc_curve);
  if (pv < gSmall){
    throw std::runtime_error("PV01 is zero. Cannot compute swap rate.");
  }
  auto disc_factor = [&](auto val_date, auto eff_date){
    if (val_date < eff_date){
      return disc_curve.df(eff_date);
    } else {
      return disc_curve.df(val_date);
    }
  };
  auto df = disc_factor(val_date, eff_date_);
  double double_leg_pv = 0.0;
  if (!index_curve.has_value()){
    auto df_t = disc_curve.df(maturity_date_);
    double_leg_pv = (df - df_t);
  } else {
    double_leg_pv = double_leg_.value(val_date,disc_curve,index_curve.value(),first_fixing);
    double_leg_pv /= double_leg_pv/notional_;
  }
  auto cpn = double_leg_pv / pv;
  return cpn;
}

ChronoDate IborSwap::get_eff_date() const { return eff_date_;}
ChronoDate IborSwap::get_maturity_date() const { return maturity_date_;}
SwapFixedLeg IborSwap::get_fixed_leg() const { return fixed_leg_;}
SwapFloatLeg IborSwap::get_double_leg() const {return double_leg_;}
ChronoDate IborSwap::get_termination_date() const { return termination_date_;}

