#ifndef FINPROJ_INCLUDE_FINPROJ_CURVES_IBORFUTURE_H_
#define FINPROJ_INCLUDE_FINPROJ_CURVES_IBORFUTURE_H_

#include <finproj/utils/DayCount.h>
#include <finproj/curves/IborFRA.h>
#include <string>

class IborFuture {
 public:
  IborFuture(const ChronoDate& date, int fut_number, std::string tenor = "3M",
  DayCountTypes accrual_type = DayCountTypes::ACT_360,
             double contract_size = 1'000'000);
  IborFRA to_fra(double futures_price, double convexity) const;
  double futures_rate(double futures_price) const;
  double fra_rate(double futures_price, double convexity) const;
  double convexity(const ChronoDate& val_date, double volatility, double mean_reversion) const;
  ChronoDate get_delivery_date() const;
  ChronoDate get_start_date() const;

 private:
  ChronoDate date_{}, delivery_date_{}, end_of_interest_period_{}, last_trading_date_{};
  int fut_number_{};
  std::string tenor_{};
  DayCountTypes accrual_type_{};
  double contract_size_{};


};

#endif//FINPROJ_INCLUDE_FINPROJ_CURVES_IBORFUTURE_H_
