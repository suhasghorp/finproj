#include "Application.h"
#include <finproj/curves/CDS.h>
#include <finproj/utils/Misc.h>
namespace plt = matplotlibcpp;

IborSwap Application::create_swap(const ChronoDate& val_date, const ChronoDate& maturity_date, double swap_rate){
  auto accrual = DayCountTypes::THIRTY_E_360;
  auto freq = FrequencyTypes::SEMI_ANNUAL;
  auto spot_days = 2;
  auto settlement_date = val_date.add_weekdays(spot_days);
  auto fixed_leg_type = SwapTypes::PAY;
  return IborSwap(settlement_date, maturity_date, fixed_leg_type,swap_rate, freq, accrual);
}

IborDeposit Application::create_deposit(const ChronoDate& val_date, double rate){
  auto spot_days = 0;
  auto settlement_date = val_date.add_weekdays(spot_days);
  auto depoDCCType = DayCountTypes::ACT_360;
  auto maturity_date = settlement_date.add_months(3);
  return IborDeposit(settlement_date, maturity_date, rate, depoDCCType);
}

IborSingleCurve Application::build_swap_curve(const ChronoDate& val_date, std::string&& data_file, InterpTypes interp) {
  InterpTypes interp_type{interp};
  auto fut_count{0};
  std::vector<IborDeposit> depos{};
  std::vector<IborFRA> fras{};
  std::vector<IborSwap> swaps{};
  std::ifstream fcurve;
  fcurve.open(data_file);
  if (fcurve.fail())
    throw std::runtime_error("failed to open swap curve data file");
  std::string line;
  getline(fcurve,line); //skip header
  while(std::getline(fcurve,line)){
    std::stringstream input_string(line);
    std::string instrument, date, rate, convexity;
    SwapCurveNode node{};
    getline(input_string,instrument,',');
    node.instr_ = instrument;
    getline(input_string,date,',');
    node.date_ = ChronoDate(date);
    getline(input_string,rate,',');
    node.rate_ = std::stod(rate);
    if (node.instr_ == "FUT"){
      getline(input_string,convexity,',');
      node.convexity_ = std::stod(convexity);
      fras.emplace_back(IborFuture(val_date, ++fut_count).to_fra(node.rate_,node.convexity_));
    } else if (node.instr_ == "DEPOSIT"){
      depos.push_back(create_deposit(val_date,node.rate_));
    } else if (node.instr_ == "SWAP"){
      swaps.push_back(create_swap(val_date,node.date_,node.rate_));
    }
  }
  auto libor_curve = IborSingleCurve(val_date, depos, fras, swaps, interp_type);
  return libor_curve;
}

std::vector<CreditCurve> Application::build_credit_curves(const ChronoDate& val_date, const std::string&& spreads_file,
                                             const IborSingleCurve& libor_curve, double recovery_rate)
{
  std::vector<CDS> cds_contracts{};
  std::vector<CreditCurve> credit_curves{};
  std::ifstream fcurve;
  fcurve.open(spreads_file);
  if (fcurve.fail())
    throw std::runtime_error("failed to open cds spreads data file");
  std::string line;
  getline(fcurve,line); //skip header
  while(std::getline(fcurve,line)){
    cds_contracts.clear();
    std::stringstream input_string(line);
    std::string temp;
    getline(input_string,temp,',');
    auto ticker = temp;
    getline(input_string,temp,',');
    cds_contracts.emplace_back(CDS(val_date, "6M", std::stod(temp)));
    getline(input_string,temp,',');
    cds_contracts.emplace_back(CDS(val_date, "1Y", std::stod(temp)));
    getline(input_string,temp,',');
    cds_contracts.emplace_back(CDS(val_date, "2Y", std::stod(temp)));
    getline(input_string,temp,',');
    cds_contracts.emplace_back(CDS(val_date, "3Y", std::stod(temp)));
    getline(input_string,temp,',');
    cds_contracts.emplace_back(CDS(val_date, "4Y", std::stod(temp)));
    getline(input_string,temp,',');
    cds_contracts.emplace_back(CDS(val_date, "5Y", std::stod(temp)));
    getline(input_string,temp,',');
    cds_contracts.emplace_back(CDS(val_date, "7Y", std::stod(temp)));
    getline(input_string,temp,',');
    cds_contracts.emplace_back(CDS(val_date, "10Y", std::stod(temp)));
    getline(input_string,temp,',');
    cds_contracts.emplace_back(CDS(val_date, "15Y", std::stod(temp)));
    getline(input_string,temp,',');
    cds_contracts.emplace_back(CDS(val_date, "20Y", std::stod(temp)));
    getline(input_string,temp,',');
    cds_contracts.emplace_back(CDS(val_date, "30Y", std::stod(temp)));

    credit_curves.emplace_back(CreditCurve(val_date,ticker, cds_contracts,libor_curve,recovery_rate));
  }
  return credit_curves;
}

void Application::plot_discount_curve(const IborSingleCurve& curve, const std::string& filename) const {
  plt::figure();
  plt::plot(curve.times_, curve.dfs_);
  plt::title("Interpolated Discount Curve");
  plt::xlabel("Years");
  plt::ylabel("Discount Factor");
  plt::save(filename);
}

void Application::plot_zero_curve(const IborSingleCurve& curve, const std::string& filename) const {
  plt::figure();
  plt::plot(curve.times_, curve.df_to_zero(curve.dfs_,curve.times_,FrequencyTypes::CONTINUOUS));
  plt::title("Interpolated Zero Curve");
  plt::xlabel("Years");
  plt::ylabel("Zero Rate");
  plt::save(filename);
}
void Application::plot_surv_prob_curves(const ChronoDate& val_date,const std::vector<CreditCurve>& ccurves, const std::string& filename) const {
  plt::figure();
  for (auto& curve : ccurves){
    auto years = linspace(0.0,10.0,40);
    std::vector<double> surv_probs{};
    std::vector<double> hazard_rates{};
    hazard_rates.push_back(0.0);
    for (auto y{0}; y < years.size(); ++y){
      auto date = val_date.add_years(years[y]);
      auto q = curve.surv_prob(date);
      surv_probs.push_back(q);
      if (y > 0){
        auto h = -log(surv_probs[y]/surv_probs[y-1])/(years[y] - years[y-1]);
        hazard_rates.push_back(h);
      }
    }
    plt::plot(years, surv_probs, {{"label", curve.ticker_}});
  }
  plt::title("Survival Probability Curves");
  plt::xlabel("Years");
  plt::ylabel("Survival Probability (q)");
  plt::legend();
  plt::save(filename);
}

void Application::plot_hazard_curves(const ChronoDate& val_date,const std::vector<CreditCurve>& ccurves, const std::string& filename) const {
  plt::figure();
  for (auto& curve : ccurves){
    auto years = linspace(0.0,10.0,40);
    std::vector<double> surv_probs{};
    std::vector<double> hazard_rates{};
    hazard_rates.push_back(0.0);
    for (auto y{0}; y < years.size(); ++y){
      auto date = val_date.add_years(years[y]);
      auto q = curve.surv_prob(date);
      surv_probs.push_back(q);
      if (y > 0){
        auto h = -log(surv_probs[y]/surv_probs[y-1])/(years[y] - years[y-1]);
        hazard_rates.push_back(h);
      }
    }
    plt::plot(years, hazard_rates, {{"label", curve.ticker_}});
  }
  plt::title("Term Structure Hazard Rates - Quarterly");
  plt::xlabel("Years");
  plt::ylabel("Hazard Rate (h)");
  plt::legend();
  plt::save(filename);
}