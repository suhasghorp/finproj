#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <finproj/curves/CDS.h>
#include <finproj/curves/IborDeposit.h>
#include <finproj/curves/IborFuture.h>
#include <finproj/curves/IborSingleCurve.h>
#include <finproj/curves/IborSwap.h>
#include <tuple>


TEST_CASE( "test_credit_curve", "[single-file]" ){
  ChronoDate curve_date{2018,12,20};
  auto fixedDCC = DayCountTypes::ACT_365F;
  auto fixedFreq = FrequencyTypes::SEMI_ANNUAL;
  auto fixed_coupon = 0.05;
  std::vector<IborDeposit> depos{};
  std::vector<IborFRA> fras{};
  std::vector<IborSwap> swaps{};
  std::vector<CDS> cds_contracts{};
  for (int i{1}; i < 11; ++i) {
    auto maturity_date = curve_date.add_months(12 * i);
    auto swap = IborSwap(curve_date,
                         maturity_date,
                         SwapTypes::PAY,
                         fixed_coupon,
                         fixedFreq,
                         fixedDCC);
    swaps.emplace_back(swap);
  }
  auto libor_curve = IborSingleCurve(curve_date, depos, fras, swaps);
  for (int i{1}; i < 11; ++i){
    auto maturity_date = curve_date.add_months(12 * i);
    auto cds = CDS(curve_date, maturity_date, 0.005 + 0.001 * (i - 1));
    cds_contracts.emplace_back(cds);
  }
  auto recovery_rate = 0.40;

  auto issuer_curve = CreditCurve(curve_date,"XYZ", cds_contracts,libor_curve,recovery_rate);

  REQUIRE_THAT(issuer_curve.times_[0], Catch::Matchers::WithinAbs(0.0, 0.0001));
  REQUIRE_THAT(issuer_curve.times_[5], Catch::Matchers::WithinAbs(5.0027, 0.0001));
  REQUIRE_THAT(issuer_curve.times_[9], Catch::Matchers::WithinAbs(9.0055, 0.0001));
  REQUIRE_THAT(issuer_curve.values_[0], Catch::Matchers::WithinAbs(1.0, 0.0001));
  REQUIRE_THAT(issuer_curve.values_[5], Catch::Matchers::WithinAbs(0.9249, 0.0001));
  REQUIRE_THAT(issuer_curve.values_[9], Catch::Matchers::WithinAbs(0.8072, 0.0001));

  auto i = 1;
  auto maturity_date = curve_date.add_months(12 * i);
  auto cds = CDS(curve_date, maturity_date, 0.005 + 0.001 * (i - 1));
  auto v = cds.value(curve_date, issuer_curve, recovery_rate);
  REQUIRE_THAT(std::get<0>(v) * 1000, Catch::Matchers::WithinAbs(5.6111, 0.0001)); //full PV
  REQUIRE_THAT(std::get<1>(v) * 1000, Catch::Matchers::WithinAbs(5.6111, 0.0001)); //clean PV

  i = 5;
  maturity_date = curve_date.add_months(12 * i);
  cds = CDS(curve_date, maturity_date, 0.005 + 0.001 * (i - 1));
  v = cds.value(curve_date, issuer_curve, recovery_rate);
  REQUIRE_THAT(std::get<0>(v) * 1000, Catch::Matchers::WithinAbs(8.5117, 0.0001)); //full PV
  REQUIRE_THAT(std::get<1>(v) * 1000, Catch::Matchers::WithinAbs(8.5117, 0.0001)); //clean PV

  i = 10;
  maturity_date = curve_date.add_months(12 * i);
  cds = CDS(curve_date, maturity_date, 0.005 + 0.001 * (i - 1));
  v = cds.value(curve_date, issuer_curve, recovery_rate);
  REQUIRE_THAT(std::get<0>(v) * 1000, Catch::Matchers::WithinAbs(0.0, 0.0001)); //full PV
  REQUIRE_THAT(std::get<1>(v) * 1000, Catch::Matchers::WithinAbs(0.0, 0.0001)); //clean PV



}
