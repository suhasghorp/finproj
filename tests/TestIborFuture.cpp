#include <catch2/catch_test_macros.hpp>
#include <finproj/curves/IborFuture.h>

TEST_CASE( "test_fin_ibor_future", "[single-file]" ){
  auto i{1};
  auto today = ChronoDate{2020,5,5};
  auto fut = IborFuture(today, i, "3M");
  auto fra = fut.to_fra(0.020, 0.0);
  auto expected = ChronoDate{2020,6,17};
  auto actual = fut.get_delivery_date();
  REQUIRE(actual == expected); //delivery date
  expected = ChronoDate{2020,6,17};
  actual = fra.get_start_date();
  REQUIRE(actual == expected); //start date

  i = 4;
  fut = IborFuture(today, i, "3M");
  fra = fut.to_fra(0.020, 0.0);
  expected = ChronoDate{2021,3,17};
  actual = fut.get_delivery_date();
  REQUIRE(actual == expected); //delivery date
  expected = ChronoDate{2021,3,17};
  actual = fra.get_start_date();
  REQUIRE(actual == expected); //start date

  i = 7;
  fut = IborFuture(today, i, "3M");
  fra = fut.to_fra(0.020, 0.0);
  expected = ChronoDate{2021,12,15};
  actual = fut.get_delivery_date();
  REQUIRE(actual == expected); //delivery date
  expected = ChronoDate{2021,12,15};
  actual = fra.get_start_date();
  REQUIRE(actual == expected); //start date

  i = 10;
  fut = IborFuture(today, i, "3M");
  fra = fut.to_fra(0.020, 0.0);
  expected = ChronoDate{2022,9,21};
  actual = fut.get_delivery_date();
  REQUIRE(actual == expected); //delivery date
  expected = ChronoDate{2022,9,21};
  actual = fra.get_start_date();
  REQUIRE(actual == expected); //start date
}
