#ifndef FINPROJ_INCLUDE_FINPROJ_MODELS_STUDENTTCOPULA_H_
#define FINPROJ_INCLUDE_FINPROJ_MODELS_STUDENTTCOPULA_H_
#include <finproj/curves/CreditCurve.h>
#include <finproj/utils/Misc.h>
#include <Eigen/Dense>
using namespace Eigen;

class StudentTCopula {
 public:
  static MatrixXd default_times(const std::vector<CreditCurve> &issuer_curves,
                            const MatrixXd& correlationMatrix,
                            float degrees_of_freedom,
                            int num_trials,
                            int seed,
                         const std::string& random_number_generation) ;
  static double uniform_to_default_time_student(double u, const std::vector<double>& times, const std::vector<double>& values);
};

#endif//FINPROJ_INCLUDE_FINPROJ_MODELS_STUDENTTCOPULA_H_
