#include <finproj/curves/IborDeposit.h>
#include <finproj/curves/IborFRA.h>
#include <finproj/curves/IborSwap.h>
#include <finproj/curves/IborFuture.h>
#include <finproj/curves/IborSingleCurve.h>
#include <finproj/matplot/matplotlibcpp.h>
#include <finproj/utils/ChronoDate.h>
#include <finproj/utils/DayCount.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>
#include "Application.h"
#include <finproj/curves/CDS.h>
#include <finproj/utils/Misc.h>
//#include <matplot/matplot.h>

namespace plt = matplotlibcpp;
//using namespace matplot;
constexpr double RECOVERY_RATE = 0.4;

int main() {
  ChronoDate val_date{2023,6,29};

  Application application{};
  IborSingleCurve libor_curve = application.create_swap_curve(val_date,std::move("../swap_curve.csv"),
                                                             InterpTypes::FLAT_FWD_RATES);

  /*std::vector<double> x = matplot::linspace(0, 2 * matplot::pi);
  std::vector<double> y = matplot::transform(x, [](auto x) { return sin(x); });
  matplot::plot(x, y, "-o");
  matplot::hold(matplot::on);
  matplot::plot(x, matplot::transform(y, [](auto y) { return -y; }), "--xr");
  matplot::plot(x, matplot::transform(x, [](auto x) { return x / matplot::pi - 1.; }), "-:gs");
  matplot::plot({1.0, 0.7, 0.4, 0.0, -0.4, -0.7, -1}, "k");
  matplot::save("./test.png", "png");*/

  plt::figure();
  plt::plot(libor_curve.times_, libor_curve.dfs_);
  plt::title("Interpolated Discount Curve");
  plt::xlabel("Years");
  plt::ylabel("Discount Factor");
  plt::save("./discount_curve.png");

  plt::figure();
  plt::plot(libor_curve.times_, libor_curve.df_to_zero(libor_curve.dfs_,libor_curve.times_,FrequencyTypes::CONTINUOUS));
  plt::title("Interpolated Zero Curve");
  plt::xlabel("Years");
  plt::ylabel("Zero Rate");
  plt::save("./zero_curve.png");

  std::vector<CreditCurve> credit_curves = application.create_credit_curves(val_date, std::move("../current_spreads.csv"),
                                                                            libor_curve, RECOVERY_RATE);

  plt::figure();
  for (auto& curve : credit_curves){
    auto years = linspace(0.0,10.0,40);
    std::vector<double> surv_probs{};
    std::vector<double> hazard_rates{};
    hazard_rates.push_back(0.0);
    for (auto y{0}; y < years.size(); ++y){
      auto date = val_date.add_years(years[y]);
      auto df = libor_curve.df(date);
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

  plt::save("./surv_probs.png");


  return 0;
}
