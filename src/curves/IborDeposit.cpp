#include <finproj/curves/IborDeposit.h>
#include <tuple>

IborDeposit::IborDeposit(const ChronoDate& start_date,
            const ChronoDate& maturity_date,
            double deposit_rate,
            DayCountTypes day_count_type,
            double notional,
            CalendarTypes cal_type,
            BusDayAdjustTypes bus_day_adjust_type):

            start_date_{start_date},deposit_rate_{deposit_rate},
            day_count_type_{day_count_type},notional_(notional),cal_type_{cal_type},
            bus_day_adjust_type_{bus_day_adjust_type}, calendar_(Calendar(cal_type_))
{
  maturity_date_ = calendar_.adjust(maturity_date, bus_day_adjust_type);
  if (start_date_ > maturity_date_)
    throw std::runtime_error("Start date cannot be after maturity date");
}

IborDeposit::IborDeposit(const ChronoDate& start_date,
                         const std::string& tenor,
                         double deposit_rate,
                         DayCountTypes day_count_type,
                         double notional,
                         CalendarTypes cal_type,
                         BusDayAdjustTypes bus_day_adjust_type):

                        start_date_{start_date},tenor_{tenor},deposit_rate_{deposit_rate},
                        day_count_type_{day_count_type},notional_(notional),cal_type_{cal_type},
                        bus_day_adjust_type_{bus_day_adjust_type}, calendar_(Calendar(cal_type_))
{
  auto maturity_date = start_date_.add_tenor(tenor_);
  new (this) IborDeposit(start_date,maturity_date,deposit_rate,day_count_type,notional,cal_type,bus_day_adjust_type);
}

double IborDeposit::value(const ChronoDate& val_date, const DiscountCurve& libor_curve) const{
  if (val_date > start_date_)
    throw std::runtime_error("Start date after maturity date");
  auto dc = DayCount(day_count_type_);
  auto acc_factor = std::get<0>(dc.year_frac(start_date_, maturity_date_, FrequencyTypes::ANNUAL));
  auto df_settle = libor_curve.df(start_date_);
  auto df_maturity = libor_curve.df(maturity_date_);
  auto v = (1.0 + acc_factor * deposit_rate_) * notional_;

  //Need to take into account spot days being zero so depo settling fwd
  v = v * df_maturity / df_settle;
  return v;
}

double IborDeposit::maturity_df() const{
  DayCount dc = DayCount{day_count_type_};
  auto acc_factor = std::get<0>(dc.year_frac(start_date_, maturity_date_, FrequencyTypes::ANNUAL));
  auto df = 1.0 / (1.0 + acc_factor * deposit_rate_);
  return df;
}

ChronoDate IborDeposit::get_start_date() const{ return start_date_;}
ChronoDate IborDeposit::get_maturity_date() const{ return maturity_date_;}
double IborDeposit::get_notional() const {return notional_;}

void IborDeposit::set_start_date(ChronoDate dt){start_date_ = dt;}
void IborDeposit::set_maturity_date(ChronoDate dt){maturity_date_ = dt;}