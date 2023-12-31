#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <finproj/curves/CDS.h>
#include <finproj/curves/IborDeposit.h>
#include <finproj/curves/IborFuture.h>
#include <finproj/curves/IborSingleCurve.h>
#include <finproj/curves/IborSwap.h>
#include <finproj/utils/Misc.h>
#include <finproj/curves/CDSIndexPortfolio.h>
#include <finproj/curves/CDSBasket.h>
#include <tuple>
#include <iostream>
#include <fstream>
#include <string>
#include <array>
#include <finproj/tabulate/tabulate.h>
using namespace tabulate;



TEST_CASE( "test_cds_basket", "[single-file]" ){
  ChronoDate valuation_date{2007,8,1};
  auto settlement_date = valuation_date.add_weekdays(1);

  auto dcType = DayCountTypes::THIRTY_E_360_ISDA;
  auto fixedFreq = FrequencyTypes::SEMI_ANNUAL;
  auto swapType = SwapTypes::PAY;

  std::vector<IborDeposit> depos{};
  depos.emplace_back(IborDeposit(settlement_date, "1D", 0.0502, dcType));

  std::vector<IborSwap> swaps{};
  std::vector<IborFRA> fras{};
  swaps.emplace_back(IborSwap(settlement_date,"1Y",swapType,0.0502,fixedFreq,dcType));
  swaps.emplace_back(IborSwap(settlement_date,"2Y",swapType,0.0502,fixedFreq,dcType));
  swaps.emplace_back(IborSwap(settlement_date,"3Y",swapType,0.0501,fixedFreq,dcType));
  swaps.emplace_back(IborSwap(settlement_date,"4Y",swapType,0.0502,fixedFreq,dcType));
  swaps.emplace_back(IborSwap(settlement_date,"5Y",swapType,0.0501,fixedFreq,dcType));

  auto libor_curve = IborSingleCurve(valuation_date, depos, fras, swaps);

  auto step_in_date = valuation_date.add_weekdays(0);
  valuation_date = step_in_date;
  auto maturity3Y = valuation_date.next_cds_date(36);
  auto maturity5Y = valuation_date.next_cds_date(60);
  auto maturity7Y = valuation_date.next_cds_date(84);
  auto maturity10Y = valuation_date.next_cds_date(120);

  std::vector<CreditCurve> issuer_curves{};
  std::string file = "../CDX_NA_IG_S7_SPREADS.csv";
  std::ifstream fspreads;
  fspreads.open(file);
  if (fspreads.fail())
    throw std::runtime_error("failed to open CDX_NA_IG_S7_SPREADS.csv");
  std::string line;
  getline(fspreads,line); //skip header
  int num_credits = 0;
  while(std::getline(fspreads,line)){
    if (num_credits == 5)
      break;
    std::stringstream inputstring(line);
    std::string ticker, temp;
    double spd3Y{},spd5Y{},spd7Y{},spd10Y{},rec{};
    getline(inputstring,ticker,',');
    getline(inputstring,temp,',');
    spd3Y = std::stod(temp.c_str())/10000.0;
    getline(inputstring,temp,',');
    spd5Y = std::stod(temp.c_str())/10000.0;
    getline(inputstring,temp,',');
    spd7Y = std::stod(temp.c_str())/10000.0;
    getline(inputstring,temp,',');
    spd10Y = std::stod(temp.c_str())/10000.0;
    getline(inputstring,temp,',');
    rec = std::stod(temp.c_str());

    std::vector<CDS> contracts{};
    contracts.emplace_back(CDS(step_in_date, maturity3Y, spd3Y));
    contracts.emplace_back(CDS(step_in_date, maturity5Y, spd5Y));
    contracts.emplace_back(CDS(step_in_date, maturity7Y, spd7Y));
    contracts.emplace_back(CDS(step_in_date, maturity10Y, spd10Y));

    issuer_curves.emplace_back(CreditCurve(valuation_date, ticker, contracts, libor_curve, rec));

    ++num_credits;
    line = "";
  }


  auto recovery_rate = 0.40;
  ChronoDate basketMaturity {2011,12,20};
  auto cdsIndex = CDSIndexPortfolio();
  auto intrinsicSpd = cdsIndex.intrinsic_spread(valuation_date, step_in_date, basketMaturity, issuer_curves) * 10000.0;
  auto basket = CDSBasket(valuation_date,basketMaturity);
  int seed = 42;
  int doF = 5;
  Table gauss_copula_output, student_copula_output;
  std::vector<int> num_trials {1000,5000,10000,20000,30000,40000,50000,60000,70000,80000,90000,100000};//, 500000, 1000000};
  //std::array<double,6> betas{0.0, 0.25, 0.5, 0.75, 0.90, 0.9999};
  std::array<double,1> betas{0.25};
  for (int ntd{1}; ntd < num_credits+1; ++ntd) {
    for (auto beta: betas) {
      auto rho = beta * beta;
      auto corr_matrix = corr_matrix_generator(rho, num_credits);
      for (int sims: num_trials) {
        auto [valuep, rpv01p, spdp] = basket.value_gaussian_mc(valuation_date, ntd, issuer_curves, corr_matrix,
                                                               libor_curve, sims, seed, R"(PSEUDO)");
        auto [valueq, rpv01q, spdq] = basket.value_gaussian_mc(valuation_date, ntd, issuer_curves, corr_matrix,
                                                               libor_curve, sims, seed, R"(QUASI)");
        gauss_copula_output.add_row({std::to_string(ntd), "Gaussian", "Pseudo", std::to_string(sims), std::to_string(spdp * 10000)});
        gauss_copula_output.add_row({std::to_string(ntd), "Gaussian", "Quasi", std::to_string(sims), std::to_string(spdq * 10000)});
      }
    }
  }
  std::cout << gauss_copula_output << std::endl;
  for (int ntd{1}; ntd < num_credits+1; ++ntd) {
    for (auto beta: betas) {
      auto rho = beta * beta;
      auto corr_matrix = corr_matrix_generator(rho, num_credits);
      for (int sims: num_trials) {
        auto [valuep, rpv01p, spdp] = basket.value_student_t_mc(valuation_date, ntd, issuer_curves, corr_matrix,
                                                                doF, libor_curve, sims, seed, R"(PSEUDO)");
        auto [valueq, rpv01q, spdq] = basket.value_student_t_mc(valuation_date, ntd, issuer_curves, corr_matrix,
                                                                doF, libor_curve, sims, seed, R"(QUASI)");
        student_copula_output.add_row({std::to_string(ntd), "Student-t", "Pseudo", std::to_string(sims), std::to_string(spdp * 10000)});
        student_copula_output.add_row({std::to_string(ntd), "Student-t", "Quasi", std::to_string(sims), std::to_string(spdq * 10000)});
      }
    }
  }
  std::cout << student_copula_output << std::endl;

  REQUIRE(1 == 1);


}
