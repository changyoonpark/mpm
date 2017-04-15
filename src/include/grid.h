#ifndef __GRID
#define __GRID

#include <iostream>
#include "helper.h"
#include "particle.h"
#include "constants.h"
#include "mesh.h"

#define MAX_PARTICLE_PER_CELL 20


using namespace myMath;

class Particle;
class ParticleSet;
class GridNode;
class Grid;
class GridCell;

class GridCell{
	public:
		GridCell(GridNode* _node);
		GridNode* node;
		int nP;
		std::vector<Particle*> pList;

		void clearPList();
		void addToPList(Particle* p);
};




//All the nodal rasterization happens here.
class GridNode{

	public:

		GridNode(int i, int j, int k, Grid* grid, Constants& _consts);

		Constants& consts;

		Vector3D x;
		int3 idx;
		GridCell* cell;	
		Grid* _grid;
		std::vector<GridCell*> nearCells;
		
		double mass;
		double density0;
		
		bool isActive;
		bool isInsideGeometry;
		Triangle* contactingTriangle;
		double signedDist;

		Vector3D vel;
		Vector3D velNext;
		Vector3D force;

		Vector3D r;
		Vector3D s;
		Vector3D p;
		Vector3D q;
		double gamma;
		double alpha,beta;


		void sampleMass();
		void sampleVelocity();
		void calcSignedDist();
		void calcDensity();
		void calcGeometryInteractions();
		void calcNodalForce();
		void updateVelocityWithNodalForce();
		Vector3D getHessianSum(Vector3D& delta_u);

		GridNode* neighborNode(int3& i);

		inline double W(Vector3D xp) const {
			return N((xp.x - x.x) / consts.h) * N((xp.y - x.y) / consts.h) * N((xp.z - x.z) / consts.h);
		}

		inline Vector3D gW(Vector3D xp) const {
			double xeval = (xp.x - x.x) / consts.h;
			double yeval = (xp.y - x.y) / consts.h;
			double zeval = (xp.z - x.z) / consts.h;
			return Vector3D(  (1. / consts.h) * Nx(xeval) * N (yeval) * N (zeval),
							  (1. / consts.h) * N (xeval) * Nx(yeval) * N (zeval),
							  (1. / consts.h) * N (xeval) * N (yeval) * Nx(zeval));
		}

	private:


		inline double N(double _x) const {
			double abx = std::abs(_x);
			if (abx < 1. && abx >= 0.)      return 0.5 * abx * abx * abx - abx * abx + 2./3.;
			else if (abx >= 1. && abx < 2.) return - (1. / 6.) * abx * abx * abx + abx * abx - 2. * abx + 4./3.;
			else return 0.;			
		}

		inline double Nx(double _x) const {
			double abx = std::abs(_x);
			double sign = (_x >= 0 ? 1.0 : -1.0);
			if (abx < 1. && abx >= 0.)      return sign * (3./2. * abx * abx - 2. * abx);
			else if (abx >= 1. && abx < 2.) return sign * (- 0.5 * abx * abx + 2. * abx - 2.);
			else return 0.;			
		}



};

class Grid{
	public:
		Grid(Constants& _constants, ParticleSet* _pSet);	
		
		Constants& constants;		
		const int3 gridDim;

		ParticleSet* pSet;
		std::vector<GridNode* > nodes;
		std::map<std::tuple<int,int,int>,GridNode*> activeNodes;
		std::map<std::tuple<int,int,int>,GridNode*>::iterator dataIt;
		void hashParticles();
		void rasterizeNodes();
		void solveForVelNextAndUpdateVelocities();
		void calculateSignedDistance();
		void calculateGeometryInteractions();
		void calculateNodalForcesAndUpdateVelocities();


		GridNode* idxNode(int3 idx);
		inline unsigned int idx(int i,int j,int k);

	private:

		void construct();
		const int3 hash(Particle& p);
		
};

#endif // GRID