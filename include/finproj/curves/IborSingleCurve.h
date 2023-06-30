#ifndef FINPROJ_INCLUDE_FINPROJ_CURVES_IBORSINGLECURVE_H_
#define FINPROJ_INCLUDE_FINPROJ_CURVES_IBORSINGLECURVE_H_
#include <finproj/curves/DiscountCurve.h>
#include <finproj/curves/IborDeposit.h>
#include <finproj/curves/IborFRA.h>
#include <finproj/curves/IborSwap.h>
#include <vector>

class IborSingleCurve : public DiscountCurve {
 public:
  IborSingleCurve() = default;
  IborSingleCurve ( const ChronoDate& val_date, const std::vector<IborDeposit>& ibor_deposits,
                  const std::vector<IborFRA>& ibor_fras, const std::vector<IborSwap>& ibor_swaps,
                  InterpTypes interp_type = InterpTypes::FLAT_FWD_RATES,
                  bool check_refit = false);
  void validate_inputs();
  void build_curve();
  void build_curve_using_1d_solver();
  void check_refits(double depo_tol, double fra_tol, double swap_tol);
 private:
  std::vector<IborDeposit> ibor_deposits_{};
  std::vector<IborFRA> ibor_fras_{};
  std::vector<IborSwap> ibor_swaps_{};
  bool check_refit_{};
};

#endif//FINPROJ_INCLUDE_FINPROJ_CURVES_IBORSINGLECURVE_H_
