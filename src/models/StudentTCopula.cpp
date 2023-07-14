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
      auto times = issuer_curves[icredit].times_;
      auto values = issuer_curves[icredit].values_;
      auto t1 = StudentTCopula::uniform_to_default_time_student(u1, times, values);
      auto t2 = StudentTCopula::uniform_to_default_time_student(u2, times, values);
      corr_times(icredit,itrial) = t1;
      corr_times(icredit, itrial + num_trials) = t2;
    }
  }
  return corr_times;
}

double StudentTCopula::uniform_to_default_time_student(double u, const std::vector<double>& times, const std::vector<double>& values)
{
  if (u == 0.0)
    return 99999.0;
  if (u == 1.0)
    return 0.0;
  size_t num_points = times.size();
  size_t index = 0;
  for (size_t i{1};i<num_points;++i){
    if (u <= values[i - 1] && u > values[i]){
      index = i;
      break;
    }
  }
  double tau = 0.0;
  if (index == num_points + 1) {
    auto t1 = times[num_points - 1];
    auto q1 = values[num_points - 1];
    auto t2 = times[num_points];
    auto q2 = values[num_points];
    auto lam = log(q1 / q2) / (t2 - t1);
    tau = t2 - log(u / q2) / lam;
  } else if (index == 0){
    auto t1 = times.back();
    auto q1 = values.back();
    auto t2 = times[index];
    auto q2 = values[index];
    tau = (t1 * log(q2 / u) + t2 * log(u / q1)) / log(q2 / q1);
  } else {
    auto t1 = times[index - 1];
    auto q1 = values[index - 1];
    auto t2 = times[index];
    auto q2 = values[index];
    tau = (t1 * log(q2 / u) + t2 * log(u / q1)) / log(q2 / q1);
  }
  return tau;
}
