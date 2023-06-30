#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "finproj/utils/ChronoDate.h"
#include "finproj/utils/Schedule.h"
#include <finproj/utils/Misc.h>
#include <finproj/curves/DiscountCurveZeros.h>
#include <algorithm>

TEST_CASE( "test_FinDiscountCurveZeros", "[single-file]" ){
  ChronoDate start_date(2018,1,1);
  auto times = linspace(1.0,10.0,10);
  auto zero_rates = linspace(5.0, 6.0, 10);
  std::transform(zero_rates.cbegin(), zero_rates.cend(),
                 zero_rates.begin(), // write to the same location
                 [](double z) { return z/100.0; });

  std::vector<ChronoDate> dates{};
  dates.reserve(times.size());
  std::transform(times.cbegin(), times.cend(), std::back_inserter(dates),
                 [start_date](double t) { return start_date.add_years(t); });
  FrequencyTypes freq_type = FrequencyTypes::ANNUAL;
  DayCountTypes day_count_type = DayCountTypes::ACT_ACT_ISDA;

  DiscountCuveZeros curve{start_date,dates,zero_rates,freq_type,day_count_type,InterpTypes::FLAT_FWD_RATES};

  auto dt = start_date.add_years(0);
  auto expected = curve.df(dt);
  REQUIRE_THAT(expected, Catch::Matchers::WithinAbs(1.000, 0.0001));

  dt = start_date.add_years(2.5);
  expected = curve.df(dt);
  REQUIRE_THAT(expected, Catch::Matchers::WithinAbs(0.8816, 0.0001));

  dt = start_date.add_years(5.0);
  expected = curve.df(dt);
  REQUIRE_THAT(expected, Catch::Matchers::WithinAbs(0.7672, 0.0001));

  dt = start_date.add_years(7.5);
  expected = curve.df(dt);
  REQUIRE_THAT(expected, Catch::Matchers::WithinAbs(0.6588, 0.0001));

  dt = start_date.add_years(10);
  expected = curve.df(dt);
  REQUIRE_THAT(expected, Catch::Matchers::WithinAbs(0.5584, 0.0001));





}
