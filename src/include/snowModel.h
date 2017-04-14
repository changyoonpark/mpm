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

    double hardening(double J_P) const{
        return std::exp(std::min(xi * (1. - J_P), 20.));
    }

    // double workFunction(Matrix3x3& F_P, Matrix3x3& F_E, Matrix3x3& R_E) const {
    //     double J_E = F_E.det();
    //     double J_P = F_P.det();        
    //     return calcMu(J_P) * (F_E - R_E).norm2() + 
    //            0.5 * calcLambda(J_P) * (J_E - 1.) * (J_E - 1.);
    // }
    

    Matrix3x3 calcUglyMatrix1(double J, Matrix3x3& F_inv, Matrix3x3& delta_F) const;
    Matrix3x3 calcUglyMatrix2(double J, Matrix3x3& R, Matrix3x3& S, Matrix3x3& delta_F) const;

    Matrix3x3 hessianWrtF_doubleContractionDeltaF(double J_E, double J_P, 
                                                  Matrix3x3& F_E, 
                                                  Matrix3x3& R_E,
                                                  Matrix3x3& S_E,                                                  
                                                  Matrix3x3& F_E_inv, 
                                                  Matrix3x3& delta_F_E) const;
                                                  
    Matrix3x3 cauchyStress(double J, double J_P, double J_E, Matrix3x3& F, Matrix3x3& F_E, Matrix3x3& R_E, Matrix3x3& D) const;


    const double theta_compression;
    const double theta_stretch;
    const double muStick;
    const double E0, v0, mu0, lambda0;
    const double xi;
    const double alpha;

};

#endif 