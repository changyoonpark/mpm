#ifndef __PARTICLE
#define __PARTICLE

#include <iostream>
#include "helper.h"
#include "initer.h"
#include "grid.h"
#include "constants.h"
#include "mesh.h"

using namespace myMath;

enum velType {
    NOW,
    NEXT
};

class GridCell;

struct Particle{

    public: 

        Constants& consts;

        unsigned int pID;

        double mass;
        double volume;
        double density;

        double J,J_P,J_E;

        Vector3D vel,velNext;
        Vector3D velFLIP, velPIC;
        Vector3D force;

        Vector3D velNowCollected,velNextCollected;

        Vector3D pos;

        Vector3D X_initial;
        Vector3D x_current;

        Matrix3x3 F,F_P,F_E,D;
        

        Matrix3x3 U;
        Matrix3x3 V;
        Matrix3x3 SIGMA;

        Matrix3x3 R_E;
        Matrix3x3 S_E;

        Matrix3x3 gradVelocity;

        int3 hash;
        GridCell* cell;

        double signedDist;
        Vector3D gradSignedDist;
        void calculateSignedDistance();
        void collectVelocity(velType veltype);
        void calculateDensityAndVolume();
        void calculateVelocityGradient();
        void updateDeformationGradient();
        void calculateSingularValueDecompose();
        void calculateContactState();
        void calculateGeometryInteraction();
        void updateVelNext();
        void updateWithPIC();
        void updateWithFLIP();

        void initParticle();



        Particle(unsigned int _pID, std::map<std::string,Vector3D> iniDat, Constants& _consts)
                : hash  ( int3(0,0,0) ),
                consts( _consts ) 
        {
            
            F  =  F  .identity();
            F_E = F_E.identity();
            F_P = F_P.identity();       

            J = 1.;
            J_E = 1.;
            J_P = 1.;

            pID = _pID;
            volume = 0.;
            mass = iniDat["m"].x;
            vel = iniDat["v"];
            pos = iniDat["x"];
        }

    private:

		bool isInsideGeometry;
		Triangle* contactingTriangle;
        
};

class ParticleSet{
    public:
        ParticleSet(Constants& consts, InitData& dat);
        std::vector<Particle> particleSet;
        Constants& consts;

        void updateParticleSignedDistance();
        void estimateParticleVolume();
        void calculateParticleVelocityGradient();
        void calculateGeometryInteractions();
        void updateParticleDeformationGradient();      
        void updateParticlePosition();
};

#endif