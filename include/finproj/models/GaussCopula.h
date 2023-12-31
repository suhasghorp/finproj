#ifndef FINPROJ_INCLUDE_FINPROJ_MODELS_GAUSSCOPULA_H_
#define FINPROJ_INCLUDE_FINPROJ_MODELS_GAUSSCOPULA_H_
#include <finproj/curves/CreditCurve.h>
#include <finproj/utils/Misc.h>
#include <Eigen/Dense>
using namespace Eigen;

class GaussCopula {
 public:
  static MatrixXd default_times_gc(const std::vector<CreditCurve> &issuer_curves,
                            const MatrixXd& correlationMatrix,
                            int num_trials,
                            int seed,
                            const std::string& random_number_generation) ;

};



#endif//FINPROJ_INCLUDE_FINPROJ_MODELS_GAUSSCOPULA_H_
