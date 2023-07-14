#include <finproj/models/GaussCopula.h>
#include <finproj/curves/CDS.h>
#include <finproj/utils/Misc.h>
#include <random>

/*MatrixXd get_sobol_random_ql(const int num_trials, const int num_credits) {
  MatrixXd x = MatrixXd::Zero(num_trials, num_credits);
  MoroInverseCumulativeNormal invGauss;

  SobolRsg sobolEngine(5);
  for (int t{0}; t < num_trials;++t){
    std::vector<double> sobols = sobolEngine.nextSequence().value;
    std::vector<double> normals{};
    for (double d : sobols)
      normals.push_back(invGauss(d));
    double* ptr_data = &normals[0];
    Eigen::VectorXd v2 = Eigen::Map<Eigen::VectorXd, Eigen::Unaligned>(ptr_data, normals.size());
    x.row(t) = v2;
  }
  x.transposeInPlace();
  return x;
}*/



MatrixXd GaussCopula::default_times_gc(const std::vector<CreditCurve>& issuer_curves,
                                       const MatrixXd& correlationMatrix,
                                       const int num_trials,
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
  MatrixXd corr_times = MatrixXd::Zero(num_credits, 2 * num_trials);

  for (auto i{0};i<num_credits;++i){
    for (auto t{0}; t < num_trials;++t){
      auto g = y(i, t);
      auto u1 = 1.0 - N(g);
      auto u2 = 1.0 - u1;
      auto times = issuer_curves[i].times_;
      auto values = issuer_curves[i].values_;
      auto t1 = uniform_to_default_time(u1, times, values);
      auto t2 = uniform_to_default_time(u2, times, values);
      corr_times(i,t) = t1;
      corr_times(i, num_trials + t) = t2;
    }
  }
  return corr_times;
}