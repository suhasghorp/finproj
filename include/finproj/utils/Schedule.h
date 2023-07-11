#ifndef FINDATE__SCHEDULE_H_
#define FINDATE__SCHEDULE_H_

#include "ChronoDate.h"
#include "Calendar.h"
#include <vector>

class Schedule {

 public:
  static Schedule create(const ChronoDate& eff_date, const ChronoDate& term_date) {return Schedule(eff_date, term_date, FrequencyTypes::SIMPLE);};
  Schedule& withFreqType(FrequencyTypes freq_type){freq_type_ = freq_type; return *this;}
  Schedule& withCalendar(CalendarTypes cal_type){cal_type_ = cal_type; return *this;}
  Schedule& withBusDayAdjustType(BusDayAdjustTypes bus_day_adjust_type){bus_day_adjust_type_ = bus_day_adjust_type; return *this;}
  Schedule& withDateGenRuleType(DateGenRuleTypes date_gen_rule_type){date_gen_rule_type_ = date_gen_rule_type; return *this;}
  Schedule& withAdjTermDate(bool adj_term_date){adjust_term_date_ = adj_term_date;return *this;}
  Schedule withEndOfMonth(bool end_of_month){end_of_month_ = end_of_month;return *this;}
  std::vector<ChronoDate> get_schedule();
 private:
  Schedule(const ChronoDate &eff_date, const ChronoDate &term_date, FrequencyTypes freq_type) : eff_date_{eff_date}, term_date_{term_date}, freq_type_{freq_type}{};
  void generate();
  std::vector<ChronoDate> adjusted_dates_;
  ChronoDate eff_date_, term_date_;
  CalendarTypes cal_type_{CalendarTypes::WEEKEND};
  FrequencyTypes freq_type_{FrequencyTypes::ANNUAL};
  BusDayAdjustTypes bus_day_adjust_type_{BusDayAdjustTypes::FOLLOWING};
  DateGenRuleTypes date_gen_rule_type_{DateGenRuleTypes::BACKWARD};
  bool adjust_term_date_{true};
  bool end_of_month_{false};
  Calendar calendar_{cal_type_};
};

#endif//FINDATE__SCHEDULE_H_
