#include <finproj/curves/IborDeposit.h>
#include <finproj/curves/IborSingleCurve.h>
#include <finproj/utils/ChronoDate.h>
#include <vector>
#include "Application.h"
#include <finproj/curves/CDS.h>

constexpr double RECOVERY_RATE = 0.4;

int main() {
  auto python_home = getenv("PYTHONHOME");
  if (python_home == NULL) {
    std::cout << "PYTHONHOME env variable is not set, plots will not be saved to a file." << std::endl;
    std::cout << "If you would like to save plot files, set PYTHONHOME to something similar to " <<std::endl;
    std::cout << "export PYTHONHOME=/home/suhasghorp/anaconda3" <<std::endl;
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



  return 0;
}
