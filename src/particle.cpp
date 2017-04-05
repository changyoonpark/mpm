#include "include/particle.h"
#include "include/constants.h"
#include <omp.h>

void Particle::collectVelocity(velType veltype){

    for(int ii = -2; ii <= 1; ii++){
    for(int jj = -2; jj <= 1; jj++){
    for(int kk = -2; kk <= 1; kk++){
        
        int3 nidx = int3(ii,jj,kk);
        GridNode* gn = cell->node->neighborNode(nidx);
        if (gn == NULL){ continue; }

        if(veltype == NOW){ velNowCollected += gn->vel * gn->W(pos); }
        else if(veltype == NEXT){ velNextCollected += gn->velNext * gn->W(pos); }
    }    
    }
    }
}
// x:4.5,4.5,4.0 v:0,0,-1 m:1.0,0,0
// x:4.5,5.0,4.0 v:0,0,-1 m:1.0,0,0
// x:4.5,5.5,4.0 v:0,0,-1 m:1.0,0,0
// x:5.0,4.5,4.0 v:0,0,-1 m:1.0,0,0
// x:5.0,5.0,4.0 v:0,0,-1 m:1.0,0,0
// x:5.0,5.5,4.0 v:0,0,-1 m:1.0,0,0
// x:5.5,4.5,4.0 v:0,0,-1 m:1.0,0,0
// x:5.5,5.0,4.0 v:0,0,-1 m:1.0,0,0
// x:5.5,5.5,4.0 v:0,0,-1 m:1.0,0,0

void Particle::calculateSignedDistance(){

    for(int ii = -2; ii <= 1; ii++)
    for(int jj = -2; jj <= 1; jj++)
    for(int kk = -2; kk <= 1; kk++){
        
        int3 nidx = int3(ii,jj,kk);
        GridNode* gn = cell->node->neighborNode(nidx);
        if (gn == NULL) continue;
        signedDist += gn->signedDist * gn->W(pos);
        gradSignedDist += gn->contactingTriangle->getNormal() * gn->W(pos);
    }    
    
    if (signedDist < 0.){
        isInsideGeometry = true;
    } else{
        isInsideGeometry = false;
    }
}

void Particle::calculateDensityAndVolume(){
    density = 0.;
    // std::cout << "pid : " << pID << " at : " << pos << " cell node : ";
    // std::cout << cell->node->idx.nx  << ", " << cell->node->idx.ny << ", " << cell->node->idx.nz << std::endl;
    for(int ii = -2; ii <= 1; ii++){
    for(int jj = -2; jj <= 1; jj++){
    for(int kk = -2; kk <= 1; kk++){
        
        int3 nidx = int3(ii,jj,kk);
        GridNode* gn = cell->node->neighborNode(nidx);
        if (gn == NULL) continue;
        density += gn->mass * gn->W(pos);

    }}}

    density = density / (consts.h * consts.h * consts.h );
    volume = mass / density;
    printf("Volume = %f, Density = %f\n",volume,density);
}

void Particle::calculateVelocityGradient(){
    gradVelocity = 0. * gradVelocity;
    Vector3D gW;

    //Index through the surrounding nodes.
    for(int ii = -2; ii <= 1; ii++)
    for(int jj = -2; jj <= 1; jj++)
    for(int kk = -2; kk <= 1; kk++){
        int3 nidx = int3(ii,jj,kk);
        GridNode* gn = cell->node->neighborNode(nidx);
        if (gn == NULL) continue;
        gW = gn->gW(pos);
        gradVelocity += Matrix3x3(gn->velNext.x * gW,
                                  gn->velNext.y * gW,
                                  gn->velNext.z * gW);       
    }    

}

void Particle::updateVelNext(){
    velNextCollected = Vector3D(0.,0.,0.);
    velNowCollected = Vector3D(0.,0.,0.);

    collectVelocity(NEXT);
    collectVelocity(NOW);

    // std::cout 
    // if (velNowCollected.z < 0){
    // std::cout << "----" << std::endl;
    // std::cout << velNowCollected << std::endl;
    // std::cout << velNextCollected << std::endl;
    // }
    
    velPIC = velNextCollected;
    velFLIP = (vel + velNextCollected - velNowCollected);
    velNext = (1. - consts.snowModel->alpha ) * velPIC + consts.snowModel->alpha * velFLIP;
    // velNext = velFLIP;
    // velNext = 0.5 * velNowCollected + 0.5 * velNextCollected;
    // std::cout << "********" << std::endl;
    // std::cout << "velPIC : " << velPIC << std::endl;
    // std::cout << "velFLIP : " << velFLIP << std::endl;
    // std::cout << "vel : " << vel << std::endl;
    // std::cout << "velNext : " << velNext << std::endl;

}

void Particle::calculateGeometryInteraction(){

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

}

void Particle::updateDeformationGradient(){
    // Simple update.
    // D = gradVelocity * F;
    // F = F + consts.dt * D;
    // J = F.det();
    // J_E = J;
    // F_E = F;
    // R_E = R_E.identity();
    // F_P = F_P.identity();
    // J_P = 1.;

    // Plastic Update.
    F_E = ( F_E.identity() + consts.dt * gradVelocity ) * F_E;
    F = F_E * F_P;

    F_E.singularValueDecompose(U,SIGMA,V);
    R_E = U * V.T();

    SIGMA.clamp(1. - consts.snowModel->theta_compression,
                1. + consts.snowModel->theta_stretch     );

    F_E = U * SIGMA * V.T();
    F_P = V * SIGMA.inv() * U.T() * F;
    F = F_E * F_P;

    J_E = F_E.det();
    J_P = F_P.det();
    J = F.det();


}

void Particle::initParticle(){
    velNext = Vector3D(0.,0.,0.);
    velFLIP = Vector3D(0.,0.,0.);
    velPIC = Vector3D(0.,0.,0.);
    velNowCollected = Vector3D(0.,0.,0.);
    velNextCollected = Vector3D(0.,0.,0.);
}


ParticleSet::ParticleSet(Constants& _consts, InitData& dat)
    : consts ( _consts )
 {
    unsigned int n = 0;
    Vector3D pos,vel;    
    std::cout << "Initializing " << dat.data.size() << " Particles..." << std::endl;    
    for(auto entry : dat.data){
        particleSet.push_back(Particle(n,entry,consts));
        n++;
    }    
}


void ParticleSet::updateParticleSignedDistance(){
	#pragma omp parallel for num_threads(THREADCOUNT)
	for (int i = 0; i < particleSet.size(); i++){
        particleSet[i].calculateSignedDistance();    
    }    
}

void ParticleSet::estimateParticleVolume(){
    // std::cout << "size : " << particleSet.size() << std::endl;
	#pragma omp parallel for num_threads(THREADCOUNT)
	for (int i = 0; i < particleSet.size(); i++){
        particleSet[i].calculateDensityAndVolume();    
    }
}

void ParticleSet::calculateParticleVelocityGradient(){
	#pragma omp parallel for num_threads(THREADCOUNT)
	for (int i = 0; i < particleSet.size(); i++){
        particleSet[i].calculateVelocityGradient();    
    }
}

void ParticleSet::updateParticlePosition(){

	#pragma omp parallel for num_threads(THREADCOUNT)
	for (int i = 0; i < particleSet.size(); i++){
        particleSet[i].updateVelNext();    
    }

	#pragma omp parallel for num_threads(THREADCOUNT)
 	for (int i = 0; i < particleSet.size(); i++){
        particleSet[i].force = particleSet[i].mass * (particleSet[i].velNext - particleSet[i].vel)/consts.dt;      
        particleSet[i].vel = particleSet[i].velNext;
        particleSet[i].pos += particleSet[i].vel * consts.dt;
        particleSet[i].initParticle();
    }
 
}


void ParticleSet::updateParticleDeformationGradient(){
	#pragma omp parallel for num_threads(THREADCOUNT)    
	for (int i = 0; i < particleSet.size(); i++){        
        particleSet[i].updateDeformationGradient();    
    }    
    
}

void ParticleSet::calculateGeometryInteractions(){
	#pragma omp parallel for num_threads(THREADCOUNT)    
	for (int i = 0; i < particleSet.size(); i++){        
        particleSet[i].calculateGeometryInteraction();    
    }    
    
}
