#include <finproj/models/GaussCopula.h>
#include <finproj/curves/CDS.h>
#include <random>

MatrixXd GaussCopula::default_times_gc(const std::vector<CreditCurve>& issuer_curves,
                        const MatrixXd& correlationMatrix,
                        const int num_trials,
                        int seed)
{
  int num_credits = static_cast<int>(issuer_curves.size());

  std::mt19937_64 mtEngine(seed);
  std::normal_distribution<> nd;
  MatrixXd x = MatrixXd::Zero(num_credits,num_trials).unaryExpr([&](double dummy){return nd(mtEngine);});
  auto m = x.mean();
  MatrixXd L( correlationMatrix.llt().matrixL() );
  auto y = (L * x).eval();
  MatrixXd corr_times = MatrixXd::Zero(num_credits, 2 * num_trials);
  for (auto i{0};i<num_credits;++i){

    for (auto t{0}; t < num_trials;++t){
      auto g = y(i, t);
      auto u1 = 1.0 - N(g);
      auto u2 = 1.0 - u1;
      auto times = issuer_curves.at(i).times_;
      auto values = issuer_curves.at(i).values_;
      auto t1 = uniform_to_default_time(u1, times, values);
      auto t2 = uniform_to_default_time(u2, times, values);
      corr_times(i,t) = t1;
      corr_times(i, num_trials + t) = t2;
    }
  }
  return corr_times;
}