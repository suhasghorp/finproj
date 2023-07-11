#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <finproj/curves/IborDeposit.h>
#include <finproj/curves/IborSwap.h>
#include <finproj/curves/IborFuture.h>
#include <finproj/curves/IborSingleCurve.h>
#include <iostream>

TEST_CASE( "test_ibor_single_curve", "[single-file]" ){
  ChronoDate val_date{2018,6,6};
  InterpTypes interp_type{InterpTypes::FLAT_FWD_RATES};

  std::vector<IborDeposit> depos{};
  //deposits
  auto spot_days = 0;
  auto settlement_date = val_date.add_weekdays(spot_days); //check this
  auto depoDCCType = DayCountTypes::ACT_360;
  auto deposit_rate = 0.0231381;
  auto maturity_date = settlement_date.add_months(3);
  depos.emplace_back(settlement_date, maturity_date, deposit_rate, depoDCCType);

  //Futures
  std::vector<IborFuture> futs{};
  futs.emplace_back(IborFuture(val_date, 1));
  futs.emplace_back(IborFuture(val_date, 2));
  futs.emplace_back(IborFuture(val_date, 3));
  futs.emplace_back(IborFuture(val_date, 4));
  futs.emplace_back(IborFuture(val_date, 5));
  futs.emplace_back(IborFuture(val_date, 6));
  //FRAs
  std::vector<IborFRA> fras{};
  fras.reserve(futs.size());
  fras.push_back(futs.at(0).to_fra(97.6675, -0.00005));
  fras.push_back(futs.at(1).to_fra(97.5200, -0.00060));
  fras.push_back(futs.at(2).to_fra(97.3550, -0.00146));
  fras.push_back(futs.at(3).to_fra(97.2450, -0.00263));
  fras.push_back(futs.at(4).to_fra(97.1450, -0.00411));
  fras.push_back(futs.at(5).to_fra(97.0750, -0.00589));

  auto accrual = DayCountTypes::THIRTY_E_360;
  auto freq = FrequencyTypes::SEMI_ANNUAL;
  spot_days = 2;
  settlement_date = val_date.add_weekdays(spot_days);
  auto notional = 1'000'000;
  auto fixed_leg_type = SwapTypes::PAY;

  //Swaps
  std::vector<IborSwap> swaps{};
  swaps.emplace_back(IborSwap(settlement_date, "2Y", fixed_leg_type,(2.77417 + 2.77844) / 200, freq, accrual));
  swaps.emplace_back(IborSwap(settlement_date, "3Y", fixed_leg_type,(2.86098 + 2.86582) / 200, freq, accrual));
  swaps.emplace_back(IborSwap(settlement_date, "4Y", fixed_leg_type,(2.90240 + 2.90620) / 200, freq, accrual));
  swaps.emplace_back(IborSwap(settlement_date, "5Y", fixed_leg_type,(2.92944 + 2.92906) / 200, freq, accrual));
  swaps.emplace_back(IborSwap(settlement_date, "6Y", fixed_leg_type,(2.94001 + 2.94499) / 200, freq, accrual));
  swaps.emplace_back(IborSwap(settlement_date, "7Y", fixed_leg_type,(2.95352 + 2.95998) / 200, freq, accrual));
  swaps.emplace_back(IborSwap(settlement_date, "8Y", fixed_leg_type,(2.96830 + 2.97400) / 200, freq, accrual));
  swaps.emplace_back(IborSwap(settlement_date, "9Y", fixed_leg_type,(2.98403 + 2.98817) / 200, freq, accrual));
  swaps.emplace_back(IborSwap(settlement_date, "10Y", fixed_leg_type,(2.99716 + 3.00394) / 200, freq, accrual));
  swaps.emplace_back(IborSwap(settlement_date, "11Y", fixed_leg_type,(3.01344 + 3.01596) / 200, freq, accrual));
  swaps.emplace_back(IborSwap(settlement_date, "12Y", fixed_leg_type,(3.02276 + 3.02684) / 200, freq, accrual));
  swaps.emplace_back(IborSwap(settlement_date, "15Y", fixed_leg_type,(3.04092 + 3.04508) / 200, freq, accrual));
  swaps.emplace_back(IborSwap(settlement_date, "20Y", fixed_leg_type,(3.04417 + 3.05183) / 200, freq, accrual));
  swaps.emplace_back(IborSwap(settlement_date, "25Y", fixed_leg_type,(3.03219 + 3.03621) / 200, freq, accrual));
  swaps.emplace_back(IborSwap(settlement_date, "30Y", fixed_leg_type,(3.01030 + 3.01370) / 200, freq, accrual));
  swaps.emplace_back(IborSwap(settlement_date, "40Y", fixed_leg_type,(2.96946 + 2.97354) / 200, freq, accrual));
  swaps.emplace_back(IborSwap(settlement_date, "50Y", fixed_leg_type,(2.91552 + 2.93748) / 200, freq, accrual));

  auto libor_curve = IborSingleCurve(val_date, depos, fras, swaps, interp_type);
  //The valuation of 53714.55 is very close to the spreadsheet value 53713.96
  auto principal = 0.0f;
  std::optional<DiscountCurve> idx_optional = std::nullopt;
  std::optional<double> ffr_optional = std::nullopt;
  auto actual = swaps[0].value(val_date, libor_curve, idx_optional, ffr_optional);
  REQUIRE_THAT(actual, Catch::Matchers::WithinAbs(0.0, 0.0001));
  actual = -swaps[0].get_fixed_leg().value(val_date, libor_curve);
  REQUIRE_THAT(actual, Catch::Matchers::WithinAbs(53707.6667, 0.0001));
  actual = swaps[0].get_double_leg().value(val_date, libor_curve, libor_curve, ffr_optional);
  REQUIRE_THAT(actual, Catch::Matchers::WithinAbs(53707.6667, 0.0001));

  actual = swaps[0].value(settlement_date, libor_curve, idx_optional, ffr_optional);
  REQUIRE_THAT(actual, Catch::Matchers::WithinAbs(0.0, 0.0001));
  actual = -swaps[0].get_fixed_leg().value(settlement_date, libor_curve);
  REQUIRE_THAT(actual, Catch::Matchers::WithinAbs(53714.5507, 0.0001));
  actual = swaps[0].get_double_leg().value(settlement_date, libor_curve, libor_curve, ffr_optional);
  REQUIRE_THAT(actual, Catch::Matchers::WithinAbs(53714.5507, 0.0001));

}

TEST_CASE( "test_ibor_again", "[single-file]" ){
  ChronoDate val_date{2023,6,29};
  InterpTypes interp_type{InterpTypes::FLAT_FWD_RATES};

  std::vector<IborDeposit> depos{};
  //deposits
  auto spot_days = 0;
  auto settlement_date = val_date.add_weekdays(spot_days); //check this

  auto depoDCCType = DayCountTypes::ACT_360;
  auto deposit_rate = 0.0231381;
  auto maturity_date = settlement_date.add_months(3);
  depos.emplace_back(settlement_date, maturity_date, deposit_rate, depoDCCType);

  //Futures
  std::vector<IborFuture> futs{};
  futs.emplace_back(IborFuture(val_date, 1));
  futs.emplace_back(IborFuture(val_date, 2));
  futs.emplace_back(IborFuture(val_date, 3));
  futs.emplace_back(IborFuture(val_date, 4));
  futs.emplace_back(IborFuture(val_date, 5));
  futs.emplace_back(IborFuture(val_date, 6));
  //FRAs
  std::vector<IborFRA> fras{};
  fras.reserve(futs.size());
  fras.push_back(futs.at(0).to_fra(97.6675, -0.00005));
  fras.push_back(futs.at(1).to_fra(97.5200, -0.00060));
  fras.push_back(futs.at(2).to_fra(97.3550, -0.00146));
  fras.push_back(futs.at(3).to_fra(97.2450, -0.00263));
  fras.push_back(futs.at(4).to_fra(97.1450, -0.00411));
  fras.push_back(futs.at(5).to_fra(97.0750, -0.00589));

  auto accrual = DayCountTypes::THIRTY_E_360;
  auto freq = FrequencyTypes::SEMI_ANNUAL;
  spot_days = 2;
  settlement_date = val_date.add_weekdays(spot_days);
  auto notional = 1'000'000;
  auto fixed_leg_type = SwapTypes::PAY;

  //Swaps
  std::vector<IborSwap> swaps{};
  swaps.emplace_back(IborSwap(settlement_date, "2Y", fixed_leg_type,(2.77417 + 2.77844) / 200, freq, accrual));
  std::cout << swaps[0].get_termination_date() <<","<< (2.77417 + 2.77844) / 200 << std::endl;
  swaps.emplace_back(IborSwap(settlement_date, "3Y", fixed_leg_type,(2.86098 + 2.86582) / 200, freq, accrual));
  std::cout << swaps[1].get_termination_date() <<","<< (2.86098 + 2.86582) / 200 << std::endl;
  swaps.emplace_back(IborSwap(settlement_date, "4Y", fixed_leg_type,(2.90240 + 2.90620) / 200, freq, accrual));
  std::cout << swaps[2].get_termination_date() <<","<< (2.90240 + 2.90620) / 200 << std::endl;
  swaps.emplace_back(IborSwap(settlement_date, "5Y", fixed_leg_type,(2.92944 + 2.92906) / 200, freq, accrual));
  std::cout << swaps[3].get_termination_date() <<","<< (2.92944 + 2.92906) / 200 << std::endl;
  swaps.emplace_back(IborSwap(settlement_date, "6Y", fixed_leg_type,(2.94001 + 2.94499) / 200, freq, accrual));
  std::cout << swaps[4].get_termination_date() <<","<< (2.94001 + 2.94499) / 200 << std::endl;
  swaps.emplace_back(IborSwap(settlement_date, "7Y", fixed_leg_type,(2.95352 + 2.95998) / 200, freq, accrual));
  std::cout << swaps[5].get_termination_date() <<","<< (2.95352 + 2.95998) / 200 << std::endl;
  swaps.emplace_back(IborSwap(settlement_date, "8Y", fixed_leg_type,(2.96830 + 2.97400) / 200, freq, accrual));
  std::cout << swaps[6].get_termination_date() <<","<< (2.96830 + 2.97400) / 200 << std::endl;
  swaps.emplace_back(IborSwap(settlement_date, "9Y", fixed_leg_type,(2.98403 + 2.98817) / 200, freq, accrual));
  std::cout << swaps[7].get_termination_date() <<","<< (2.98403 + 2.98817) / 200 << std::endl;
  swaps.emplace_back(IborSwap(settlement_date, "10Y", fixed_leg_type,(2.99716 + 3.00394) / 200, freq, accrual));
  std::cout << swaps[8].get_termination_date() <<","<< (2.99716 + 3.00394) / 200 << std::endl;
  swaps.emplace_back(IborSwap(settlement_date, "11Y", fixed_leg_type,(3.01344 + 3.01596) / 200, freq, accrual));
  std::cout << swaps[9].get_termination_date() <<","<< (3.01344 + 3.01596) / 200 << std::endl;
  swaps.emplace_back(IborSwap(settlement_date, "12Y", fixed_leg_type,(3.02276 + 3.02684) / 200, freq, accrual));
  std::cout << swaps[10].get_termination_date() <<","<< (3.02276 + 3.02684) / 200 << std::endl;
  swaps.emplace_back(IborSwap(settlement_date, "15Y", fixed_leg_type,(3.04092 + 3.04508) / 200, freq, accrual));
  std::cout << swaps[11].get_termination_date() <<","<< (3.04092 + 3.04508) / 200 << std::endl;
  swaps.emplace_back(IborSwap(settlement_date, "20Y", fixed_leg_type,(3.04417 + 3.05183) / 200, freq, accrual));
  std::cout << swaps[12].get_termination_date() <<","<< (3.04417 + 3.05183) / 200 << std::endl;
  swaps.emplace_back(IborSwap(settlement_date, "25Y", fixed_leg_type,(3.03219 + 3.03621) / 200, freq, accrual));
  std::cout << swaps[13].get_termination_date() <<","<< (3.03219 + 3.03621) / 200 << std::endl;
  swaps.emplace_back(IborSwap(settlement_date, "30Y", fixed_leg_type,(3.01030 + 3.01370) / 200, freq, accrual));
  std::cout << swaps[14].get_termination_date() <<","<< (3.01030 + 3.01370) / 200 << std::endl;
  swaps.emplace_back(IborSwap(settlement_date, "40Y", fixed_leg_type,(2.96946 + 2.97354) / 200, freq, accrual));
  std::cout << swaps[15].get_termination_date() <<","<< (2.96946 + 2.97354) / 200 << std::endl;
  swaps.emplace_back(IborSwap(settlement_date, "50Y", fixed_leg_type,(2.91552 + 2.93748) / 200, freq, accrual));
  std::cout << swaps[16].get_termination_date() <<","<< (2.91552 + 2.93748) / 200 << std::endl;

  auto libor_curve = IborSingleCurve(val_date, depos, fras, swaps, interp_type);
  //The valuation of 53714.55 is very close to the spreadsheet value 53713.96
  auto principal = 0.0f;
  std::optional<DiscountCurve> idx_optional = std::nullopt;
  std::optional<double> ffr_optional = std::nullopt;
  auto actual = swaps[0].value(val_date, libor_curve, idx_optional, ffr_optional);
  REQUIRE_THAT(actual, Catch::Matchers::WithinAbs(0.0, 0.0001));
  actual = -swaps[0].get_fixed_leg().value(val_date, libor_curve);
  //REQUIRE_THAT(actual, Catch::Matchers::WithinAbs(53707.6667, 0.0001));
  REQUIRE_THAT(actual, Catch::Matchers::WithinAbs(53726.1678, 0.001));
  actual = swaps[0].get_double_leg().value(val_date, libor_curve, libor_curve, ffr_optional);
  REQUIRE_THAT(actual, Catch::Matchers::WithinAbs(53726.1678, 0.001));

  actual = swaps[0].value(settlement_date, libor_curve, idx_optional, ffr_optional);
  REQUIRE_THAT(actual, Catch::Matchers::WithinAbs(0.0, 0.0001));
  actual = -swaps[0].get_fixed_leg().value(settlement_date, libor_curve);
  REQUIRE_THAT(actual, Catch::Matchers::WithinAbs(53739.9414, 0.001));
  actual = swaps[0].get_double_leg().value(settlement_date, libor_curve, libor_curve, ffr_optional);
  REQUIRE_THAT(actual, Catch::Matchers::WithinAbs(53739.9414, 0.001));

}
