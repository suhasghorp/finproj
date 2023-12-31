#include <finproj/curves/IborDeposit.h>
#include <finproj/curves/IborSingleCurve.h>
#include <finproj/utils/ChronoDate.h>
#include <vector>
#include "Application.h"
#include <finproj/curves/CDS.h>
#include <finproj/curves/CDSBasket.h>
#include <finproj/tabulate/tabulate.h>
#include <finproj/utils/Misc.h>
using namespace tabulate;
constexpr double RECOVERY_RATE = 0.4;
constexpr unsigned int NUM_CREDITS = 5;


int main() {
  auto python_home = getenv("PYTHONHOME");
  if (python_home == NULL) {
    std::cout << "PYTHONHOME env variable is not set, plots will not be saved to a file." << std::endl;
    std::cout << "If you would like to save plot files, set PYTHONHOME to something similar to " <<std::endl;
    std::cout << "export PYTHONHOME=/home/sghorp/miniconda3" <<std::endl;
  }


  ChronoDate val_date{2023,6,29};

  Application application{};
  IborSingleCurve libor_curve = application.build_swap_curve(val_date,R"(../swap_curve.csv)",
                                                             InterpTypes::FLAT_FWD_RATES);

  if (python_home != NULL) {
    application.plot_discount_curve(libor_curve, R"(discount_curve)");
    application.plot_zero_curve(libor_curve, R"(zero_curve)");
  }
  std::vector<CreditCurve> credit_curves = application.build_credit_curves(val_date,
                                                                            R"(../current_spreads.csv)",
                                                                            libor_curve, RECOVERY_RATE);
  if (python_home != NULL) {
    application.plot_surv_prob_curves(val_date, credit_curves, R"(surv_probs.png)");
    application.plot_hazard_curves(val_date, credit_curves, R"(hazard_rates.png)");
  }

  auto seed = 42;
  Table gauss_copula_output, student_copula_output;
  ChronoDate basketMaturity {2025,12,17}; //third wednesday of december
  auto basket = CDSBasket(val_date,basketMaturity);
  std::vector<double> num_trials {1000,5000,10000,20000,30000,40000,50000,60000};//,70000,80000,90000,100000};
  std::vector<double> kth_to_default{1,2,3,4,5};
  std::vector<double> spreads_pseudo(5), spreads_quasi(5);
  std::vector<double> spreads_converge_pseudo{},spreads_converge_quasi{};

  // Gaussian
  auto gauss_corr_matrix = load_csv<MatrixXd>(R"(../gaussian_corr.csv)");
  for (size_t ntd{1}; ntd < NUM_CREDITS +1; ++ntd) {
    for (const auto& sims: num_trials) {
      auto [valuep, rpv01p, spdp] = basket.value_gaussian_mc(val_date, (int)ntd, credit_curves, gauss_corr_matrix,
                                                             libor_curve, (int)sims, seed, R"(PSEUDO)");

      auto [valueq, rpv01q, spdq] = basket.value_gaussian_mc(val_date, (int)ntd, credit_curves, gauss_corr_matrix,
                                                             libor_curve, (int)sims, seed, R"(QUASI)");

      spreads_pseudo.at(ntd - 1) = std::trunc(spdp * 10000);spreads_quasi.at(ntd - 1) = std::trunc(spdq * 10000);
      if (ntd == 1) {
        spreads_converge_pseudo.push_back(std::trunc(spdp * 10000));
        spreads_converge_quasi.push_back(std::trunc(spdq * 10000));
      }
      gauss_copula_output.add_row({std::to_string(ntd), "Gaussian", "Pseudo", std::to_string(int(sims)), std::to_string(spdp * 10000)});
      gauss_copula_output.add_row({std::to_string(ntd), "Gaussian", "Quasi", std::to_string(int(sims)), std::to_string(spdq * 10000)});
    }
  }
  application.plot_kth_to_default_spreads(R"(Gaussian Distribution)", kth_to_default,spreads_pseudo,spreads_quasi, R"(gauss_basket_spreads.png)");
  application.plot_spread_convergence(R"(Gaussian Distribution)", num_trials, spreads_converge_pseudo,
                          spreads_converge_quasi, R"(gauss_spread_convergence.png)");
  std::cout << gauss_copula_output << std::endl;
  //Student-t
  spreads_converge_pseudo.clear();spreads_converge_quasi.clear();
  auto student_corr_matrix = load_csv<MatrixXd>(R"(../student_corr.csv)");
  auto dof = 2.9645286f; // from student_df.txt file
  for (size_t ntd{1}; ntd < NUM_CREDITS +1; ++ntd) {
    for (int sims: num_trials) {
      auto [valuep, rpv01p, spdp] = basket.value_student_t_mc(val_date, (int)ntd, credit_curves, student_corr_matrix,
                                                              dof, libor_curve, sims, seed, R"(PSEUDO)");
      auto [valueq, rpv01q, spdq] = basket.value_student_t_mc(val_date, (int)ntd, credit_curves, student_corr_matrix,
                                                              dof, libor_curve, sims, seed, R"(QUASI)");
      spreads_pseudo.at(ntd - 1) = std::trunc(spdp *10000) ;spreads_quasi.at(ntd - 1) = std::trunc(spdq *10000);
      if (ntd == 1) {
        spreads_converge_pseudo.push_back(std::trunc(spdp * 10000));
        spreads_converge_quasi.push_back(std::trunc(spdq * 10000));
      }
      student_copula_output.add_row({std::to_string(ntd), "Student-t", "Pseudo", std::to_string(int(sims)), std::to_string(spdp * 10000)});
      student_copula_output.add_row({std::to_string(ntd), "Student-t", "Quasi", std::to_string(int(sims)), std::to_string(spdq * 10000)});
    }
  }
  application.plot_kth_to_default_spreads(R"(Student-t Distribution)", kth_to_default,spreads_pseudo,spreads_quasi, R"(student_basket_spreads.png)");
  application.plot_spread_convergence(R"(Student-t Distribution)", num_trials, spreads_converge_pseudo,
                                      spreads_converge_quasi, R"(student_spread_convergence.png)");
  std::cout << student_copula_output << std::endl;

  //correlation sensitivity

  std::vector<std::vector<double>> shocked_data{};
  std::vector<double> spreads{};
  std::vector<double> shocks{-99,-90,-80,-70,-60,-50,-40,-30,-20,-10,0.0,10,20,30,40,50,60,70,80,90,99};
  for (size_t ntd{1}; ntd < NUM_CREDITS +1; ++ntd) {
    for (const auto& shock : shocks) {
      auto shocked_gauss_corr_matrix(gauss_corr_matrix);
      shocked_gauss_corr_matrix = shocked_gauss_corr_matrix.unaryExpr([&](double dummy) { return dummy + ((1 - dummy) * (shock/100.0)); });
      auto [valuep, rpv01p, spdp] = basket.value_gaussian_mc(val_date, (int) ntd, credit_curves, shocked_gauss_corr_matrix,
                                                             libor_curve, 40000, seed, R"(PSEUDO)");
      spreads.push_back(std::trunc(spdp * 10000));
    }
    shocked_data.push_back(spreads);
    spreads.clear();
  }
  application.plot_correlation_sensitivity(shocks,shocked_data,R"(correlation_sensitivity.png)");

  //recovery rate sensitivity

  shocked_data.clear();
  spreads.clear();
  std::vector<double> rec_rates{ 0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0 };
  std::vector<CreditCurve> bumped_curves{};

  for (size_t ntd{1}; ntd < NUM_CREDITS +1; ++ntd) {
    for (const auto &r: rec_rates) {
      for (const auto &c: credit_curves) {
        bumped_curves.push_back(c.get_bumped_rec_rate_curve(r));
      }

      auto [valuep, rpv01p, spdp] = basket.value_gaussian_mc(val_date, (int) ntd, bumped_curves, gauss_corr_matrix,
                                                           libor_curve, 50000, seed, R"(PSEUDO)");
      spreads.push_back(std::round(spdp * 10000 * 1000.0) / 1000.0);
      bumped_curves.clear();
    }
    shocked_data.push_back(spreads);
    spreads.clear();
  }
  application.plot_rec_rate_sensitivity(rec_rates,shocked_data,R"(recovery_rates_sensitivity.png)");

  //underlying CDS spread sensitivity

  shocked_data.clear();
  spreads.clear();
  bumped_curves.clear();
  std::vector<double> spread_shocks{ -90, -80, -70, -60, -50, -40, -30, -20, -10, 0, 10, 20, 30, 40, 50, 60, 70, 80, 90 };

  for (size_t ntd{1}; ntd < NUM_CREDITS +1; ++ntd) {
    for (const auto &s: spread_shocks) {
      for (const auto &c: credit_curves) {
        bumped_curves.push_back(c.get_bumped_spread_curve(s));
      }

      auto [valuep, rpv01p, spdp] = basket.value_gaussian_mc(val_date, (int) ntd, bumped_curves, gauss_corr_matrix,
                                                             libor_curve, 50000, seed, R"(PSEUDO)");
      spreads.push_back(std::round(spdp * 10000 * 1000.0) / 1000.0);
      bumped_curves.clear();
    }
    shocked_data.push_back(spreads);
    spreads.clear();
  }
  application.plot_spread_sensitivity(spread_shocks,shocked_data,R"(market_cds_spreads_sensitivity.png)");



  return 0;
}
