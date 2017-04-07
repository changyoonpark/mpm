#ifndef __SNOW
#define __SNOW

#include "helper.h"
#include <math.h>
#include <iostream>

using namespace myMath;


class SnowModel{

public:
    SnowModel(double theta_compression,
              double theta_stretch,
              double muStick,
              double E0,
              double v0,
              double xi,
              double alpha);

// Snow Parameters

    double calcMu(double J_P) const {
        return mu0 * std::exp(std::min(xi * (1. - J_P), 20.));
    }

    double calcLambda(double J_P) const {
        return lambda0 * std::exp(std::min(xi * (1. - J_P), 20.));
    }

    // double workFunction(Matrix3x3& F_P, Matrix3x3& F_E, Matrix3x3& R_E) const {
    //     double J_E = F_E.det();
    //     double J_P = F_P.det();        
    //     return calcMu(J_P) * (F_E - R_E).norm2() + 
    //            0.5 * calcLambda(J_P) * (J_E - 1.) * (J_E - 1.);
    // }

    Matrix3x3 cauchyStress(double J, double J_P, double J_E, Matrix3x3& F, Matrix3x3& F_E, Matrix3x3& R_E, Matrix3x3& D) const {
        
        // Matrix3x3 I = F.identity();
        // Matrix3x3 strain = 0.5 * (F.T() * F - I);

        // double trace = strain(0,0) + strain(1,1) + strain(2,2); 
        // return lambda0 * trace * I + 2.0 * mu0 * strain;

        Matrix3x3 I = F_E.identity();

        return 2. * (calcMu(J_P) / J) * (F_E - R_E) * F_E.T() + (calcLambda(J_P) / J) * (J_E - 1.) * J_E * I;
    }

    const double theta_compression;
    const double theta_stretch;
    const double muStick;
    const double E0, v0, mu0, lambda0;
    const double xi;
    const double alpha;

};

#endif 