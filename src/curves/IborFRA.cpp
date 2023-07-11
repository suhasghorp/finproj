#include <finproj/curves/IborFRA.h>
#include <tuple>

IborFRA::IborFRA(const ChronoDate& start_date,
        const ChronoDate& maturity_date,
        double fra_rate,
        DayCountTypes day_count_type,
        double notional,
        bool pay_fixed_rate,
        CalendarTypes cal_type,
        BusDayAdjustTypes bus_day_adjust_type):
        start_date_{start_date},maturity_date_{maturity_date},fra_rate_{fra_rate},
        day_count_type_{day_count_type},notional_(notional),pay_fixed_rate_{pay_fixed_rate},cal_type_{cal_type},
        bus_day_adjust_type_{bus_day_adjust_type}, calendar_(Calendar(cal_type_))
{

}

IborFRA::IborFRA(const ChronoDate& start_date,
                 const std::string& tenor,
                 double fra_rate,
                 DayCountTypes day_count_type,
                 double notional,
                 bool pay_fixed_rate,
                 CalendarTypes cal_type,
                 BusDayAdjustTypes bus_day_adjust_type):
                start_date_{start_date},tenor_{tenor},fra_rate_{fra_rate},
                day_count_type_{day_count_type},notional_(notional),pay_fixed_rate_{pay_fixed_rate},cal_type_{cal_type},
                bus_day_adjust_type_{bus_day_adjust_type}, calendar_(Calendar(cal_type_))
{
  maturity_date_ = start_date_.add_tenor(const_cast<std::string &>(tenor_));
  maturity_date_ = calendar_.adjust(maturity_date_, bus_day_adjust_type);
  if (start_date_ > maturity_date_)
    throw std::runtime_error("Start date cannot be after maturity date");
}

double IborFRA::value(const ChronoDate& val_date, const DiscountCurve& disc_curve, const DiscountCurve& index_curve) const{
  //Get the Libor index from the index curve
  auto dc = DayCount(day_count_type_);
  auto acc_factor = std::get<0>(dc.year_frac(start_date_, maturity_date_,FrequencyTypes::ANNUAL));
  auto dfIndex1 = index_curve.df(start_date_);
  auto dfIndex2 = index_curve.df(maturity_date_);
  auto liborFwd = (dfIndex1 / dfIndex2 - 1.0) / acc_factor;

  //Get the discount factor from a discount curve
  auto dfDiscount2 = disc_curve.df(maturity_date_);
  auto v = acc_factor * (liborFwd - fra_rate_) * dfDiscount2;

  //Forward value the FRA to the value date
  auto df_to_valuation_date = disc_curve.df(val_date);
  v = v * notional_ / df_to_valuation_date;
  if (pay_fixed_rate_)
        v *= -1.0;
  return v;
}

double IborFRA::maturity_df(const DiscountCurve& index_curve) const {
  DayCount dc = DayCount(day_count_type_);
  auto df1 = index_curve.df(start_date_);
  auto acc_factor = std::get<0>(dc.year_frac(start_date_, maturity_date_, FrequencyTypes::ANNUAL));
  auto df2 = df1 / (1.0 + acc_factor * fra_rate_);
  return df2;
}

ChronoDate IborFRA::get_start_date() const { return start_date_;}
ChronoDate IborFRA::get_maturity_date() const { return maturity_date_;}
double IborFRA::get_notional() const { return notional_;}