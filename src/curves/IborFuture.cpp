#include <finproj/curves/IborFuture.h>
#include <cmath>

IborFuture::IborFuture(const ChronoDate& date, int fut_number, std::string tenor,
           DayCountTypes accrual_type, double contract_size):
            date_{date},fut_number_{fut_number},tenor_{tenor},accrual_type_{accrual_type},
                                                          contract_size_{contract_size}
{
  if (fut_number_ < 1)
    throw std::runtime_error("Future number must be 1 or more");
  if (tenor_ != "3M")
    throw std::runtime_error("Only 3M IMM futures handled currently.");
  delivery_date_ = date_.next_imm_date();
  for (auto i{0}; i < (fut_number_ - 1); ++i){
    delivery_date_ = delivery_date_.next_imm_date();
  }
  end_of_interest_period_ = delivery_date_.next_imm_date();
  last_trading_date_ = delivery_date_.add_days(-2);
}

IborFuture::IborFuture(const ChronoDate& date, const ChronoDate& delivery_date, std::string tenor,
           DayCountTypes accrual_type,
           double contract_size):date_{date},delivery_date_{delivery_date},tenor_{tenor},accrual_type_{accrual_type},
                                               contract_size_{contract_size}
{
  end_of_interest_period_ = delivery_date_.next_imm_date();
  last_trading_date_ = delivery_date_.add_days(-2);
}

double IborFuture::fra_rate(double futures_price, double convexity) const{
  /** Convert futures price and convexity to a FRA rate using the BBG
negative convexity (in percent). This is then divided by 100 before
being added to the futures rate. */
  double fra_rate{};
  auto fut_rate = (100.0 - futures_price) / 100.0;
  if (convexity < 0)
      fra_rate = fut_rate + convexity/100.0;
  else
      fra_rate = fut_rate - convexity/100.0;
  return fra_rate;
}

double IborFuture::futures_rate(double futures_price) const {
  /** Calculate implied futures rate from the futures price. */
  auto futures_rate = (100.0 - futures_price) / 100.0;
  return futures_rate;
}

IborFRA IborFuture::to_fra(double futures_price, double convexity) const {
  /** Convert the futures contract to a IborFRA object so it can be
used to boostrap a Ibor curve. For this we need to adjust the futures
rate using the convexity correction. */

  auto f_rate = fra_rate(futures_price, convexity);

  auto fra = IborFRA(delivery_date_,
                          end_of_interest_period_,
                          f_rate,
                          accrual_type_,
                          contract_size_,
                          false);
  return fra;
}

ChronoDate IborFuture::get_delivery_date() const {return delivery_date_;}
ChronoDate IborFuture::get_start_date() const {return date_;}
