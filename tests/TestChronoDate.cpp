#include "finproj/utils/Calendar.h"
#include "finproj/utils/ChronoDate.h"
#include "finproj/utils/Schedule.h"
#include <catch2/catch_test_macros.hpp>

auto freq_type = FrequencyTypes::SEMI_ANNUAL;
auto date_gen_rule_type = DateGenRuleTypes::BACKWARD;

TEST_CASE( "test_date_adjust_noweekend_following", "[single-file]" ){
  auto start_date = ChronoDate(2008,2,28);
  auto end_date = ChronoDate(2011,2,28);
  auto cal_type = CalendarTypes::NONE;
  auto bus_day_adjust = BusDayAdjustTypes::FOLLOWING;

  auto schedule = Schedule::create(start_date, end_date)
                      .withFreqType(freq_type)
  .withCalendar(cal_type)
  .withBusDayAdjustType(bus_day_adjust)
  .withDateGenRuleType(date_gen_rule_type);

  std::vector<ChronoDate> expected{ChronoDate(2008,2,28),
                                   ChronoDate(2008,8,28),
                                   ChronoDate(2009,2,28),
                                   ChronoDate(2009,8,28),
                                   ChronoDate(2010,2,28),
                                   ChronoDate(2010,8,28),
                                   ChronoDate(2011,2,28)};
  REQUIRE(expected == schedule.get_schedule());

}

TEST_CASE( "test_date_adjust_noweekend_modfollowing", "[single-file]" ){
  auto start_date = ChronoDate(2008,2,28);
  auto end_date = ChronoDate(2011,2,28);
  auto cal_type = CalendarTypes::NONE;
  auto bus_day_adjust = BusDayAdjustTypes::MODIFIED_FOLLOWING;

  auto schedule = Schedule::create(start_date, end_date)
                      .withFreqType(freq_type)
                      .withCalendar(cal_type)
                      .withBusDayAdjustType(bus_day_adjust)
                      .withDateGenRuleType(date_gen_rule_type);

  std::vector<ChronoDate> expected{ChronoDate(2008,2,28),
                                   ChronoDate(2008,8,28),
                                   ChronoDate(2009,2,28),
                                   ChronoDate(2009,8,28),
                                   ChronoDate(2010,2,28),
                                   ChronoDate(2010,8,28),
                                   ChronoDate(2011,2,28)};
  REQUIRE(expected == schedule.get_schedule());

}

TEST_CASE( "test_date_adjust_noweekend_usholidays_modfollowing", "[single-file]" ){
  auto start_date = ChronoDate(2008,7,4);
  auto end_date = ChronoDate(2011,7,4);
  auto cal_type = CalendarTypes::UNITED_STATES;
  auto bus_day_adjust = BusDayAdjustTypes::MODIFIED_FOLLOWING;

  auto schedule = Schedule::create(start_date, end_date)
                      .withFreqType(freq_type)
                      .withCalendar(cal_type)
                      .withBusDayAdjustType(bus_day_adjust)
                      .withDateGenRuleType(date_gen_rule_type);

  std::vector<ChronoDate> expected{ChronoDate(2008,7,4),
                                   ChronoDate(2009,1,5),
                                   ChronoDate(2009,7,6),
                                   ChronoDate(2010,1,4),
                                   ChronoDate(2010,7,6),
                                   ChronoDate(2011,1,4),
                                   ChronoDate(2011,7,5)};
  std::vector<ChronoDate> actual = schedule.get_schedule();
  //for (ChronoDate c : actual)
    //std::cout << c << std::endl;
  REQUIRE(expected == actual);

}