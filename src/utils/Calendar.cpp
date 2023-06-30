#include "finproj/utils/Calendar.h"
#include <stdexcept>

Calendar::Calendar(CalendarTypes calType) : calType_{ calType } {}

ChronoDate Calendar::adjust(const ChronoDate& dt, const BusDayAdjustTypes busDayConv) const {
  ChronoDate new_dt{dt};
  if (calType_ == CalendarTypes::NONE || busDayConv == BusDayAdjustTypes::NONE) {
    return new_dt;
  }
  if (busDayConv == BusDayAdjustTypes::FOLLOWING) {
    while (is_holiday(new_dt)) {
      new_dt = new_dt.add_days(1);
    }
    return new_dt;
  }
  if (busDayConv == BusDayAdjustTypes::MODIFIED_FOLLOWING) {
    unsigned int orig_day = dt.day();
    unsigned int orig_month = dt.month();
    unsigned int orig_year = dt.year();

    while (!is_business_day(new_dt)) {
      new_dt = new_dt.add_days(1);
    }
    //if the business day is in a different month look back
    //for previous first business day one day at a time
    if (new_dt.month() != orig_month) {
      auto dt_new = ChronoDate(orig_year, orig_month, orig_day);
      while (is_holiday(dt_new)) {
        dt_new = dt_new.add_days(-1);
      }
      return dt_new;
    } else return new_dt;

  }
  if (busDayConv == BusDayAdjustTypes::PRECEDING) {
    throw std::invalid_argument("Preceding conv is not yet implemented");
  }
  if (busDayConv == BusDayAdjustTypes::MODIFIED_PRECEDING) {
    throw std::invalid_argument("Modified Preceding conv is not yet implemented");
  }
  return new_dt;
}

bool Calendar::is_business_day(const ChronoDate& dt) const {
  if (dt.is_weekend()) return false;
  if (is_holiday(dt)) return false;
  return true;
}
bool Calendar::is_holiday(const ChronoDate& dt) const {
  if (calType_ == CalendarTypes::NONE)
    return false;
  else if (calType_ == CalendarTypes::WEEKEND) {
    if (dt.is_weekend()) {
      return true;;
    }
    else return false;
  }
  else if (calType_ == CalendarTypes::UNITED_STATES) {
    return holiday_united_states(dt);
  }
  else throw std::invalid_argument("Unknown Calendar provided");
}

ChronoDate Calendar::add_business_days(const ChronoDate& dt, unsigned int n) const {
  auto orig = dt.ymd();
  auto new_dt = ChronoDate(dt);
  while (n > 0) {
    new_dt = new_dt.add_days(1);
    if (is_business_day(new_dt)) {
      n = n - 1;
    }
  }
  return new_dt;
}

bool Calendar::holiday_united_states(const ChronoDate& dt) const {
  unsigned int d = dt.day();
  unsigned int m = dt.month();
  std::chrono::weekday wd = dt.weekday();

  if (dt.day() == 1 && dt.month() == 1)
    return true;;
  if (m == 1 and d == 2 and wd == std::chrono::Monday)
    return true;;
  if (m == 1 and d == 3 and wd == std::chrono::Monday)
    return true;;

  if (m == 1 && d >= 15 && d < 22 && wd == std::chrono::Monday)
    return true;;

  if (m == 2 && d >= 15 && d < 22 && wd == std::chrono::Monday)
    return true;;

  if (m == 5 && d >= 25 && d <= 31 && wd == std::chrono::Monday)
    return true;;

  if (m == 7 && d == 4)
    return true;;

  if (m == 7 && d == 5 && wd == std::chrono::Monday)
    return true;

  if (m == 7 && d == 3 && wd == std::chrono::Friday)
    return true;

  if (m == 9 && d >= 1 && d < 8 && wd == std::chrono::Monday)
    return true;

  if (m == 10 && d >= 8 && d < 15 && wd == std::chrono::Monday)
    return true;

  if (m == 11 && d == 11)
    return true;

  if (m == 11 && d == 12 && wd == std::chrono::Monday)
    return true;

  if (m == 11 && d == 10 && wd == std::chrono::Friday)
    return true;

  if (m == 11 && d >= 22 && d < 29 && wd == std::chrono::Thursday)
    return true;

  if (m == 12 && d == 24 && wd == std::chrono::Friday)
    return true;

  if (m == 12 && d == 25)
    return true;

  if (m == 12 && d == 26 && wd == std::chrono::Monday)
    return true;

  if (m == 12 && d == 31 && wd == std::chrono::Friday)
    return true;

  return false;
}