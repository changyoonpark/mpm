#ifndef __INITER
#define __INITER

#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include "helper.h"



using namespace myMath;

class InitData{
    public:
        InitData(std::string s);
        unsigned int n;
        std::vector<std::map<std::string,Vector3D> > data;    
};

#endif // INITER