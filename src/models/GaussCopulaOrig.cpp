#include <finproj/models/GaussCopula.h>
#include <finproj/curves/CDS.h>
#include <finproj/utils/Misc.h>
#include <random>
#include <finproj/sobol/sobol.h>
#include <ql/math/randomnumbers/sobolrsg.hpp>
#include <ql/math/distributions/normaldistribution.hpp>

using namespace QuantLib;

MatrixXd GaussCopula::i4_sobol_generate_std_normal(int dim_num, int n){
  MatrixXd x = MatrixXd::Zero(n, dim_num);
    auto sobols = i4_sobol_generate(dim_num, n, 1);
    /*for (auto s : sobols){
      auto normal = NormalCDFInverse(s);
    }*/
    return x;

}

MatrixXd GaussCopula::default_times_gc(const std::vector<CreditCurve>& issuer_curves,
                        const MatrixXd& correlationMatrix,
                        const int num_trials,
                        int seed)
{
  int num_credits = static_cast<int>(issuer_curves.size());

  std::mt19937_64 mtEngine(seed);
  std::normal_distribution<> nd;

  MoroInverseCumulativeNormal invGauss;
  MatrixXd x = MatrixXd::Zero(num_credits,num_trials);

  //auto rand = i4_sobol_generate_std_normal(num_trials, num_credits);
  double current_sobol_num{}, current_normal_number{};
  SobolRsg sobolEngine(num_trials);
  for (int c{0}; c< num_credits;++c){
    for (int t{0}; t < num_trials;++t){
        current_sobol_num = (sobolEngine.nextSequence().value)[0];
        current_normal_number = invGauss(current_sobol_num);
        x(c,t) = current_normal_number;
    };
  }

  //MatrixXd x = MatrixXd::Zero(num_credits,num_trials).unaryExpr([&](double dummy){return nd(mtEngine);});

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