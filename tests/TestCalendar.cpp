#include "finproj/utils/Calendar.h"
#include "finproj/utils/ChronoDate.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE( "test_add_business_day", "[single-file]" ){
  int bus_days_in_decade = 2507;

  auto cal = Calendar(CalendarTypes::UNITED_STATES);
  ChronoDate start{2020,1,3};
  ChronoDate expected{2030,1,3};
  auto actual = cal.add_business_days(start,bus_days_in_decade);
  REQUIRE(actual == expected);

}
