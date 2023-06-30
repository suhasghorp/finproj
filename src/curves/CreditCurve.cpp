#include <finproj/curves/CreditCurve.h>
#include <finproj/optim/brent.h>
#include <finproj/curves/CDS.h>
#include <ranges>
#include <iostream>

CreditCurve::CreditCurve(const ChronoDate& valuation_date, const std::vector<CDS>& cds_contracts,
            IborSingleCurve& libor_curve,double recovery_rate,
            InterpTypes interp_type):
 valuation_date_{valuation_date},cds_contracts_{cds_contracts},libor_curve_{libor_curve},
recovery_rate_{recovery_rate}, interp_type_{interp_type}
{
  build_curve();
}

void CreditCurve::build_curve() {
  validate();
  auto num_times = cds_contracts_.size();
  times_.push_back(0.0);values_.push_back(1.0);
  for (int i{0}; i < num_times;++i){
    auto maturity_date = cds_contracts_.at(i).get_maturity_date();
    auto tmat = (maturity_date - valuation_date_) / 365.0;
    auto q = values_[i];
    times_.push_back(tmat);
    values_.push_back(q);
    auto _g = [&](const double q) {
      (*this).values_.back() = q;
      double v = std::get<1>(cds_contracts_.at(i).value(valuation_date_, *this, recovery_rate_));
      if (v == 0.0) v = 1e-12;
      return v;
    };
    Iteration *brent = new Brent(1e-7, _g);
    q = brent->solve(1e-3,q);
  }
}

void CreditCurve::validate() const {
  if (cds_contracts_.empty()){
    throw std::runtime_error("No CDS contracts have been supplied");
  }
  auto maturity_date = cds_contracts_.at(0).get_maturity_date();
  for (auto cds : cds_contracts_ | std::views::drop(1)){
    if (cds.get_maturity_date() <= maturity_date){
      throw std::runtime_error("CDS contracts not in increasing maturity");
    }
    maturity_date = cds.get_maturity_date();
  }
}