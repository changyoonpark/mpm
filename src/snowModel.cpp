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
                     lambda0(E0 * v0 / ((1.+v0) * (1.-2.*v0))),
                     mu0(E0 / (2. * (1.+v0))) 
                     
                     {}
                     