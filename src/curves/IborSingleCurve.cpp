#include <finproj/optim/secant.h>
#include <finproj/curves/IborSingleCurve.h>
#include <ranges>
#include <cmath>

IborSingleCurve::IborSingleCurve ( const ChronoDate& val_date, const std::vector<IborDeposit>& ibor_deposits,
                const std::vector<IborFRA>& ibor_fras, const std::vector<IborSwap>& ibor_swaps,
                InterpTypes interp_type,
                bool check_refit):
               DiscountCurve(val_date,FrequencyTypes::ANNUAL,DayCountTypes::ACT_360,interp_type),
                                                     ibor_deposits_{ibor_deposits},
                                                     ibor_fras_{ibor_fras},
                                                     ibor_swaps_{ibor_swaps}, check_refit_{check_refit}
{
  validate_inputs();
  build_curve();
}
void IborSingleCurve::build_curve(){
  build_curve_using_1d_solver();
}
void IborSingleCurve::validate_inputs(){

  //deposits checks
  if (ibor_deposits_.empty() && ibor_fras_.empty() && ibor_swaps_.empty()){
    throw std::runtime_error("No calibration instruments provided");
  }
  auto deposit_start_date = valuation_date_;
  auto swap_start_date = valuation_date_;
  if (ibor_deposits_.size() > 0){
    deposit_start_date = ibor_deposits_.at(0).get_start_date();
    if (deposit_start_date < valuation_date_){
      throw std::runtime_error("First deposit starts before valuation date");
    }
  }
  for (auto& dep : ibor_deposits_){
    if (dep.get_start_date() >= dep.get_maturity_date()){
      throw std::runtime_error("Deposit ends on or before it begins");
    }
  }
  if (ibor_deposits_.size() > 0){
    auto prev_dt = ibor_deposits_.at(0).get_maturity_date();
    for (auto dep : ibor_deposits_ | std::views::drop(1)){
      auto next_dt = dep.get_maturity_date();
      if (next_dt <= prev_dt){
        throw std::runtime_error("Deposits must be in increasing maturity");
      }
      prev_dt = next_dt;
    }
  }

  // FRA checks
  for (auto& fra : ibor_fras_){
    if (fra.get_start_date() < valuation_date_){
      throw std::runtime_error("FRA starts before valuation date");
    }
  }
  if (ibor_fras_.size() > 1){
    auto prev_dt = ibor_fras_.at(0).get_maturity_date();
    for (auto fra : ibor_fras_ | std::views::drop(1)){
      auto next_dt = fra.get_maturity_date();
      if (next_dt <= prev_dt){
        throw std::runtime_error("FRAs must be in increasing maturity");
      }
    }
  }

  //SWAP checks
  if (ibor_swaps_.size() > 1){
    auto start_dt = ibor_swaps_.at(0).get_eff_date();
    for (auto swap : ibor_swaps_ | std::views::drop(1)){
      auto next_dt = swap.get_eff_date();
      if (next_dt != start_dt){
        throw std::runtime_error("Swaps must all have same start date.");
      }
    }
    auto prev_dt = ibor_swaps_.at(0).get_maturity_date();
    for (auto swap : ibor_swaps_ | std::views::drop(1)){
      auto next_dt = swap.get_maturity_date();
      if (next_dt <= prev_dt){
        throw std::runtime_error("Swaps must be in increasing maturity");
      }
      prev_dt = next_dt;
    }
  }
  auto longest_swap = ibor_swaps_.back();
  auto longest_swap_cpn_dates = longest_swap.get_fixed_leg().get_payment_dates();
  for (auto swap : ibor_swaps_ | std::views::take(ibor_swaps_.size() - 1)){
    auto swap_cpn_dates = swap.get_fixed_leg().get_payment_dates();
    for (int i{0}; i < swap_cpn_dates.size();++i){
      if (swap_cpn_dates[i] != longest_swap_cpn_dates[i]){
        throw std::runtime_error("Swap coupons are not on the same date grid.");
      }
    }
  }

  ChronoDate last_dep_maturity_date{}, first_fra_maturity_date{},last_fra_maturity_date{},first_swap_maturity_date{};
  if (ibor_deposits_.size() > 0){
    last_dep_maturity_date = ibor_deposits_.back().get_maturity_date();
  }
  if (ibor_fras_.size() > 0){
    first_fra_maturity_date = ibor_fras_.at(0).get_maturity_date();
    last_fra_maturity_date = ibor_fras_.back().get_maturity_date();
  }
  if(ibor_swaps_.size() > 0){
    first_swap_maturity_date = ibor_swaps_.at(0).get_maturity_date();
  }
  if (ibor_deposits_.size() > 0 && ibor_fras_.size() > 0){
    if (first_fra_maturity_date <= last_dep_maturity_date)
      throw std::runtime_error("First FRA must end after last Deposit");
  }
  if (ibor_fras_.size() > 0 && ibor_swaps_.size() > 0){
    if (first_swap_maturity_date <= last_fra_maturity_date)
      throw std::runtime_error("First Swap must mature after last FRA ends");
  }

  /** If both depos and swaps start after T, we need a rate to get them to
  the first deposit. So we create a synthetic deposit rate contract. */
  swap_start_date = ibor_swaps_.at(0).get_eff_date();
  if (swap_start_date > valuation_date_){
    if (ibor_deposits_.size() == 0)
      throw std::runtime_error("Need a deposit rate to pin down short end.");
    deposit_start_date = ibor_deposits_.at(0).get_start_date();
    if (deposit_start_date > valuation_date_){
      auto first_depo = ibor_deposits_.at(0);
      if (first_depo.get_start_date() > valuation_date_){
        auto synthetic_deposit(first_depo);
        synthetic_deposit.set_start_date(valuation_date_);
        synthetic_deposit.set_maturity_date(first_depo.get_start_date());
        ibor_deposits_.insert(ibor_deposits_.begin(), synthetic_deposit);
      }
    }
  }

  if (ibor_swaps_.size() > 0){
    day_count_type_ = ibor_swaps_.at(0).get_double_leg().get_day_count_type();
  } else {
    day_count_type_ = DayCountTypes{};
  }
}



void IborSingleCurve::build_curve_using_1d_solver() {
  double tmat = 0.0, df_mat = 1.0;
  times_.push_back(tmat);
  dfs_.push_back(df_mat);
  interpolator_.fit(times_,dfs_);

  for (auto &dep: ibor_deposits_) {
    auto df_settle = df(dep.get_start_date());
    auto df_mat = dep.maturity_df() * df_settle;
    tmat = double(dep.get_maturity_date() - valuation_date_) / 365.0;
    times_.push_back(tmat);
    dfs_.push_back(df_mat);
    interpolator_.fit(times_,dfs_);
  }

  auto old_tmat = tmat;

  for (auto &fra: ibor_fras_) {
    auto tset = double(fra.get_start_date() - valuation_date_) / 365.0;
    tmat = double(fra.get_maturity_date() - valuation_date_) / 365.0;
    if (tset < old_tmat && tmat > old_tmat) {
      df_mat = fra.maturity_df(*this);
      times_.push_back(tmat);
      dfs_.push_back(df_mat);
    } else {
      times_.push_back(tmat);
      dfs_.push_back(df_mat);
      auto _g = [&](const double df) {
        (*this).dfs_.back() = df;
        (*this).interpolator_.fit(times_, dfs_);
        auto v_fra = fra.value(valuation_date_, *this, *this);
        v_fra /= fra.get_notional();
        return v_fra;
      };
      Iteration *secant1 = new Secant(1e-10, _g);
      df_mat = secant1->solve(1e-3, 2);

    }
  }

  for (auto &swap: ibor_swaps_) {
    auto maturity_date = swap.get_fixed_leg().generate_payment_dates().back();
    tmat = double(maturity_date - valuation_date_) / 365.0;
    times_.push_back(tmat);
    dfs_.push_back(df_mat);

    auto _f = [&](const double df) {
      (*this).dfs_.back() = df;
      (*this).interpolator_.fit(times_, dfs_);
      std::optional<DiscountCurve> idx_optional = std::nullopt;
      std::optional<double> ffr_optional = std::nullopt;
      auto v_swap = swap.value(valuation_date_, *this, idx_optional, ffr_optional);
      v_swap /= swap.get_fixed_leg().get_notional();
      return v_swap;
    };
    Iteration *secant1 = new Secant(1e-10, _f);
    df_mat = secant1->solve(1e-3, 2);
  }
  if (check_refit_)
    check_refits(1e-10,1e-10,1e-5);
}

void IborSingleCurve::check_refits(double depo_tol, double fra_tol, double swap_tol){
  for (auto& dep : ibor_deposits_){
    auto v = dep.value(valuation_date_, (*this))/dep.get_notional();
    if (fabs(v - 1.0) > depo_tol)
      throw std::runtime_error("Deposit not repriced");
  }
  for (auto& fra : ibor_fras_){
    auto v = fra.value(valuation_date_,(*this), (*this));
    if (fabs(v) > fra_tol){
      throw std::runtime_error("FRA not repriced");
    }
  }
  for (auto& swap : ibor_swaps_){
    auto v = swap.value(valuation_date_,(*this),(*this), std::optional<double>{});
    v = v / swap.get_fixed_leg().get_notional();
    if (fabs(v) > swap_tol){
      throw std::runtime_error("Swap not repriced");
    }
  }
}

