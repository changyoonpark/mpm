#include "include/grid.h"
#include "include/helper.h"
#include "include/constants.h"
#include "include/mesh.h"


#include <math.h>
#include <assert.h>
#include <omp.h>

#define DBL_MAX 1.7976931348623158e+308

using namespace myMath;


/*--------------GRID CELL CLASS MEMEBERS DEFINITION--------------*/
GridCell::GridCell(GridNode* _node){
	pList.resize(MAX_PARTICLE_PER_CELL);
	node = _node;	
}


void GridCell::clearPList(){
	for(auto& p : pList) p = NULL;	
	nP = 0;
}

void GridCell::addToPList(Particle* p){	
	assert(nP <= MAX_PARTICLE_PER_CELL);
	
	pList[nP] = p;
	nP++;
	
}
/*---------------------------------------------------------------*/



/*--------------GRID NODE CLASS MEMEBERS DEFINITION--------------*/
GridNode::GridNode(int i, int j, int k, Grid* grid, Constants& _consts)
		 : idx    ( int3(i,j,k) ),
		   consts ( _consts ) 
{

	cell = new GridCell(this);
	x = Vector3D(i,j,k) * consts.h;
	_grid = grid;
}

GridNode* GridNode::neighborNode(int3& i){
	return _grid->idxNode(this->idx + i);
}

void GridNode::calcSignedDist(){
	double dist;
	signedDist = DBL_MAX;

	bool didContact = false;
	for( auto& tri : consts.mesh->triangles){
		if(tri.getDistance(x,dist)){
			if(dist < signedDist){
				contactingTriangle = &tri;
				signedDist = dist;		
				didContact = true;
			} 
		} 
	}

	if (!didContact) signedDist = DBL_MAX;

	if(signedDist < 0.) isInsideGeometry = true;
	else 			   isInsideGeometry = false;

	// std::cout << "Grid Node pos : " << x << std::endl;
	// std::cout << "SD : " << signedDist << std::endl;
	// std::cout << "Is inside : " << isInsideGeometry << std::endl;
 }

void GridNode::calcNodalForce(){
	force = force * 0.;
	for (auto& cell : nearCells){
		if (cell == NULL) continue;
		for (auto& particle : cell->pList){
			if (particle == NULL) break;			
			force = force + (particle->volume) * (particle->J) *
					   		consts.snowModel->cauchyStress(particle->J,particle->J_P,particle->J_E,particle->F, particle->F_E,particle->R_E, particle->D)
					   	   * gW(particle->pos);

		}
	}
	// force = force + mass * consts.bodyForce;
}

void GridNode::updateVelocityWithNodalForce(){
	velNext = vel + consts.dt * (consts.bodyForce - force / mass);
}

void GridNode::calcGeometryInteractions(){
		// std::cout << "vel : " << vel << std::endl;

	if (isInsideGeometry){		
		Vector3D relVel = velNext - contactingTriangle->vel;
		double vn = dot(relVel, contactingTriangle->getNormal());
		//material and boundary is coming together

		if ( vn  < 0. ){
			Vector3D vTan = relVel - vn * contactingTriangle->getNormal(); 
			double vTanLength = vTan.norm();

			// if (vel.norm() > 0.01) std::cout << vTanLength << std::endl;
			if(vTanLength <  - consts.snowModel->muStick * vn){
				relVel = Vector3D(0.,0.,0.);
			} else{
				relVel = vTan + consts.snowModel->muStick * vn * vTan / vTanLength;
			}

			velNext = relVel + contactingTriangle->vel;
		}

	} 

	// if (isInsideGeometry){
	// 	velNext = contactingTriangle->vel;
	// } 

}

void GridNode::calcDensity(){
	density0 = mass / (consts.h*consts.h*consts.h);
}

void GridNode::sampleMass(){
	mass = 0.;
	for (auto& cell : nearCells){
		if (cell == NULL) continue;
		for (auto& particle : cell->pList){
			if (particle == NULL) break;			
			mass += particle->mass * W(particle->pos);		
			// std::cout << particle->mass << std::endl;
			// std::cout << W(particle->pos) << std::endl;
		}
	}

}

void GridNode::sampleVelocity(){
	vel = vel * 0.;
	for (auto& cell :nearCells){
		if (cell == NULL) continue;
		for (auto& particle : cell->pList){
			if (particle == NULL) break;			
			vel += particle->vel * particle->mass * W(particle->pos);
		}
	}
	
	if (mass != 0.) vel = vel / mass;
	else vel = Vector3D(0.,0.,0.);

	// if (vel.z != 0) std::cout << "sampled vel : " << vel << std::endl;
}

/*---------------------------------------------------------------*/




/*--------------GRID NODE CLASS MEMEBERS DEFINITION--------------*/
void Grid::construct(){

	//Allocate Nodes and associated Cells
	for(int i = 0; i < gridDim.nx; i++){
	for(int j = 0; j < gridDim.ny; j++){
	for(int k = 0; k < gridDim.nz; k++){
		nodes[idx(i,j,k)] = new GridNode(i,j,k,this,constants);
	}}}

	//Assign Neighbor Cells.
	for(int i = 0; i < gridDim.nx; i++){
	for(int j = 0; j < gridDim.ny; j++){
	for(int k = 0; k < gridDim.nz; k++){

		for(int ii = -1; ii <= 2; ii++){
		for(int jj = -1; jj <= 2; jj++){
		for(int kk = -1; kk <= 2; kk++){

			GridCell* neigh;

			if (i+ii < 0 || j+jj < 0 || k+kk < 0) neigh = NULL;
			else if (i+ii >= gridDim.nx || j+jj >= gridDim.ny || k+kk >= gridDim.nz ) neigh = NULL;
			else neigh = nodes[idx(i+ii,j+jj,k+kk)]->cell;

			nodes[idx(i,j,k)]->nearCells.push_back(neigh);

		}}}
	}}}

}

const int3 Grid::hash(Particle& p){
	// std::cout << "FOOBAR" << std::endl;
	// std::cout << p.pos.x << "->" << (int) ceil(p.pos.x/constants.h) << std::endl;
	// std::cout << p.pos.y << "->" << (int) ceil(p.pos.y/constants.h) << std::endl;
	// std::cout << p.pos.z << "->" << (int) ceil(p.pos.z/constants.h) << std::endl;
	
	return int3((int) ceil(p.pos.x/constants.h), (int) ceil(p.pos.y/constants.h), (int) ceil(p.pos.z/constants.h));
}
 
GridNode* Grid::idxNode(int3 idxx){

	GridNode* ptr;

	if(idxx.nx < 0 || idxx.ny < 0 || idxx.nz < 0) ptr = NULL;
	else if(idxx.nx >= (gridDim.nx) || idxx.ny >= (gridDim.ny) || idxx.nz >= (gridDim.nz) ) ptr = NULL;
	else ptr = nodes[idx(idxx.nx,idxx.ny,idxx.nz)];

	return ptr;
}

inline unsigned int Grid::idx(int i, int j, int k){
	return (gridDim.nx * gridDim.ny) * k +  (gridDim.nx * j) + i;
}




//This Function is called every timestep.
void Grid::hashParticles(){
	activeNodes.clear();
	//Clear the pList for each Cell
	#pragma omp parallel for num_threads(THREADCOUNT)
	for(int i = 0; i < nodes.size(); i++){
		nodes[i]->isActive = false;
		nodes[i]->mass = 0.;
		nodes[i]->vel = nodes[i]->vel * 0.;
		nodes[i]->cell->clearPList();
	}

	#pragma omp parallel for num_threads(THREADCOUNT)
	for(int i = 0; i < pSet->particleSet.size(); i++){
		Particle& particle = pSet->particleSet[i];
		particle.hash = hash(particle);
		particle.cell = idxNode(particle.hash)->cell;
		particle.cell->addToPList(&particle);
	}

}

//This Function is called every timestep.
void Grid::rasterizeNodes(){
	
	#pragma omp parallel for num_threads(THREADCOUNT)
	for (int i = 0; i < nodes.size(); i++){
        nodes[i]->sampleMass();
		nodes[i]->sampleVelocity();
    }
	
}

void Grid::calculateNodalForcesAndUpdateVelocities(){
	std::cout << "updating nodal forces" << std::endl;
	#pragma omp parallel for num_threads(THREADCOUNT)

	for(int i=0;i<activeNodes.size();i++){
		auto dataIt = activeNodes.begin();
		std::advance(dataIt,i);
		GridNode* node = dataIt->second;

	// for(int i=0;i<nodes.size();i++){
	// 	GridNode* node = nodes[i];

		node->calcNodalForce();
		// node->calcImplicitNodalForce();
		node->updateVelocityWithNodalForce();
	}	

	std::cout << "done" << std::endl;

}


void Grid::calculateGeometryInteractions(){
	#pragma omp parallel for num_threads(THREADCOUNT)
	for(int i=0;i<activeNodes.size();i++){
		auto dataIt = activeNodes.begin();
		std::advance(dataIt,i);
		GridNode* node = dataIt->second;

	// for(int i=0;i<nodes.size();i++){
	// 	GridNode* node = nodes[i];


		node->calcGeometryInteractions();
	}
}

void Grid::calculateSignedDistance(){
	std::cout << "Initializing signed distance" << std::endl;
	#pragma omp parallel for num_threads(THREADCOUNT)
	for(int i=0;i<nodes.size();i++){
		GridNode* node = nodes[i];
		node->calcSignedDist();
	}
	std::cout << "done" << std::endl;
} 

Grid::Grid(Constants& _constants, ParticleSet* _pSet)

 : gridDim (int3((int)(_constants.domainExtent.x/_constants.h) + 1,
 				 (int)(_constants.domainExtent.y/_constants.h) + 1,
				 (int)(_constants.domainExtent.z/_constants.h) + 1)),
   constants( _constants )
{
	pSet = _pSet;
	nodes.resize(gridDim.nx * gridDim.ny * gridDim.nz);	
	std::cout << "Grid Dimensions : " << (gridDim.nx) << " x " << (gridDim.ny) << " x " << (gridDim.nz) << std::endl;
	std::cout << "Total Nodes : " << nodes.size() << std::endl;
	construct();
}
/*---------------------------------------------------------------*/
