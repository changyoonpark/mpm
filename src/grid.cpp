#include "include/grid.h"
#include "include/helper.h"
#include "include/constants.h"
#include "include/mesh.h"


#include <math.h>
#include <assert.h>
#include <omp.h>

#define DBL_MAX 1.7976931348623158e+308
#define EPS_D_SMALL (1.E-100)

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

Vector3D GridNode::getHessianSum(Vector3D& delta_u){
	Vector3D sum(0,0,0);
	for (auto& cell : nearCells){
		if (cell == NULL) continue;
		for (auto& particle : cell->pList){
			if (particle == NULL) break;						
			sum += particle->volume * particle->calculateAMatrix(delta_u) * (particle->F_E.T() * gW(particle->pos));
		}
	}
	// if(sum == 0) std::cout << "wtf something wrong? : "<< std::endl;
	return sum;	
}

void GridNode::updateVelocityWithNodalForce(){
	velNext = vel + consts.dt * (consts.bodyForce - force / (mass + EPS_D_SMALL));
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
			relVel = Vector3D(0.,0.,0.);
			// if(vTanLength <  - consts.snowModel->muStick * vn){
			// 	relVel = Vector3D(0.,0.,0.);
			// } else{
			// 	relVel = vTan + consts.snowModel->muStick * vn * vTan / vTanLength;
			// }

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
	
	if (mass != 0) vel = vel / mass;
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
	
	return int3((int) ceil((p.pos.x-1.E-10)/constants.h), (int) ceil((p.pos.y-1.E-10)/constants.h), (int) ceil((p.pos.z-1.E-10)/constants.h));
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
	std::cout << "hashing particles..." << std::endl;
	#pragma omp parallel for num_threads(THREADCOUNT)
	for(int i = 0; i < nodes.size(); i++){
		// nodes[i]->isActive = false;
		nodes[i]->isActive = true;
		nodes[i]->mass = 0.;
		nodes[i]->vel = Vector3D(0,0,0);
		nodes[i]->cell->clearPList();
	}
	std::cout << "adding particles to cell..." << std::endl;
	#pragma omp parallel for num_threads(THREADCOUNT)
	for(int i = 0; i < pSet->particleSet.size(); i++){
		Particle& particle = pSet->particleSet[i];
		particle.hash = hash(particle);
		// printf("particle hash : %d, %d, %d\n",particle.hash.nx,particle.hash.ny,particle.hash.nz);		
		if(particle.hash.nx >= (int)(constants.domainExtent.x / constants.h) || 
		   particle.hash.ny >= (int)(constants.domainExtent.y / constants.h) || 
		   particle.hash.nz >= (int)(constants.domainExtent.z / constants.h) ||
		   particle.hash.nx < 0 || particle.hash.ny < 0 || particle.hash.nz < 0){
			printf("rouge particle with particle hash : %d, %d, %d\n",particle.hash.nx,particle.hash.ny,particle.hash.nz);
			particle.isActive = false;
			// continue;
			printf("reduce timestep and try again.");
			assert(false);
		}

		particle.cell = idxNode(particle.hash)->cell;
		particle.cell->addToPList(&particle);
	}
	std::cout << "done" << std::endl;
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
	// for(int i=0;i<activeNodes.size();i++){
	// 	auto dataIt = activeNodes.begin();
	// 	std::advance(dataIt,i);
	// 	GridNode* node = dataIt->second;

	for(int i=0;i<nodes.size();i++){
		GridNode* node = nodes[i];

		node->calcNodalForce();
		// node->calcImplicitNodalForce();
		node->updateVelocityWithNodalForce();
	}	

	std::cout << "done" << std::endl;

}

void Grid::solveForVelNextAndUpdateVelocities(){
	//Initialize values for Conjugate Residual
	std::cout << "Performing Implicit Velocity Update" << std::endl;

	double residualSum = 0;
	std::vector<double> residuals(nodes.size(),0);

	#pragma omp parallel for num_threads(THREADCOUNT) reduction(+:residualSum)	
	// for(int i=0;i<activeNodes.size();i++){
	// 	auto dataIt = activeNodes.begin();
	// 	std::advance(dataIt,i);
	// 	GridNode* node = dataIt->second;		

		for(int i=0;i<nodes.size();i++){
			GridNode* node = nodes[i];

		// if(node->mass > EPS_D){
		
			node->r     =  -(node->consts.beta * node->consts.dt2 / (node->mass + EPS_D_SMALL)) * node->getHessianSum(node->velNext);
			node->s     =  node->r + (node->consts.beta * node->consts.dt2 / (node->mass + EPS_D_SMALL)) * node->getHessianSum(node->r);
			node->p     =  node->r;
			node->q     =  node->s;
			node->gamma =  dot(node->r,node->s);		

			node->alpha   = node->gamma / (dot(node->q,node->q) + EPS_D_SMALL);
			residuals[i] = node->alpha * node->alpha * node->p.norm2();

			if (residuals[i] != residuals[i]){
			std::cout << node->gamma << std::endl;			
			std::cout << "velnext : " << node->velNext << std::endl;
			std::cout << "mass : " << node->mass << std::endl;
			std::cout << "-(node->consts.beta * node->consts.dt2 / (node->mass + EPS_D_SMALL)) : " << -(node->consts.beta * node->consts.dt2 / (node->mass + EPS_D_SMALL)) << std::endl;
			std::cout << "node->getHessianSum(node->velNext) : " << node->getHessianSum(node->velNext) << std::endl;			
			std::cout << "r : " << node->q << std::endl;
			std::cout << "s : " << node->q << std::endl;
			std::cout << "p : " << node->q << std::endl;			
			std::cout << "q : " << node->q << std::endl;
			std::cout << (dot(node->q,node->q)) << std::endl;
			std::cout << node->alpha << std::endl;
			std::cout << "---" << std::endl;
			}
		// } else{
		// 	node->r = node->r * 0;
		// 	node->s = node->s * 0;
		// 	node->p = node->p * 0;
		// 	node->q = node->q * 0;
		// 	node->gamma = 0;
		// 	node->alpha = 0;
		// 	residuals[i] = 0;
		// }

		residualSum += residuals[i];

	}	

	int iterCount = 0;
	// if (residualSum > 0.1)
	std::cout << "initial residual sum : " << residualSum << std::endl;


	//Start iterations
	while(residualSum > 1.E-40 && (iterCount < 10) ){

		residualSum = 0;

		#pragma omp parallel for num_threads(THREADCOUNT) reduction(+:residualSum)
		// for(int i=0;i<activeNodes.size();i++){
		// 	auto dataIt = activeNodes.begin();
		// 	std::advance(dataIt,i);
		// 	GridNode* node = dataIt->second;

			for(int i=0;i<nodes.size();i++){
				GridNode* node = nodes[i];

			// if(node->mass > EPS_D){

				node->alpha   = node->gamma / (dot(node->q,node->q) + EPS_D_SMALL);

				residuals[i] = node->alpha * node->alpha * node->p.norm2();

				node->velNext = node->velNext + node->alpha * node->p;
				node->r       = node->r - node->alpha * node->q;
				node->s       = node->r + (node->consts.beta * node->consts.dt2 / (node->mass + EPS_D_SMALL)) * node->getHessianSum(node->r);
				node->beta    = dot(node->r,node->s) / (node->gamma + EPS_D_SMALL);
				node->gamma   = node->beta * node->gamma;
				node->p       = node->r + node->beta * node->p;
				node->q       = node->s + node->beta * node->q;

			// } else{

			// 	node->r = node->r * 0;
			// 	node->s = node->s * 0;
			// 	node->p = node->p * 0;
			// 	node->q = node->q * 0;
			// 	node->gamma = 0;
			// 	node->alpha = 0;
			// 	residuals[i] = 0;				

			// }

			residualSum += residuals[i];

		}
		std::cout << "Conjugate Residual : " << residualSum << std::endl;

		iterCount ++;
	}
	std::cout << "Convergence Criterion Met." << std::endl;

}

void Grid::calculateGeometryInteractions(){
	std::cout << "calculating geometry interactions" << std::endl;
	#pragma omp parallel for num_threads(THREADCOUNT)
	// for(int i=0;i<activeNodes.size();i++){
	// 	auto dataIt = activeNodes.begin();
	// 	std::advance(dataIt,i);
	// 	GridNode* node = dataIt->second;
	for(int i=0;i<nodes.size();i++){
		GridNode* node = nodes[i];

		node->calcGeometryInteractions();
	}
	std::cout << "done" << std::endl;
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
