#include <finproj/curves/CDSIndexPortfolio.h>
#include <finproj/curves/CDS.h>
#include <tuple>

CDSIndexPortfolio::CDSIndexPortfolio(FrequencyTypes freq_type,
                  DayCountTypes day_count_type,
                  CalendarTypes cal_type,
                  DateGenRuleTypes date_gen_rule_type):
 freq_type_{freq_type},day_count_type_{day_count_type},cal_type_{cal_type},date_gen_rule_type_{date_gen_rule_type}
{

}

double CDSIndexPortfolio::intrinsic_rpv01(const ChronoDate& valuation_date,const ChronoDate& step_in_date,const ChronoDate& maturity_date,
                       std::vector<CreditCurve>& issuer_curves) const
{
  auto num_credits = issuer_curves.size();
  auto cds_contract = CDS(step_in_date,maturity_date,0.0);
  auto intrinsic_rpv01 = 0.0;
  for (auto m{0};m<num_credits;++m){
    auto [full_rpv01, clean_rpv01] = cds_contract.risky_pv01(valuation_date,issuer_curves[m]);
    intrinsic_rpv01 += clean_rpv01;
  }
  intrinsic_rpv01 /= num_credits;
  return intrinsic_rpv01;
}

double CDSIndexPortfolio::intrinsic_protection_leg_pv(const ChronoDate& valuation_date,const ChronoDate& step_in_date,const ChronoDate& maturity_date,
                                   std::vector<CreditCurve>& issuer_curves) const
{
  auto num_credits = issuer_curves.size();
  auto cds_contract = CDS(step_in_date,maturity_date,0.0, 1.0);
  auto intrinsic_prot_pv = 0.0;
  auto standard_rec_rate = 0.4;
  for (auto m{0};m<num_credits;++m){
    auto protection_pv = cds_contract.protection_leg_pv(valuation_date,issuer_curves[m],standard_rec_rate);
    intrinsic_prot_pv += protection_pv;
  }
  intrinsic_prot_pv /= num_credits;
  return intrinsic_prot_pv;
}

double CDSIndexPortfolio::intrinsic_spread(const ChronoDate& valuation_date,const ChronoDate& step_in_date,const ChronoDate& maturity_date,
                        std::vector<CreditCurve>& issuer_curves) const
{
  auto intrinsic_prot_pv = intrinsic_protection_leg_pv(valuation_date,
                                                       step_in_date,
                                                       maturity_date,
                                                       issuer_curves);
  auto intrinsic_rpv = intrinsic_rpv01(valuation_date,
                                              step_in_date,
                                              maturity_date,
                                              issuer_curves);
  auto intrinsic_spread = intrinsic_prot_pv / intrinsic_rpv;
  return intrinsic_spread;
}