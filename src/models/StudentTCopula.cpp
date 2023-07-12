#include <finproj/models/StudentTCopula.h>
#include <finproj/curves/CDS.h>
#include <random>
#include <boost/math/distributions/students_t.hpp>

MatrixXd StudentTCopula::default_times(const std::vector<CreditCurve> &issuer_curves,
                          const MatrixXd& correlationMatrix,
                          float degrees_of_freedom,
                          int num_trials,
                          int seed,
                          const std::string& random_number_generation)
{
  int num_credits = static_cast<int>(issuer_curves.size());
  MatrixXd x;
  if (random_number_generation == "PSEUDO"){
    std::mt19937_64 mtEngine(seed);
    std::normal_distribution<> nd;
    x = MatrixXd::Zero(num_credits,num_trials).unaryExpr([&](double dummy){return dummy + nd(mtEngine);});
  } else if (random_number_generation == "QUASI"){
    x = get_sobol_random_boost(num_trials,num_credits);
  }
  //auto m = x.mean();
  MatrixXd L( correlationMatrix.llt().matrixL() );
  auto y = (L * x).eval();
  std::default_random_engine generator(seed);
  MatrixXd corr_times = MatrixXd::Zero(num_credits, 2 * num_trials);
  for (auto itrial{0};itrial<num_trials;++itrial){
    std::chi_squared_distribution<double> distribution(degrees_of_freedom);
    double chi2 = distribution(generator);
    auto c = sqrt(chi2 / degrees_of_freedom);
    for (auto icredit{0};icredit<num_credits;++icredit){
      auto g = y(icredit, itrial) / c;
      boost::math::students_t boost_t_dist{degrees_of_freedom};
      auto u1 = boost::math::cdf(boost_t_dist,g);
      auto u2 = 1.0 - u1;
      auto times = issuer_curves.at(icredit).times_;
      auto values = issuer_curves.at(icredit).values_;
      auto t1 = uniform_to_default_time(u1, times, values);
      auto t2 = uniform_to_default_time(u2, times, values);
      corr_times(icredit,itrial) = t1;
      corr_times(icredit, itrial + num_trials) = t2;
    }
  }
  return corr_times;
}
