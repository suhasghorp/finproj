#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <finproj/curves/CDS.h>
#include <finproj/curves/IborDeposit.h>
#include <finproj/curves/IborFuture.h>
#include <finproj/curves/IborSingleCurve.h>
#include <finproj/curves/IborSwap.h>
#include <finproj/utils/Misc.h>
#include <tuple>
#include <cmath>
#include <array>
#include <map>

static CreditCurve build_issuer_curve_one(double mkt_spread_bump, double ir_bump){
    ChronoDate trade_date{2019,8,9};
    auto valuation_date = trade_date.add_days(1);
    auto m = 1.0; //0.00000000000
    DayCountTypes dc_type {DayCountTypes::ACT_360};
    std::vector<IborDeposit> depos{};
    std::vector<IborFRA> fras{};
    std::vector<IborSwap> swaps{};
    std::vector<CDS> cds_contracts{};

    auto depo1 = IborDeposit(valuation_date, "1D", m * 0.0220, dc_type);
    depos.push_back(depo1);

    auto spot_days = 2;
    auto settlement_date = valuation_date.add_days(spot_days);

    auto maturity_date = settlement_date.add_months(1);
    depo1 = IborDeposit(settlement_date, maturity_date, m * 0.022009, dc_type);
    depos.push_back(depo1);

    maturity_date = settlement_date.add_months(2);
    auto depo2 = IborDeposit(settlement_date, maturity_date, m * 0.022138, dc_type);
    depos.push_back(depo2);

    maturity_date = settlement_date.add_months(3);
    auto depo3 = IborDeposit(settlement_date, maturity_date, m * 0.021810, dc_type);
    depos.push_back(depo3);

    maturity_date = settlement_date.add_months(6);
    auto depo4 = IborDeposit(settlement_date, maturity_date, m * 0.020503, dc_type);
    depos.push_back(depo4);

    maturity_date = settlement_date.add_months(12);
    auto depo5 = IborDeposit(settlement_date, maturity_date, m * 0.019930, dc_type);
    depos.push_back(depo5);

    dc_type = DayCountTypes::THIRTY_E_360_ISDA;
    FrequencyTypes fixedFreq{FrequencyTypes::SEMI_ANNUAL};

    maturity_date = settlement_date.add_months(24);
    auto swap1 = IborSwap(settlement_date,maturity_date,SwapTypes::PAY,m * 0.015910 + ir_bump,fixedFreq,dc_type);
    swaps.push_back(swap1);

    maturity_date = settlement_date.add_months(36);
    auto swap2 = IborSwap(settlement_date,maturity_date,SwapTypes::PAY,m * 0.014990 + ir_bump,fixedFreq,dc_type);
    swaps.push_back(swap2);

    maturity_date = settlement_date.add_months(48);
    auto swap3 = IborSwap(settlement_date,maturity_date,SwapTypes::PAY,m * 0.014725 + ir_bump,fixedFreq,dc_type);
    swaps.push_back(swap3);

    maturity_date = settlement_date.add_months(60);
    auto swap4 = IborSwap(settlement_date,maturity_date,SwapTypes::PAY,m * 0.014640 + ir_bump,fixedFreq,dc_type);
    swaps.push_back(swap4);

    maturity_date = settlement_date.add_months(72);
    auto swap5 = IborSwap(settlement_date,maturity_date,SwapTypes::PAY,m * 0.014800 + ir_bump,fixedFreq,dc_type);
    swaps.push_back(swap5);

    maturity_date = settlement_date.add_months(84);
    auto swap6 = IborSwap(settlement_date,maturity_date,SwapTypes::PAY,m * 0.014995 + ir_bump,fixedFreq,dc_type);
    swaps.push_back(swap6);

    maturity_date = settlement_date.add_months(96);
    auto swap7 = IborSwap(settlement_date,maturity_date,SwapTypes::PAY,m * 0.015180 + ir_bump,fixedFreq,dc_type);
    swaps.push_back(swap7);

    maturity_date = settlement_date.add_months(108);
    auto swap8 = IborSwap(settlement_date,maturity_date,SwapTypes::PAY,m * 0.015610 + ir_bump,fixedFreq,dc_type);
    swaps.push_back(swap8);

    maturity_date = settlement_date.add_months(120);
    auto swap9 = IborSwap(settlement_date,maturity_date,SwapTypes::PAY,m * 0.015880 + ir_bump,fixedFreq,dc_type);
    swaps.push_back(swap9);

    maturity_date = settlement_date.add_months(144);
    auto swap10 = IborSwap(settlement_date,maturity_date,SwapTypes::PAY,m * 0.016430 + ir_bump,fixedFreq,dc_type);
    swaps.push_back(swap10);

    auto libor_curve = IborSingleCurve(valuation_date, depos, fras, swaps);

    auto cds_coupon = 0.04 + mkt_spread_bump;

    maturity_date = valuation_date.next_cds_date(6);
    auto cds = CDS(valuation_date, maturity_date, cds_coupon);
    cds_contracts.push_back(cds);

    maturity_date = valuation_date.next_cds_date(12);
    cds = CDS(valuation_date, maturity_date, cds_coupon);
    cds_contracts.push_back(cds);

    maturity_date = valuation_date.next_cds_date(24);
    cds = CDS(valuation_date, maturity_date, cds_coupon);
    cds_contracts.push_back(cds);

    maturity_date = valuation_date.next_cds_date(36);
    cds = CDS(valuation_date, maturity_date, cds_coupon);
    cds_contracts.push_back(cds);

    maturity_date = valuation_date.next_cds_date(48);
    cds = CDS(valuation_date, maturity_date, cds_coupon);
    cds_contracts.push_back(cds);

    maturity_date = valuation_date.next_cds_date(60);
    cds = CDS(valuation_date, maturity_date, cds_coupon);
    cds_contracts.push_back(cds);

    maturity_date = valuation_date.next_cds_date(84);
    cds = CDS(valuation_date, maturity_date, cds_coupon);
    cds_contracts.push_back(cds);

    maturity_date = valuation_date.next_cds_date(120);
    cds = CDS(valuation_date, maturity_date, cds_coupon);
    cds_contracts.push_back(cds);

    maturity_date = valuation_date.next_cds_date(180);
    cds = CDS(valuation_date, maturity_date, cds_coupon);
    cds_contracts.push_back(cds);

    auto recovery_rate = 0.40;

    auto issuer_curve = CreditCurve(valuation_date,"ONE", cds_contracts,libor_curve,recovery_rate);
    return issuer_curve;
}

static CreditCurve build_issuer_curve_two(double mkt_spread_bump, double ir_bump){
    auto m = 1.0;
    ChronoDate valuation_date{2020,8,24};
    ChronoDate settlement_date{2020,8,24};
    DayCountTypes dcType{DayCountTypes::ACT_360};
    std::vector<IborDeposit> depos{};
    std::vector<IborFRA> fras{};
    std::vector<IborSwap> swaps{};
    std::vector<CDS> cds_contracts{};

    std::map<unsigned int, double> deposit_rates = {{1,0.001709},{2,0.002123},{3,0.002469},{6,0.003045},{12,0.004449}};
    for (const auto& [key, value] : deposit_rates){
      auto maturity_date = settlement_date.add_months(key);
      depos.push_back(IborDeposit(settlement_date, maturity_date, m * value, dcType));
    }
    dcType = DayCountTypes::THIRTY_E_360_ISDA;
    auto fixedFreq = FrequencyTypes::SEMI_ANNUAL;

    std::map<unsigned int, double> swap_rates = {{24,0.002155},{36,0.002305},{48,0.002665},{60,0.003290}};
    for (const auto& [key, value] : swap_rates){
      auto maturity_date = settlement_date.add_months(key);
      swaps.push_back(IborSwap(settlement_date,maturity_date,SwapTypes::PAY,m * value + ir_bump,fixedFreq,dcType));
    }
    auto libor_curve = IborSingleCurve(valuation_date, depos, fras, swaps);
    auto cdsCoupon = 0.01 + mkt_spread_bump;
    ChronoDate effective_date {2020,8,21};
    std::array<std::string,8> tenors {"6M", "1Y", "2Y", "3Y","4Y", "5Y", "7Y", "10Y"};
    for (std::string tenor : tenors){
      cds_contracts.push_back(CDS(effective_date, std::move(tenor), cdsCoupon));
    }
    auto recovery_rate = 0.40;

    auto issuer_curve = CreditCurve(settlement_date,"TWO", cds_contracts,libor_curve,recovery_rate);
    auto years = linspace(0.0,10.0,20);
    for (auto y : years){
      auto date = settlement_date.add_years(y);
      auto df = libor_curve.df(date);
      auto q = issuer_curve.surv_prob(date);
    }
    return issuer_curve;
}

TEST_CASE( "test_cds_one", "[single-file]" ){
    auto cds_recovery = 0.40;
    auto issuer_curve1 = build_issuer_curve_one(0.0, 0.0);
    //This is the 10 year contract at an off market coupon
    ChronoDate maturity_date{2029,6,20};
    auto cds_coupon = 0.0150;
    auto notional = 1000000.0;
    auto long_protection = true;
    ChronoDate tradeDate{2019,8,9};
    auto valuation_date1 = tradeDate.add_days(1);
    auto cds_contract1 = CDS(valuation_date1,maturity_date,cds_coupon,notional,long_protection);
    auto t = (maturity_date - valuation_date1) / 365.0;
    auto z = issuer_curve1.libor_curve_.df(maturity_date);
    auto r1 = -log(z) / t;
    //print(t, z, r1, maturity_date)
    auto mktSpread1 = 0.040;

    auto spd = cds_contract1.par_spread(valuation_date1,issuer_curve1,cds_recovery) * 10000.0;
    REQUIRE_THAT(spd, Catch::Matchers::WithinAbs(399.9992, 0.0001));

    auto v = cds_contract1.value(valuation_date1, issuer_curve1, cds_recovery);
    REQUIRE_THAT(std::get<0>(v), Catch::Matchers::WithinAbs(168552.827, 0.001));
    REQUIRE_THAT(std::get<1>(v), Catch::Matchers::WithinAbs(170677.827, 0.001));

    auto p = cds_contract1.clean_price(valuation_date1, issuer_curve1, cds_recovery);
    REQUIRE_THAT(p, Catch::Matchers::WithinAbs(82.9322, 0.0001));

    auto accrued_days = cds_contract1.accrued_days();
    REQUIRE_THAT(accrued_days, Catch::Matchers::WithinAbs(51, 0.0001));

    auto accrued_interest = cds_contract1.accrued_interest();
    REQUIRE_THAT(accrued_interest, Catch::Matchers::WithinAbs(-2125.0, 0.0001));

    auto prot_pv = cds_contract1.protection_leg_pv(valuation_date1, issuer_curve1, cds_recovery);
    REQUIRE_THAT(prot_pv, Catch::Matchers::WithinAbs(273084.8405, 0.0001));

    auto prem_pv = cds_contract1.premium_leg_pv(valuation_date1, issuer_curve1);
    REQUIRE_THAT(prem_pv, Catch::Matchers::WithinAbs(104532.0142, 0.0001));

    auto [full_pv, clean_pv, credit01, ir01] = cds_contract1.value_fast_approx(valuation_date1,r1,mktSpread1,cds_recovery, 0.4);
    REQUIRE_THAT(full_pv, Catch::Matchers::WithinAbs(165262.8062, 0.0001));
    REQUIRE_THAT(clean_pv, Catch::Matchers::WithinAbs(167387.8062, 0.0001));
    REQUIRE_THAT(credit01, Catch::Matchers::WithinAbs(555.5746, 0.0001));
    REQUIRE_THAT(ir01, Catch::Matchers::WithinAbs(-71.4881, 0.0001));
}

TEST_CASE( "test_cds_two", "[single-file]" ){
    auto cds_recovery = 0.40;
    auto issuer_curve2 = build_issuer_curve_two(0.0, 0.0);
    //This is the 10 year contract at an off market coupon
    ChronoDate maturity_date{2025,6,20};
    auto cdsCoupon = 0.050;
    auto notional = 1000000.0;
    auto long_protection = true;
    ChronoDate tradeDate{2020,8,20};
    ChronoDate effective_date{2020,8,21};
    auto valuation_date2 = tradeDate;
    auto cds_contract2 = CDS(effective_date,maturity_date,cdsCoupon,notional,long_protection);
    auto t = (maturity_date - valuation_date2) / 365.0;
    auto z = issuer_curve2.libor_curve_.df(maturity_date);
    auto r2 = -log(z) / t;
    auto mktSpread2 = 0.01;

    auto spd = cds_contract2.par_spread(valuation_date2,issuer_curve2,cds_recovery) * 10000.0;
    REQUIRE_THAT(spd, Catch::Matchers::WithinAbs(100.1772, 0.0001));

    auto [full_pv, clean_pv] = cds_contract2.value(valuation_date2, issuer_curve2, cds_recovery);
    REQUIRE_THAT(full_pv, Catch::Matchers::WithinAbs(-195240.9653, 0.0001));
    REQUIRE_THAT(clean_pv, Catch::Matchers::WithinAbs(-186907.6319, 0.0001));

    auto p = cds_contract2.clean_price(valuation_date2, issuer_curve2, cds_recovery);
    REQUIRE_THAT(p, Catch::Matchers::WithinAbs(118.6907, 0.0001));

    auto accrued_days = cds_contract2.accrued_days();
    REQUIRE_THAT(accrued_days, Catch::Matchers::WithinAbs(60, 0.0001));

    auto accrued_interest = cds_contract2.accrued_interest();
    REQUIRE_THAT(accrued_interest, Catch::Matchers::WithinAbs(-8333.3333, 0.0001));

    auto prot_pv = cds_contract2.protection_leg_pv(valuation_date2, issuer_curve2, cds_recovery);
    REQUIRE_THAT(prot_pv, Catch::Matchers::WithinAbs(46830.4743, 0.0001));

    auto premPV = cds_contract2.premium_leg_pv(valuation_date2, issuer_curve2);
    REQUIRE_THAT(premPV, Catch::Matchers::WithinAbs(242071.4397, 0.0001));

    auto [full_pvv, clean_pvv, credit01, ir01] = cds_contract2.value_fast_approx(valuation_date2,r2,mktSpread2,cds_recovery, 0.4);
    REQUIRE_THAT(full_pvv, Catch::Matchers::WithinAbs(-195853.3675, 0.0001));
    REQUIRE_THAT(clean_pvv, Catch::Matchers::WithinAbs(-187520.0342, 0.0001));
    REQUIRE_THAT(credit01, Catch::Matchers::WithinAbs(534.9973, 0.0001));
    REQUIRE_THAT(ir01, Catch::Matchers::WithinAbs(44.6327, 0.0001));
}
