#include "finproj/utils/DayCount.h"
#include <tuple>

DayCount::DayCount(DayCountTypes dcc_type):dcc_type_{dcc_type} {}

std::tuple<double,unsigned int, unsigned int> DayCount::year_frac(const ChronoDate& dt1, const ChronoDate& dt2,
                                                        FrequencyTypes freq_type,std::optional<bool> is_term_date,
                                                        const std::optional<ChronoDate> &dt3) {
  std::tuple<double,unsigned int, unsigned int> ret{};
  unsigned int d1 = dt1.day();
  unsigned int m1 = dt1.month();
  int y1 = dt1.year();

  unsigned int d2 = dt2.day();
  unsigned int m2 = dt2.month();
  int y2 = dt2.year();


  if (dcc_type_ == DayCountTypes::THIRTY_360_BOND){
    if (d1 == 31)
      d1 = 30;
    if (d2 == 31 && d1 == 30)
          d2 = 30;

    unsigned int num = 360 * (y2 - y1) + 30 * (m2 - m1) + (d2 - d1);
    unsigned int den = 360;
    auto acc_factor = double(num) / double(den);
    ret = std::make_tuple(acc_factor, num, den);

  } else if (dcc_type_ == DayCountTypes::THIRTY_E_360){
    if (d1 == 31)
          d1 = 30;
    if (d2 == 31)
          d2 = 30;

    unsigned int num = 360 * (y2 - y1) + 30 * (m2 - m1) + (d2 - d1);
    unsigned int den = 360;
    auto acc_factor = double(num) / double(den);
    ret = std::make_tuple(acc_factor, num, den);

  } else if (dcc_type_ == DayCountTypes::THIRTY_E_360_ISDA){
    if (d1 == 31)
          d1 = 30;

    if (dt1.is_last_day_of_feb())
      d1 = 30;

    if (d2 == 31)
      d2 = 30;


    if (dt2.is_last_day_of_feb() && is_term_date)
      d2 = 30;

    unsigned int num = 360 * (y2 - y1) + 30 * (m2 - m1) + (d2 - d1);
    auto den = 360;
    double acc_factor = double(num) / double(den);
    ret = std::make_tuple(acc_factor, num, den);

  } else if (dcc_type_ == DayCountTypes::THIRTY_E_PLUS_360) {
    if (d1 == 31)
      d1 = 30;
    if (d2 == 31) {
      m2 = m2 + 1;
      d2 = 1;
    }

    unsigned int num = 360 * (y2 - y1) + 30 * (m2 - m1) + (d2 - d1);
    unsigned int den = 360;
    double acc_factor = double(num) / double(den);
    ret = std::make_tuple(acc_factor, num, den);

  } else if (dcc_type_ == DayCountTypes::ACT_ACT_ISDA or dcc_type_ == DayCountTypes::ZERO) {
    unsigned int denom1 = 365, denom2 = 365;
    if (dt1.ymd().year().is_leap())
      denom1 = 366;
    if (dt2.ymd().year().is_leap())
      denom2 = 366;
    if (y1 == y2){
      auto num = dt2 - dt1;
      auto den = denom1;
      double acc_factor = double(dt2 - dt1) / double(denom1);
      ret = std::make_tuple(acc_factor, num, den);
    } else {
      int daysYear1 = ChronoDate(y1+1,1,1).serial_date() - dt1.serial_date();
      int daysYear2 = dt2.serial_date() - ChronoDate(y2,1,1).serial_date();
      double acc_factor1 = double(daysYear1) / double(denom1);
      double acc_factor2 = double(daysYear2) / double(denom2);
      double yearDiff = double(y2) - double(y1) - 1.0;
      auto num = daysYear1 + daysYear2;
      auto den = denom1 + denom2;
      double acc_factor = acc_factor1 + acc_factor2 + yearDiff;
      ret = std::make_tuple(acc_factor, num, den);
    }
  } else if (dcc_type_ == DayCountTypes::ACT_ACT_ICMA) {
    if (!dt3)
      throw std::runtime_error("ACT_ACT_ICMA requires three dates and a freq");
    auto num = dt2 - dt1;
    auto den = static_cast<int>(freq_type) * (dt3.value() - dt1);
    double acc_factor = double(num) / double(den);
    ret = std::make_tuple(acc_factor, num, den);
  } else if (dcc_type_ == DayCountTypes::ACT_365F){
    auto num = dt2 - dt1;
    auto den = 365;
    double acc_factor = double(num) / double(den);
    ret = std::make_tuple(acc_factor, num, den);
  } else if (dcc_type_ == DayCountTypes::ACT_360){
    auto num = dt2 - dt1;
    auto den = 360;
    double acc_factor = double(num) / double(den);
    ret = std::make_tuple(acc_factor, num, den);
  } else if (dcc_type_ == DayCountTypes::ACT_365L){
    int y3 = y2;
    if (dt3)
      y3 = dt3.value().year();
    auto num = dt2 - dt1;
    auto den = 365;
    ChronoDate feb29;
    if (dt1.ymd().year().is_leap())
      feb29 = ChronoDate(y1,2,29);
    else if (dt3.value().ymd().year().is_leap())
      feb29 = ChronoDate(y3,2,29);
    if (static_cast<int>(freq_type) == 1) {
      if (feb29 > dt1 && feb29 <= dt3)
        den = 366;
    } else {
      if (dt3.value().ymd().year().is_leap())
        den = 366;
    }
    double acc_factor = double(num) / double(den);
    ret = std::make_tuple(acc_factor, num, den);

  } else if (dcc_type_ == DayCountTypes::SIMPLE){
    auto num = dt2 - dt1;
    auto den = 365;
    double acc_factor = double(num) / double(den);
    ret = std::make_tuple(acc_factor, num, den);
  }
  return ret;
}
