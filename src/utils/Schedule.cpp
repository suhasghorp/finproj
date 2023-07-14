#include "finproj/utils/Schedule.h"
#include <algorithm>
#include <stdexcept>

std::vector<ChronoDate> Schedule::get_schedule() {
  generate();
  return adjusted_dates_;
}
void Schedule::generate() {
  int num_months = 12/static_cast<int>(freq_type_);
  std::vector<ChronoDate> unadjusted_dates;
  int flow_num{0};
  if (date_gen_rule_type_ == DateGenRuleTypes::BACKWARD){
    auto next_date = term_date_;
    while(next_date > eff_date_){
      unadjusted_dates.push_back(next_date);
      int tot_num_months = num_months * (1 + flow_num);
      next_date = term_date_.add_months(-tot_num_months);
      if (end_of_month_)
        next_date = next_date.eom();
      flow_num += 1;
    }
    unadjusted_dates.push_back(next_date);
    ++flow_num;

    auto dt = unadjusted_dates[flow_num - 1];
    adjusted_dates_.push_back(dt);
    for (auto i{1}; i < flow_num-1;++i){
      dt = calendar_.adjust(unadjusted_dates[flow_num - i - 1],bus_day_adjust_type_);
      adjusted_dates_.push_back(dt);
    }
    adjusted_dates_.push_back(term_date_);

    if (adjusted_dates_[0] < eff_date_)
      adjusted_dates_[0] = eff_date_;

    if (adjust_term_date_){
      term_date_ = calendar_.adjust(term_date_, bus_day_adjust_type_);
      adjusted_dates_.back() = term_date_;
    }
    if (adjusted_dates_.size() < 2)
      throw std::runtime_error("Schedule has 2 dates only");

    //return adjusted_dates_;
  }
}

