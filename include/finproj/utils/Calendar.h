#ifndef FINDATE__CALENDAR_H_
#define FINDATE__CALENDAR_H_

#include "ChronoDate.h"

enum class BusDayAdjustTypes {
  NONE = 1,
  FOLLOWING = 2,
  MODIFIED_FOLLOWING = 3,
  PRECEDING = 4,
  MODIFIED_PRECEDING = 5
};
enum class DateGenRuleTypes {
  FORWARD = 1,
  BACKWARD = 2
};
enum class CalendarTypes {
  NONE = 1,
  WEEKEND = 2,
  UNITED_STATES = 14
};
enum class FrequencyTypes {
  ZERO = 100,
  SIMPLE = 0,
  ANNUAL = 1,
  SEMI_ANNUAL = 2,
  TRI_ANNUAL = 3,
  QUARTERLY = 4,
  MONTHLY = 12,
  CONTINUOUS = -1
};
class Calendar {

 public:
  Calendar(CalendarTypes calType);
  ChronoDate adjust(const ChronoDate& dt, const BusDayAdjustTypes busDayConv) const;
  bool is_business_day(const ChronoDate& dt) const;
  bool is_holiday(const ChronoDate& dt) const;
  bool holiday_united_states(const ChronoDate& dt) const;
  ChronoDate add_business_days(const ChronoDate& dt, unsigned int n) const;

 private:
  CalendarTypes calType_;
  //BusDayAdjustTypes busDayConv_;

};

#endif//FINDATE__CALENDAR_H_
