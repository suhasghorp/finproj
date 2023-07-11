#ifndef FINPROJ_INCLUDE_FINPROJ_CURVES_IBORFRA_H_
#define FINPROJ_INCLUDE_FINPROJ_CURVES_IBORFRA_H_
#include <finproj/utils/ChronoDate.h>
#include <finproj/utils/DayCount.h>
#include <finproj/curves/DiscountCurve.h>
#include <string>

class IborFRA {
 public:
  IborFRA(const ChronoDate& start_date,
              const ChronoDate& maturity_date,
              double fra_rate,
              DayCountTypes day_count_type,
              double notional = 100.0,
              bool pay_fixed_rate = true,
              CalendarTypes cal_type = CalendarTypes::WEEKEND,
              BusDayAdjustTypes bus_day_adjust_type = BusDayAdjustTypes::MODIFIED_FOLLOWING);
  IborFRA(const ChronoDate& start_date,
          const std::string& tenor,
          double fra_rate,
          DayCountTypes day_count_type,
          double notional = 100.0,
          bool pay_fixed_rate = true,
          CalendarTypes cal_type = CalendarTypes::WEEKEND,
          BusDayAdjustTypes bus_day_adjust_type = BusDayAdjustTypes::MODIFIED_FOLLOWING);
  double value(const ChronoDate& val_date, const DiscountCurve& disc_curve, const DiscountCurve& index_curve) const;
  double maturity_df(const DiscountCurve& index_curve) const;
  ChronoDate get_start_date() const;
  ChronoDate get_maturity_date() const;
  double get_notional() const;

 private:

  ChronoDate start_date_{}, maturity_date_{} ;
  std::string tenor_{};
  double fra_rate_{};
  DayCountTypes day_count_type_{};
  double notional_{};
  bool pay_fixed_rate_{};
  CalendarTypes cal_type_{};
  BusDayAdjustTypes bus_day_adjust_type_{};

  Calendar calendar_;

};

#endif//FINPROJ_INCLUDE_FINPROJ_CURVES_IBORFRA_H_
