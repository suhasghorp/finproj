#include "finproj/utils/Calendar.h"
#include "finproj/utils/ChronoDate.h"
#include "finproj/utils/DayCount.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <tuple>

ChronoDate start(2019,1,1);
ChronoDate end(2019,5,21);
FrequencyTypes freq(FrequencyTypes::ANNUAL);

TEST_CASE( "test_year_frace_THIRTY_360_BOND", "[single-file]" ){
  auto day_count_type = DayCountTypes::THIRTY_360_BOND;
  auto day_count = DayCount(day_count_type);
  auto expected = day_count.year_frac(start, end, freq);
  REQUIRE_THAT(std::get<0>(expected), Catch::Matchers::WithinAbs(0.3889, 0.0001));
}

TEST_CASE( "test_year_frace_THIRTY_E_360_ISDA", "[single-file]" ){
  auto day_count_type = DayCountTypes::THIRTY_E_360_ISDA;
  auto day_count = DayCount(day_count_type);
  auto expected = day_count.year_frac(start, end, freq);
  REQUIRE_THAT(std::get<0>(expected), Catch::Matchers::WithinAbs(0.3889, 0.0001));
}

TEST_CASE( "test_year_frace_THIRTY_E_PLUS_360", "[single-file]" ){
  auto day_count_type = DayCountTypes::THIRTY_E_PLUS_360;
  auto day_count = DayCount(day_count_type);
  auto expected = day_count.year_frac(start, end, freq);
  REQUIRE_THAT(std::get<0>(expected), Catch::Matchers::WithinAbs(0.3889, 0.0001));
}

TEST_CASE( "test_year_frace_ACT_ACT_ISDA", "[single-file]" ){
  auto day_count_type = DayCountTypes::ACT_ACT_ISDA;
  auto day_count = DayCount(day_count_type);
  auto expected = day_count.year_frac(start, end, freq);
  REQUIRE_THAT(std::get<0>(expected), Catch::Matchers::WithinAbs(0.3836, 0.0001));
}

TEST_CASE( "test_year_frace_ACT_ACT_ICMA", "[single-file]" ){
  auto day_count_type = DayCountTypes::ACT_ACT_ICMA;
  auto day_count = DayCount(day_count_type);
  auto expected = day_count.year_frac(start, end, freq, false, end);
  REQUIRE_THAT(std::get<0>(expected), Catch::Matchers::WithinAbs(1.0000, 0.0001));
}

TEST_CASE( "test_year_frace_ACT_365F", "[single-file]" ){
  auto day_count_type = DayCountTypes::ACT_365F;
  auto day_count = DayCount(day_count_type);
  auto expected = day_count.year_frac(start, end, freq);
  REQUIRE_THAT(std::get<0>(expected), Catch::Matchers::WithinAbs(0.3836, 0.0001));
}

TEST_CASE( "test_year_frace_ACT_360", "[single-file]" ){
  auto day_count_type = DayCountTypes::ACT_360;
  auto day_count = DayCount(day_count_type);
  auto expected = day_count.year_frac(start, end, freq);
  REQUIRE_THAT(std::get<0>(expected), Catch::Matchers::WithinAbs(0.3889, 0.0001));
}

TEST_CASE( "test_year_frace_ACT_365L", "[single-file]" ){
  auto day_count_type = DayCountTypes::ACT_365L;
  auto day_count = DayCount(day_count_type);
  auto expected = day_count.year_frac(start, end, freq, false, end);
  REQUIRE_THAT(std::get<0>(expected), Catch::Matchers::WithinAbs(0.3836, 0.0001));
}

TEST_CASE( "test_year_frace_SIMPLE", "[single-file]" ){
  auto day_count_type = DayCountTypes::SIMPLE;
  auto day_count = DayCount(day_count_type);
  auto expected = day_count.year_frac(start, end, freq, false, end);
  REQUIRE_THAT(std::get<0>(expected), Catch::Matchers::WithinAbs(0.3836, 0.0001));
}

