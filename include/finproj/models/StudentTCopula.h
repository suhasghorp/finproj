#ifndef FINPROJ_INCLUDE_FINPROJ_MODELS_STUDENTTCOPULA_H_
#define FINPROJ_INCLUDE_FINPROJ_MODELS_STUDENTTCOPULA_H_
#include <finproj/curves/CreditCurve.h>
#include <finproj/utils/Misc.h>
#include <Eigen/Dense>
using namespace Eigen;

class StudentTCopula {
 public:
  MatrixXd default_times(const std::vector<CreditCurve> &issuer_curves,
                            const MatrixXd& correlationMatrix,
                            float degrees_of_freedom,
                            int num_trials,
                            int seed);
};

#endif//FINPROJ_INCLUDE_FINPROJ_MODELS_STUDENTTCOPULA_H_
