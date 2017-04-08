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
                                  0.1, //muStick
                                  1.5e5, //Youngs Modulus
                                  0.2, //Poisson Ratio
                                  20., //xi
                                  0.95 //FLIP contribution
                                 );

        mesh = new Mesh(meshin);
        h = 0.025;
        dt = 0.0005;
        domainExtent = Vector3D(1.,1.,1.);        
        bodyForce = Vector3D(0.,0.,-20.0);
    };