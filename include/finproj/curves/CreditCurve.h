#ifndef FINPROJ_INCLUDE_FINPROJ_CURVES_CREDITCURVE_H_
#define FINPROJ_INCLUDE_FINPROJ_CURVES_CREDITCURVE_H_
#include <vector>
#include <string>
#include <finproj/utils/ChronoDate.h>
#include <finproj/curves/IborSingleCurve.h>
#include <finproj/utils/Interpolator.h>

class CDS; //forward declare CDS to avoid circular dependencies, cds.h is included in CreditCurve.cpp

class CreditCurve{
 public:
  CreditCurve(const ChronoDate& valuation_date, const std::string& ticker, const std::vector<CDS>& cds_contracts,
        const IborSingleCurve& libor_curve,double recovery_rate,
        InterpTypes interp_type = InterpTypes::FLAT_FWD_RATES);
  void build_curve();
  CreditCurve get_bumped_spread_curve(double bump) const;
  CreditCurve get_bumped_rec_rate_curve(double bump) const;
  void validate() const;
  double get_rec_rate() const;
  void set_rec_rate(double rate);
  double surv_prob(const ChronoDate& dt) const;
  std::vector<double> times_{}, values_{};
  IborSingleCurve libor_curve_{};
  double recovery_rate_{};
  std::string ticker_{};
 private:
  ChronoDate valuation_date_{};
  std::vector<CDS> cds_contracts_{};
  InterpTypes interp_type_{};



};

#endif//FINPROJ_INCLUDE_FINPROJ_CURVES_CREDITCURVE_H_
