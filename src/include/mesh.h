#ifndef __MESH
#define __MESH

#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>
#include <string>

#include "helper.h"

using namespace myMath;

class Triangle{
 public:

  size_t ID;

  Triangle(Vector3D p1, Vector3D p2, Vector3D p3, Vector3D vel, size_t primID);
  Vector3D p1,p2,p3,vel,normal;
  // void draw() const;

  bool getDistance(Vector3D& pos, double& dist);  
  const Vector3D getNormal();
  

}; // class Triangle


class Mesh{

public:

    Mesh(std::string meshin);
    std::vector<Triangle> triangles;
    Triangle& getTri(size_t primID){
        return triangles[primID];
    }

    void updatePosition(double dt);

};

#endif