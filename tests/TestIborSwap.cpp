#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <finproj/curves/IborDeposit.h>
#include <finproj/curves/IborSwap.h>
#include <finproj/curves/IborFuture.h>
#include <finproj/curves/IborSingleCurve.h>

static IborSingleCurve build_ibor_curve(const ChronoDate& val_date){
  std::vector<IborDeposit> depos{};
  std::vector<IborFRA> fras{};
  std::vector<IborSwap> swaps{};

  auto settlement_date = val_date.add_days(2);
  auto dc_type = DayCountTypes::ACT_360;

  auto maturity_date = settlement_date.add_months(1);
  auto depo1 = IborDeposit(val_date, maturity_date, -0.00251, dc_type);
  depos.push_back(depo1);

  //Series of 1M futures
  auto start_date = settlement_date.next_imm_date();
  auto end_date = start_date.add_months(1);
  auto fra = IborFRA(start_date, end_date, -0.0023, dc_type);
  fras.push_back(fra);

  start_date = start_date.add_months(1);
  end_date = start_date.add_months(1);
  fra = IborFRA(start_date, end_date, -0.00234, dc_type);
  fras.push_back(fra);

  start_date = start_date.add_months(1);
  end_date = start_date.add_months(1);
  fra = IborFRA(start_date, end_date, -0.00225, dc_type);
  fras.push_back(fra);

  start_date = start_date.add_months(1);
  end_date = start_date.add_months(1);
  fra = IborFRA(start_date, end_date, -0.00226, dc_type);
  fras.push_back(fra);

  start_date = start_date.add_months(1);
  end_date = start_date.add_months(1);
  fra = IborFRA(start_date, end_date, -0.00219, dc_type);
  fras.push_back(fra);

  start_date = start_date.add_months(1);
  end_date = start_date.add_months(1);
  fra = IborFRA(start_date, end_date, -0.00213, dc_type);
  fras.push_back(fra);

  start_date = start_date.add_months(1);
  end_date = start_date.add_months(1);
  fra = IborFRA(start_date, end_date, -0.00186, dc_type);
  fras.push_back(fra);

  start_date = start_date.add_months(1);
  end_date = start_date.add_months(1);
  fra = IborFRA(start_date, end_date, -0.00189, dc_type);
  fras.push_back(fra);

  start_date = start_date.add_months(1);
  end_date = start_date.add_months(1);
  fra = IborFRA(start_date, end_date, -0.00175, dc_type);
  fras.push_back(fra);

  start_date = start_date.add_months(1);
  end_date = start_date.add_months(1);
  fra = IborFRA(start_date, end_date, -0.00143, dc_type);
  fras.push_back(fra);

  start_date = start_date.add_months(1);
  end_date = start_date.add_months(1);
  fra = IborFRA(start_date, end_date, -0.00126, dc_type);
  fras.push_back(fra);

  start_date = start_date.add_months(1);
  end_date = start_date.add_months(1);
  fra = IborFRA(start_date, end_date, -0.00126, dc_type);
  fras.push_back(fra);

  /////////////////////////////////////////////

  auto fixedFreq = FrequencyTypes::ANNUAL;
  dc_type = DayCountTypes::THIRTY_E_360;
  auto fixed_leg_type = SwapTypes::PAY;

  maturity_date = settlement_date.add_months(24);
  auto swap_rate = -0.001506;
  auto swap1 = IborSwap(settlement_date, maturity_date, fixed_leg_type,
                                                   swap_rate, fixedFreq, dc_type);
  swaps.push_back(swap1);

  maturity_date = settlement_date.add_months(36);
  swap_rate = -0.000185;
  auto swap2 = IborSwap(settlement_date, maturity_date, fixed_leg_type,
                                                   swap_rate, fixedFreq, dc_type);
  swaps.push_back(swap2);

  maturity_date = settlement_date.add_months(48);
  swap_rate = 0.001358;
  auto swap3 = IborSwap(settlement_date, maturity_date, fixed_leg_type,
                       swap_rate, fixedFreq, dc_type);
  swaps.push_back(swap3);

  maturity_date = settlement_date.add_months(60);
  swap_rate = 0.0027652;
  auto swap4 = IborSwap(settlement_date, maturity_date, fixed_leg_type,
                       swap_rate, fixedFreq, dc_type);
  swaps.push_back(swap4);

  maturity_date = settlement_date.add_months(72);
  swap_rate = 0.0041539;
  auto swap5 = IborSwap(settlement_date, maturity_date, fixed_leg_type,
                       swap_rate, fixedFreq, dc_type);
  swaps.push_back(swap5);

  maturity_date = settlement_date.add_months(84);
  swap_rate = 0.0054604;
  auto swap6 = IborSwap(settlement_date, maturity_date, fixed_leg_type,
                       swap_rate, fixedFreq, dc_type);
  swaps.push_back(swap6);

  maturity_date = settlement_date.add_months(96);
  swap_rate = 0.006674;
  auto swap7 = IborSwap(settlement_date, maturity_date, fixed_leg_type,
                       swap_rate, fixedFreq, dc_type);
  swaps.push_back(swap7);

  maturity_date = settlement_date.add_months(108);
  swap_rate = 0.007826;
  auto swap8 = IborSwap(settlement_date, maturity_date, fixed_leg_type,
                       swap_rate, fixedFreq, dc_type);
  swaps.push_back(swap8);

  maturity_date = settlement_date.add_months(120);
  swap_rate = 0.008821;
  auto swap9 = IborSwap(settlement_date, maturity_date, fixed_leg_type,
                       swap_rate, fixedFreq, dc_type);
  swaps.push_back(swap9);

  maturity_date = settlement_date.add_months(132);
  swap_rate = 0.0097379;
  auto swap10 = IborSwap(settlement_date, maturity_date, fixed_leg_type,
                        swap_rate, fixedFreq, dc_type);
  swaps.push_back(swap10);

  maturity_date = settlement_date.add_months(144);
  swap_rate = 0.0105406;
  auto swap11 = IborSwap(settlement_date, maturity_date, fixed_leg_type,
                        swap_rate, fixedFreq, dc_type);
  swaps.push_back(swap11);

  maturity_date = settlement_date.add_months(180);
  swap_rate = 0.0123927;
  auto swap12 = IborSwap(settlement_date, maturity_date, fixed_leg_type,
                        swap_rate, fixedFreq, dc_type);
  swaps.push_back(swap12);

  maturity_date = settlement_date.add_months(240);
  swap_rate = 0.0139882;
  auto swap13 = IborSwap(settlement_date, maturity_date, fixed_leg_type,
                        swap_rate, fixedFreq, dc_type);
  swaps.push_back(swap13);

  maturity_date = settlement_date.add_months(300);
  swap_rate = 0.0144972;
  auto swap14 = IborSwap(settlement_date, maturity_date, fixed_leg_type,
                        swap_rate, fixedFreq, dc_type);
  swaps.push_back(swap14);

  maturity_date = settlement_date.add_months(360);
  swap_rate = 0.0146081;
  auto swap15 = IborSwap(settlement_date, maturity_date, fixed_leg_type,
                        swap_rate, fixedFreq, dc_type);
  swaps.push_back(swap15);

  maturity_date = settlement_date.add_months(420);
  swap_rate = 0.01461897;
  auto swap16 = IborSwap(settlement_date, maturity_date, fixed_leg_type,
                        swap_rate, fixedFreq, dc_type);
  swaps.push_back(swap16);

  maturity_date = settlement_date.add_months(480);
  swap_rate = 0.014567455;
  auto swap17 = IborSwap(settlement_date, maturity_date, fixed_leg_type,
                        swap_rate, fixedFreq, dc_type);
  swaps.push_back(swap17);

  maturity_date = settlement_date.add_months(540);
  swap_rate = 0.0140826;
  auto swap18 = IborSwap(settlement_date, maturity_date, fixed_leg_type,
                        swap_rate, fixedFreq, dc_type);
  swaps.push_back(swap18);

  maturity_date = settlement_date.add_months(600);
  swap_rate = 0.01436822;
  auto swap19 = IborSwap(settlement_date, maturity_date, fixed_leg_type,
                        swap_rate, fixedFreq, dc_type);
  swaps.push_back(swap19);

  auto libor_curve = IborSingleCurve(val_date, depos, fras, swaps, InterpTypes::FLAT_FWD_RATES, true);
  return libor_curve;
}

TEST_CASE( "test_LiborSwap", "[single-file]" ){
  ChronoDate start_date{2017,12,27};
  ChronoDate end_date{2067,12,27};
  auto fixed_coupon = 0.015f;
  auto fixedFreqType = FrequencyTypes::ANNUAL;
  auto fixed_day_count_type = DayCountTypes::THIRTY_E_360;
  auto double_spread = 0.0;
  auto doubleFreqType = FrequencyTypes::SEMI_ANNUAL;
  auto double_day_count_type = DayCountTypes::ACT_360;
  auto firstFixing = -0.00268f;
  auto swapCalendarType = CalendarTypes::WEEKEND;
  auto bus_day_adjust_type = BusDayAdjustTypes::FOLLOWING;
  auto date_gen_rule_type = DateGenRuleTypes::BACKWARD;
  auto fixed_leg_type = SwapTypes::RECEIVE;
  auto notional = 10.0 * 1'000'000;
  auto swap = IborSwap(start_date,
                       end_date,
                       fixed_leg_type,
                       fixed_coupon,
                       fixedFreqType,
                       fixed_day_count_type,
                       notional,
                       double_spread,
                       doubleFreqType,
                       double_day_count_type,
                       swapCalendarType,
                       bus_day_adjust_type,
                       date_gen_rule_type);
  ChronoDate valuation_date{2018,11,30};
  auto settlement_date = valuation_date.add_days(2);
  auto libor_curve = build_ibor_curve(valuation_date);
  auto v = swap.value(settlement_date, libor_curve, libor_curve, firstFixing);
  REQUIRE_THAT(v, Catch::Matchers::WithinAbs(318901.4791, 0.0001));
}

