#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "finproj/utils/Interpolator.h"
#include "finproj/utils/Misc.h"
#include <vector>
#include <cmath>

const std::vector<double> xValues{0.25, 0.5, 0.75, 1.0, 2.0, 3.0, 5.0, 10.0};
const double a = -0.1;
const double b = 0.002;

const std::vector<double> yValues = [] {
  auto z = decltype(yValues){};
  for (int i = 0; i < xValues.size(); ++i) {
    z.push_back(exp(a * xValues[i] + b * xValues[i] * xValues[i]));
  }
  return z;
}();

auto xInterpolateValues = linspace(0.0, 10.0, 20);

TEST_CASE( "test_FLAT_FWD_RATES", "[single-file]" ){
  auto interp_type = InterpTypes::FLAT_FWD_RATES;
  Interpolator interpolator{xValues,yValues,interp_type};

  auto index = 0;
  auto x = xInterpolateValues[index];
  auto y_int = interpolator.interpolate(x);
  REQUIRE_THAT(x,Catch::Matchers::WithinAbs(0.0, 0.0001));
  REQUIRE_THAT(y_int,Catch::Matchers::WithinAbs(1.0, 0.0001));

  index = 5;
  x = xInterpolateValues[index];
  y_int = interpolator.interpolate(x);
  REQUIRE_THAT(x,Catch::Matchers::WithinAbs(2.6316, 0.0001));
  REQUIRE_THAT(y_int,Catch::Matchers::WithinAbs(0.7797, 0.0001));

  index = 10;
  x = xInterpolateValues[index];
  y_int = interpolator.interpolate(x);
  REQUIRE_THAT(x,Catch::Matchers::WithinAbs(5.2632, 0.0001));
  REQUIRE_THAT(y_int,Catch::Matchers::WithinAbs(0.6260, 0.0001));

}

TEST_CASE( "test_LINEAR_FWD_RATES", "[single-file]" ){
  auto interp_type = InterpTypes::LINEAR_FWD_RATES;
  Interpolator interpolator{xValues,yValues,interp_type};

  auto index = 3;
  auto x = xInterpolateValues[index];
  auto y_int = interpolator.interpolate(x);
  REQUIRE_THAT(x,Catch::Matchers::WithinAbs(1.5789, 0.0001));
  REQUIRE_THAT(y_int,Catch::Matchers::WithinAbs(0.8581, 0.0001));

  index = 15;
  x = xInterpolateValues[index];
  y_int = interpolator.interpolate(x);
  REQUIRE_THAT(x,Catch::Matchers::WithinAbs(7.8947, 0.0001));
  REQUIRE_THAT(y_int,Catch::Matchers::WithinAbs(0.5119, 0.0001));

  index = 19;
  x = xInterpolateValues[index];
  y_int = interpolator.interpolate(x);
  REQUIRE_THAT(x,Catch::Matchers::WithinAbs(10.0, 0.0001));
  REQUIRE_THAT(y_int,Catch::Matchers::WithinAbs(0.4493, 0.0001));

}

TEST_CASE( "test_LINEAR_ZERO_RATES", "[single-file]" ){
  auto interp_type = InterpTypes::LINEAR_ZERO_RATES;
  Interpolator interpolator{xValues,yValues,interp_type};

  auto index = 8;
  auto x = xInterpolateValues[index];
  auto y_int = interpolator.interpolate(x);
  REQUIRE_THAT(x,Catch::Matchers::WithinAbs(4.2105, 0.0001));
  REQUIRE_THAT(y_int,Catch::Matchers::WithinAbs(0.6800, 0.0001));

  index = 13;
  x = xInterpolateValues[index];
  y_int = interpolator.interpolate(x);
  REQUIRE_THAT(x,Catch::Matchers::WithinAbs(6.8421, 0.0001));
  REQUIRE_THAT(y_int,Catch::Matchers::WithinAbs(0.5540, 0.0001));

  index = 18;
  x = xInterpolateValues[index];
  y_int = interpolator.interpolate(x);
  REQUIRE_THAT(x,Catch::Matchers::WithinAbs(9.4737, 0.0001));
  REQUIRE_THAT(y_int,Catch::Matchers::WithinAbs(0.4640, 0.0001));

}

TEST_CASE( "test_FINCUBIC_ZERO_RATES", "[single-file]" ){
  auto interp_type = InterpTypes::FINCUBIC_ZERO_RATES;
  Interpolator interpolator{xValues,yValues,interp_type};

  auto index = 1;
  auto x = xInterpolateValues[index];
  auto y_int = interpolator.interpolate(x);
  REQUIRE_THAT(x,Catch::Matchers::WithinAbs(0.5263, 0.0001));
  REQUIRE_THAT(y_int,Catch::Matchers::WithinAbs(0.9493, 0.01));

  index = 6;
  x = xInterpolateValues[index];
  y_int = interpolator.interpolate(x);
  REQUIRE_THAT(x,Catch::Matchers::WithinAbs(3.1579, 0.0001));
  REQUIRE_THAT(y_int,Catch::Matchers::WithinAbs(0.7439, 0.01));

  index = 11;
  x = xInterpolateValues[index];
  y_int = interpolator.interpolate(x);
  REQUIRE_THAT(x,Catch::Matchers::WithinAbs(5.7895, 0.0001));
  REQUIRE_THAT(y_int,Catch::Matchers::WithinAbs(0.6007, 0.01));

}
