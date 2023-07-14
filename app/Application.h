#ifndef FINPROJ_APP_APPLICATION_H_
#define FINPROJ_APP_APPLICATION_H_

#include <finproj/curves/IborDeposit.h>
#include <finproj/curves/IborFRA.h>
#include <finproj/curves/IborSwap.h>
#include <finproj/curves/IborFuture.h>
#include <finproj/curves/IborSingleCurve.h>
#include <finproj/matplot/matplotlibcpp.h>
#include <finproj/utils/ChronoDate.h>
#include <finproj/curves/CreditCurve.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

struct SwapCurveNode {
  std::string instr_{};
  ChronoDate date_{};
  double rate_{};
  double convexity_{};
};

/*struct CreditCurveNode {
  std::string ticker_{};
  double spread_6m_{};
  double spread_1y_{};
  double spread_2Y_{};
  double spread_3Y_{};
  double spread_4Y_{};
  double spread_5Y_{};
  double spread_7Y_{};
  double spread_10Y_{};
  double spread_15Y_{};
  double spread_20Y_{};
  double spread_30Y_{};
};*/

class Application {
 public:
  Application() = default;
  IborDeposit create_deposit(const ChronoDate& val_date, double rate);
  IborSwap create_swap(const ChronoDate& val_date, const ChronoDate& maturity_date, double swap_rate);
  IborSingleCurve build_swap_curve(const ChronoDate& val_date, std::string&& data_file, InterpTypes interp);
  std::vector<CreditCurve> build_credit_curves(const ChronoDate& val_date, const std::string&& spreads_file, const IborSingleCurve& libor_curve, double recovery_rate);
  void plot_discount_curve(const IborSingleCurve& curve, const std::string& filename) const;
  void plot_zero_curve(const IborSingleCurve& curve, const std::string& filename) const;
  void plot_surv_prob_curves(const ChronoDate& val_date,const std::vector<CreditCurve>& ccurves, const std::string& filename) const;
  void plot_hazard_curves(const ChronoDate& val_date,const std::vector<CreditCurve>& ccurves, const std::string& filename) const;
  void plot_kth_to_default_spreads(const std::string& dist_type, const std::vector<double>& ntd, const std::vector<double>& spreads_pseudo,
                                         const std::vector<double>& spreads_quasi, const std::string& filename) const;
  void plot_spread_convergence(const std::string& dist_type, const std::vector<double>& sims, const std::vector<double>& spreads_converge_pseudo,
                                   const std::vector<double>& spreads_converge_quasi, const std::string& filename) const;
  void plot_correlation_sensitivity(const std::vector<double>& shocks, const std::vector<std::vector<double>> shocked_data, const std::string& filename) const;
  void plot_rec_rate_sensitivity(const std::vector<double>& rec_shocks, const std::vector<std::vector<double>> shocked_data, const std::string& filename) const;
  void plot_spread_sensitivity(const std::vector<double>& bumps, const std::vector<std::vector<double>> shocked_data, const std::string& filename) const;

};

#endif//FINPROJ_APP_APPLICATION_H_
