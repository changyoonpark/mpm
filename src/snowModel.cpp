#include "include/snowModel.h"

SnowModel::SnowModel(double theta_compression,
                     double theta_stretch,
                     double muStick,
                     double E0,
                     double v0,
                     double xi,
                     double alpha) :
                     
                     theta_compression(theta_compression),
                     theta_stretch(theta_stretch),
                     muStick(muStick),
                     E0(E0),
                     v0(v0),
                     xi(xi),
                     alpha(alpha),
                     lambda0( E0 * v0 / ((1.+v0) * (1.-2.*v0))),
                     mu0(E0 / (2. * (1.+v0))) 
                     
                     {}

Matrix3x3 SnowModel::calcUglyMatrix1(double J, 
                                     Matrix3x3& F_inv, 
                                     Matrix3x3& delta_F) const {
    // calculates partial { JF^(-T)} / partial{F} : delta_F
    Matrix3x3 F_inv_T = F_inv.T();
    Matrix3x3 result;
    result.zero();

    for (int i = 0; i < 3; i++){
    for (int j = 0; j < 3; j++){
        for (int k = 0; k < 3; k++){
        for (int l = 0; l < 3; l++){
            result(i,j) += F_inv_T.tensorProductComponent(F_inv_T,i,j,k,l) * delta_F(k,l);            
            result(i,j) -= F_inv_T(i,k) * delta_F(l,k) * F_inv_T(l,j);
        }
        }
    }
    }

    return result * J;
}                     

Matrix3x3 SnowModel::calcUglyMatrix2(double J, 
                                     Matrix3x3& R,
                                     Matrix3x3& S,
                                     Matrix3x3& delta_F) const {
    // calculates delta_R
    Matrix3x3 V = R.T() * delta_F - delta_F.T() * R;

    Matrix3x3 A(Vector3D( S(0,0) + S(1,1), S(2,1)         , -S(0,2)          ),
                Vector3D( S(1,2)         , S(0,0) + S(2,2),  S(0,1)          ),
                Vector3D(-S(0,2)         , S(1,0)         ,  S(1,1) + S(2,2) ));

    Vector3D x = A.inv() * Vector3D(V(0,1),V(0,2),V(1,2));

    Matrix3x3 RTdR(Vector3D(     0.,    x.x, x.y ),
                    Vector3D(   -x.x,     0., x.z ),
                    Vector3D(   -x.y,   -x.z,   0.));
    return R * RTdR;
}


Matrix3x3 SnowModel::hessianWrtF_doubleContractionDeltaF(double J_E, double J_P, 
                                                Matrix3x3& F_E, 
                                                Matrix3x3& R_E,
                                                Matrix3x3& S_E,
                                                Matrix3x3& F_E_inv, 
                                                Matrix3x3& delta_F_E) const {

    double J_F_inv_T_delta_F = (J_E * F_E_inv.T()).doubleContraction(delta_F_E);

    Matrix3x3 delta_J_F_inv_T = calcUglyMatrix1(J_E,
                                                F_E_inv,
                                                delta_F_E);

    // std::cout << R_E << std::endl;
    // std::cout << S_E << std::endl;

    Matrix3x3       delta_R_E = calcUglyMatrix2(J_E,
                                                R_E,
                                                S_E,
                                                delta_F_E);



    return  (2. * mu0 * delta_F_E - 
                2. * mu0 * delta_R_E +
                lambda0 * J_E * F_E_inv.T() * J_F_inv_T_delta_F +
                lambda0 * (J_E - 1.) * delta_J_F_inv_T ) * hardening(J_P);

}

Matrix3x3 SnowModel::cauchyStress(double J, double J_P, double J_E, Matrix3x3& F, Matrix3x3& F_E, Matrix3x3& R_E, Matrix3x3& D) const {
    
    // Matrix3x3 I = F.identity();
    // Matrix3x3 strain = 0.5 * (F.T() * F - I);

    // double trace = strain(0,0) + strain(1,1) + strain(2,2); 
    // return lambda0 * trace * I + 2.0 * mu0 * strain;

    Matrix3x3 I = F_E.identity();
    // std::cout << hardening << std::endl;
    return hardening(J_P) * (2. * (mu0 / J) * (F_E - R_E) * F_E.T() + (lambda0 / J) * (J_E - 1.) * J_E * I);
}