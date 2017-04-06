#include "include/mesh.h"


Triangle::Triangle(Vector3D p1, Vector3D p2, Vector3D p3, Vector3D vel, size_t primID)
         : p1(p1), p2(p2), p3(p3), vel(vel), ID(primID){ 

  normal = cross(p2 - p1, p3 - p1);
  normal.normalize();

}

const Vector3D Triangle::getNormal(){
    return normal;
}

bool Triangle::getDistance(Vector3D& pos, double& dist){

  Vector3D e1 = p2 - p1, e2 = p3 - p1;
  
//   Vector3D s  = r.o - p1, s1 = cross(r.d,e2), s2 = cross(s,e1);
  Vector3D s  = pos - p1, s1 = cross(-normal,e2), s2 = cross(s,e1);
  Vector3D tbb(dot(s2,e2),dot(s1,s),dot(s2,-normal));
  tbb = tbb / (dot(s1,e1));

  double t = tbb.x;
  double b2 = tbb.y, b3 = tbb.z;
  double b1 = (1.0-b2-b3);

  //Not within the triangle boundaries. Not colliding.
  if (b2 < 0 || b3 < 0 || b1 < 0){
    return false;    
  } else{
    dist = t;
    return true;
  }
  
}

Mesh::Mesh(std::string s){

    std::string line;
    std::ifstream inputFile;
    inputFile.open(s);
    size_t ID = 0;

    if (inputFile.is_open())
    {
        std::cout << "Reading input mesh data..." << std::endl;
        while ( std::getline (inputFile,line) )
        {
            // std::cout << line << '\n';
            int n = 0;
            std::cout << "------------" << "Mesh Triangle : " << n << "------------" << std::endl;
            auto splittedLine = split(line,'|');

            Vector3D p1, p2, p3, v;
            for (std::string entry : splittedLine){
                auto splittedEntry = split(entry,',');
                if( n == 0)
                p1 = Vector3D(std::stod(splittedEntry[0]),
                              std::stod(splittedEntry[1]),
                              std::stod(splittedEntry[2]));
                else if (n == 1)
                p2 = Vector3D(std::stod(splittedEntry[0]),
                              std::stod(splittedEntry[1]),
                              std::stod(splittedEntry[2]));
                else if (n == 2)
                p3 = Vector3D(std::stod(splittedEntry[0]),
                              std::stod(splittedEntry[1]),
                              std::stod(splittedEntry[2]));
                else
                v = Vector3D(std::stod(splittedEntry[0]),
                             std::stod(splittedEntry[1]),
                             std::stod(splittedEntry[2])); 
                n ++;
            }
            Triangle TriToAppend = Triangle(p1,p2,p3,v,ID);
            ID ++;
            std::cout << "Triangle with vertices : " << p1 << p2 << p3 << "Added." << std::endl;
            triangles.push_back(TriToAppend);

        }
        inputFile.close();
    }        
}


void Mesh::updatePosition(double dt){
    for(auto& tri : triangles){
        tri.p1 += tri.vel * dt;
        tri.p2 += tri.vel * dt;
        tri.p3 += tri.vel * dt;
    }
}
