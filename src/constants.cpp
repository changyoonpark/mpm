#include "include/constants.h"

Constants::Constants(std::string meshin){
    
        // snowModel = new SnowModel(0.99, //theta_compression
        //                           0.0001, //theta_stretch
        //                           0.1, //muStick
        //                           1.4E5, //Youngs Modulus
        //                           0.2, //Poisson Ratio
        //                           20., //xi
        //                           0.95 //FLIP contribution
        //                          );
        snowModel = new SnowModel(0.025, //theta_compression
                                  0.0075, //theta_stretch
                                  0.0, //muStick
                                  1.5e5, //Youngs Modulus
                                  0.2, //Poisson Ratio
                                  10., //xi
                                  0.95 //FLIP contribution
                                  );

        mesh = new Mesh(meshin);
        beta = 0.5;
        // h = 0.008333;
        h = 0.0125;
        // h = 0.01;
        // h = 0.0125;
        dt = 0.0001;
        dt2 = dt * dt;
        domainExtent = Vector3D(1.,1.,0.5);        
        bodyForce = Vector3D(0.,0.,-10.0);
    };
