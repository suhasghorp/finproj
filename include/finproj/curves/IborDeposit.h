#ifndef FINPROJ_INCLUDE_FINPROJ_CURVES_IBORDEPOSIT_H_
#define FINPROJ_INCLUDE_FINPROJ_CURVES_IBORDEPOSIT_H_

#include <finproj/utils/ChronoDate.h>
#include <finproj/utils/DayCount.h>
#include <finproj/curves/DiscountCurve.h>
#include <string>

class IborDeposit {
 public:
  IborDeposit(const ChronoDate& start_date,
              const ChronoDate& maturity_date,
              double deposit_rate,
              DayCountTypes day_count_type,
              double notional = 100.0,
              CalendarTypes cal_type = CalendarTypes::WEEKEND,
              BusDayAdjustTypes bus_day_adjust_type = BusDayAdjustTypes::MODIFIED_FOLLOWING);
  IborDeposit(const ChronoDate& start_date,
              const std::string& tenor,
              double deposit_rate,
              DayCountTypes day_count_type,
              double notional = 100.0,
              CalendarTypes cal_type = CalendarTypes::WEEKEND,
              BusDayAdjustTypes bus_day_adjust_type = BusDayAdjustTypes::MODIFIED_FOLLOWING);
  IborDeposit(const IborDeposit& rhs) = default;
  double value(const ChronoDate& val_date, const DiscountCurve& libor_curve) const;
  double maturity_df() const;

  ChronoDate get_start_date() const;
  ChronoDate get_maturity_date() const;
  double get_notional() const;

  void set_start_date(ChronoDate dt);
  void set_maturity_date(ChronoDate dt);


 private:
  CalendarTypes cal_type_{};
  BusDayAdjustTypes bus_day_adjust_type_{};
  double notional_{};
  DayCountTypes day_count_type_{};
  double deposit_rate_{};
  ChronoDate maturity_date_{}, start_date_{};
  std::string tenor_{};
  Calendar calendar_;
};

#endif//FINPROJ_INCLUDE_FINPROJ_CURVES_IBORDEPOSIT_H_
