#ifndef __CONSTANTS
#define __CONSTANTS

#include "mesh.h"
#include "snowModel.h"

#include <iostream>

#define THREADCOUNT 4

struct Constants{

    Constants(std::string meshin);

    double h;
    double dt;

    Vector3D domainExtent;
    Vector3D bodyForce;
    Mesh* mesh;

    const SnowModel* snowModel;


};

#endif